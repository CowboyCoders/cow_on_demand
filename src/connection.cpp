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
