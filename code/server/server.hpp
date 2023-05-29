#ifndef DIPLOMA_SERVER_HPP
#define DIPLOMA_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <spdlog/spdlog.h>

#include "tcp_session.hpp"
#include "db.hpp"

class Server: boost::noncopyable {
public:
    Server(long port);

    void Start();
private:
    void startAccept();
    void handleAccept(std::shared_ptr<TcpSession> connection,
                      const boost::system::error_code& err);

    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;

    EncryptedDatabase db_;
};


#endif
