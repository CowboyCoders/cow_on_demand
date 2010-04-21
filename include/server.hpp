#ifndef ___SERVER_H___
#define ___SERVER_H___

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"

/**
 * Main server and dispatcher class.
 */
class server
  : private boost::noncopyable
{
public:
    /**
     * @param address Address to listen to
     * @param port Port to listen to
     * @param document_root Document root, path containing accessible files.
     * @param thread_pool_size Maximum number of server threads.
     */
    explicit server(const std::string& address, const std::string& port,
                    const std::string& document_root, std::size_t thread_pool_size);

  /**
   * Starts the server.
   */
  void run();

  /**
   * Stops the server.
   */
  void stop();

private:
  void handle_accept(const boost::system::error_code& e);
  std::string document_root_;
  std::size_t thread_pool_size_;

  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  connection_ptr new_connection_; // next connection
};

#endif // ___SERVER_H___ 
