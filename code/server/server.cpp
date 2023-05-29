#include "server.hpp"
#include <boost/bind.hpp>

Server::Server(long port): 
    acceptor_(ioc_, boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::tcp::v4(),
                        port)) {
    spdlog::debug("Server initialized on port: {}", port);
}

void Server::Start() {
    spdlog::debug("Server started");
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
        spdlog::info("New connection established: {}:{}", 
                connection->GetConnectionSocket().remote_endpoint().address().to_string(),
                connection->GetConnectionSocket().remote_endpoint().port());
        return;
    } else {
        spdlog::error("Could not establish connection");
    }
    startAccept();
}
