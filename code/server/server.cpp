#include "server.hpp"
#include <boost/bind.hpp>

Server::Server(long port): 
    acceptor_(ioc_, boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::tcp::v4(),
                        port)) {
    spdlog::debug("Сервер сконфігурований на порті: {}", port);
}

void Server::Start() {
    spdlog::debug("Сервер запущений");
    startAccept();
    ioc_.run();
}

void Server::startAccept() {
    auto tcpSessionPtr = std::make_shared<TcpSession>(ioc_, db_);
    acceptor_.async_accept(tcpSessionPtr->GetConnectionSocket(),
        boost::bind(&Server::handleAccept, this, tcpSessionPtr, boost::asio::placeholders::error)
    );
}

void Server::handleAccept(std::shared_ptr<TcpSession> connection,
                      const boost::system::error_code& err) {
    if (!err) {
        spdlog::info("Нове зʼєднання: {}:{}", 
                connection->GetConnectionSocket().remote_endpoint().address().to_string(),
                connection->GetConnectionSocket().remote_endpoint().port());
        connection->HandleRequest();
    } else {
        spdlog::error("Неможливо створити зʼєднання з клієнтом. Помилка: {}", err.message());
    }
    startAccept();
}
