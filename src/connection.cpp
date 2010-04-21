#include <vector>
#include <string>
#include <sstream>
#include <boost/bind.hpp>

#include "connection.hpp"
#include "req_parser.hpp"

connection::connection(boost::asio::io_service& io_service)
  : strand_(io_service),
    socket_(io_service),
    reader(1048576)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
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
        ReqParser parser; 
        for(size_t i = 0; i < bytes_transferred; ++i) {
            std::cout << buffer_[i];
            parser.consume(buffer_[i]);
        }
        std::cout << std::endl;
        if(parser.file() == "testdata" || parser.file() == "big_buck_bunny") {
            reader.read(parser.file(), parser.size(), parser.index(), parser.count());
           
            std::stringstream ss;
            ss << "HTTP/1.0 200 OK\r\nContent-Length: "
                  << reader.chunk_size() << "\r\n\r\n";

            std::string reply = ss.str();

            size_t buf_size = reply.length() + reader.chunk_size();
            buf = new char[buf_size];

            size_t counter = 0;
            for(size_t i = 0; i < reply.length(); ++i)
            {
                buf[counter++] = reply[i];
            }
            
            std::cout << "LOG: sending " << reader.chunk_size() 
                      << " bytes" << std::endl;

            for(size_t i = 0; i < reader.chunk_size(); ++i) {
                buf[counter++] = (reader.chunk())[i];
            }
            boost::asio::async_write(socket_, boost::asio::buffer(buf, buf_size),
                strand_.wrap(
                  boost::bind(&connection::handle_write, shared_from_this(),
                  boost::asio::placeholders::error)));
        }
    }
}

void connection::handle_write(const boost::system::error_code& e)
{
    delete [] buf;
    if (!e)
    {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    } 
    else 
    {
        std::cerr << "FAILURE: " << e.message() << std::endl;
    }

    // No new asynchronous operations are started. This means that all shared_ptr
    // references to the connection object will disappear and the object will be
    // destroyed automatically after this handler returns. The connection class's
    // destructor closes the socket.
}
