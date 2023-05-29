#ifndef DIPLOMA_TCP_SESSION_HPP
#define DIPLOMA_TCP_SESSION_HPP

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include <memory>

#include "db.hpp"

class TcpSession: public std::enable_shared_from_this<TcpSession>, public boost::noncopyable {
public:
    explicit TcpSession(boost::asio::io_context& ioc, EncryptedDatabase& dbRef);

    void Start();

    boost::asio::ip::tcp::socket& GetConnectionSocket();
private:
    
    void handleRead(const boost::system::error_code& err);

    boost::asio::streambuf inputBuffer_;
    boost::asio::ip::tcp::socket socket_;

    EncryptedDatabase& dbRef_;
};

#endif
