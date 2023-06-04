#ifndef DIPLOMA_TCP_SESSION_HPP
#define DIPLOMA_TCP_SESSION_HPP

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <memory>

#include "db.hpp"

class TcpSession: public std::enable_shared_from_this<TcpSession>, public boost::noncopyable {
public:
    explicit TcpSession(boost::asio::io_context& ioc, EncryptedDatabase& dbRef);

    void HandleRequest();

    boost::asio::ip::tcp::socket& GetConnectionSocket();
private:
    
    void handleRead(const boost::system::error_code& err, size_t numBytes);
    void handleQuery(const std::string& query, const nlohmann::json& body);


    void sendError(const std::string& msg);
    void sendSuccess(const nlohmann::json& body);
    void sendResponse(const nlohmann::json& responseJson);

    static std::shared_ptr<spdlog::logger> initLogger(const std::string& clientAddress);

    boost::asio::streambuf inputBuffer_;
    boost::asio::ip::tcp::socket socket_;

    std::shared_ptr<spdlog::logger> logger_;


    EncryptedDatabase& dbRef_;
};

#endif
