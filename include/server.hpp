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
     * Creates and initializs a new on_demand_server
     *
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
