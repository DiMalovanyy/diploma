#include "client.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

Client::Client(const std::string& serverIp, uint32_t port): 
    socket_(ioc_), endpoint_(boost::asio::ip::address::from_string(serverIp), port), logger_(spdlog::stdout_color_mt("client")) {}


void Client::Create(const helib::Ctxt& accountCiphertext, ResultCb cb) {
    logger_->info("Обробка запиту для створення акаунту...");
    connect([this_=shared_from_this(), cb, accountCiphertext](bool isConnected) {
        if (!isConnected) {
            this_->logger_->error("Неможливо встановити зʼєднання з сервером");
            cb(false);
            return;
        }
        this_->logger_->debug("Зʼєднання з сервером успішно встановлено");
    });

    ioc_.run();
}


void Client::connect(ResultCb onConnect) {
    socket_.async_connect(endpoint_, [this_=shared_from_this(), onConnect](const boost::system::error_code& err) {
        if (err) {
            onConnect(false);
        } else {
            onConnect(true);
        }
    });
}


void Client::request(const std::string& operation, const nlohmann::json& body, ResultWithMessageCb cb) {

    nlohmann::json requestJson;
    requestJson["oeration"] = operation;
    requestJson["body"] = body;

    boost::asio::streambuf requestBuffer;
    std::ostream outBuffer(&requestBuffer);
    outBuffer << requestJson << "\n";

    auto handleRead = [this_ = shared_from_this(), cb](const boost::system::error_code& err) {

    };
     
    boost::asio::async_write(socket_, requestBuffer, [this_ = shared_from_this(), handleRead, cb](const boost::system::error_code& err, size_t bytesTransferred ) {
        if (err) {
            this_->logger_->error("Виникла проблема під час відправки повідомлення на сервер: Код: {}, повідомлення: {}", err.value(), err.message());
            this_->socket_.close();    
            cb(false, "");
            return;
        }
        this_->logger_->debug("Успішно відправлено {} байтів на сервер", bytesTransferred);
    });
}
