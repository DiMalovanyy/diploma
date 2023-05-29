#include "tcp_session.hpp"

#include <boost/bind.hpp>

TcpSession::TcpSession(boost::asio::io_context& ioc, EncryptedDatabase& dbRef): socket_(ioc), dbRef_(dbRef) {}

boost::asio::ip::tcp::socket& TcpSession::GetConnectionSocket() {
    return socket_;
}

void TcpSession::Start() {
    boost::asio::async_read_until(socket_, inputBuffer_, "\n",
        boost::bind(&TcpSession::handleRead, shared_from_this(), boost::asio::placeholders::error)); 
}

void TcpSession::handleRead(const boost::system::error_code& err) {
    if (err) {
        spdlog::error("Could not read message from connection: {}:{}",
                socket_.remote_endpoint().address().to_string(),
                socket_.remote_endpoint().port());
        return;
    }
    std::string query( (std::istreambuf_iterator<char>(&inputBuffer_)), std::istreambuf_iterator<char>() );
    spdlog::debug("Received query from client: {}:{} - {}",
                socket_.remote_endpoint().address().to_string(),
                socket_.remote_endpoint().port(),
                query);
}
