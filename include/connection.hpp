#ifndef ___CONNECTION_H___ 
#define ___CONNECTION_H___ 

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "chunk_reader.hpp"

/**
 * Handles a single request.
 *
 * @param io_service boost::asio io_service
 * @param document_root Root path for documents.
 */
class connection
    : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
    explicit connection(boost::asio::io_service& io_service, const std::string& document_root);

    boost::asio::ip::tcp::socket& socket();

    /**
     * Starts the connection
     */
    void start();

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

    ChunkReader reader;
    std::string document_root_;
    char * buf; // FIXME: temporary reply buffer
};

typedef boost::shared_ptr<connection> connection_ptr;

#endif // ___CONNECTION_H___ 

