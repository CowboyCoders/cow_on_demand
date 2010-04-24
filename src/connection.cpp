/*
Copyright 2010 CowboyCoders. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COWBOYCODERS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COWBOYCODERS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of CowboyCoders.
*/

#include <vector>
#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

#include "connection.hpp"
#include "req_parser.hpp"

connection::connection(boost::asio::io_service& io_service,
                       const std::string& document_root)
    : strand_(io_service), document_root_(document_root),
      socket_(io_service), reader_(MAX_REQUEST_SIZE)
{
}

void connection::start()
{
    socket_.async_read_some(boost::asio::buffer(buffer_),
                            strand_.wrap(
                                    boost::bind(&connection::handle_read, shared_from_this(),
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred)));
}

void connection::handle_read(const boost::system::error_code& e,
                             std::size_t bytes_transferred)
{
    if(!e) {
        req_parser parser;

        std::stringstream req;
        req << std::endl;

        for(size_t i = 0; i < bytes_transferred; ++i) {
            req << buffer_[i];
            parser.consume(buffer_[i]);
        }

        BOOST_LOG_TRIVIAL(info) << "connection: request was: " << req.str();

        // Massage URI
        bool bad_uri = false;

        std::string filename(parser.file());

        if(filename.find_first_of('/') == 0 || filename.find("..") != std::string::npos ||
           filename.find("./") != std::string::npos) {
            bad_uri = true;
            BOOST_LOG_TRIVIAL(error) << "error: BAD request. Possible hacking attempt";
        }

        if(!bad_uri) {
            std::string path(document_root_ + filename);
            if(reader_.read(path, parser.size(), parser.indices())) {

                std::stringstream ss;
                ss << "HTTP/1.0 200 OK\r\nContent-Length: "
                        << reader_.data_size() << "\r\n\r\n";

                std::string reply = ss.str();

                size_t buf_size = reply.length() + reader_.data_size();
                buf = new char[buf_size];

                size_t counter = 0;
                for(size_t i = 0; i < reply.length(); ++i) {
                    buf[counter++] = reply[i];
                }

                BOOST_LOG_TRIVIAL(debug) << "Sending " << reader_.data_size()
                        << " bytes";

                for(size_t i = 0; i < reader_.data_size(); ++i) {
                    buf[counter++] = (reader_.chunk())[i];
                }

                boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
                    strand_.wrap(
                        boost::bind(&connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error)));
            } else {
                // FAILURE
            }
        }
    }
}

void connection::handle_write(const boost::system::error_code& e)
{
    delete [] buf;

    if (!e) {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }  else  {
        BOOST_LOG_TRIVIAL(error) << e.message();
    }

    // No new asynchronous operations are started. This means that all shared_ptr
    // references to the connection object will disappear and the object will be
    // destroyed automatically after this handler returns. The connection class's
    // destructor closes the socket.
}
