/*
Copyright 2010 CowboyCoders. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COWBOYCODERS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COWBOYCODERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of CowboyCoders.
*/

#ifndef ___CONNECTION_H___ 
#define ___CONNECTION_H___ 

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "chunk_reader.hpp"

class connection
    : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
    /**
     * Creates a new connection.
     *
     * @param io_service A worker for read jobs.
     * @param document_root The root catalog where all the documents are.
     */
    explicit connection(boost::asio::io_service& io_service, const std::string& document_root);

    /**
     * Returns the socket which the connection sends data to.
     * @return The TCP socket.
     */
    boost::asio::ip::tcp::socket& socket()
    {
        return socket_;
    }

    /**
     * Starts the connection.
     */
    void start();

    /**
     * The maximum size of a request (in bytes).
     */
    const static int MAX_REQUEST_SIZE=1024*1024*10;

private:
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);

    void handle_write(const boost::system::error_code& e);

    /// Strand to ensure the connection's handlers are not called concurrently.
    boost::asio::io_service::strand strand_;

    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    chunk_reader reader_;
    std::string document_root_;
    char * buf; // FIXME: temporary reply buffer
};

typedef boost::shared_ptr<connection> connection_ptr;

#endif // ___CONNECTION_H___ 

