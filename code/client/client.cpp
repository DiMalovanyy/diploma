#include "client.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

Client::Client(const std::string& serverIp, uint32_t port): 
    socket_(ioc_), endpoint_(boost::asio::ip::address::from_string(serverIp), port), logger_(spdlog::stdout_color_mt("client")) {}


void Client::Create(const helib::PubKey& publicKey, 
                    const helib::Context& encryptionContext,
                    const helib::Ctxt& accountCiphertext, ResultCb cb) {
    logger_->info("Обробка запиту для створення акаунту...");
    connect([this, this_=shared_from_this(), cb, &accountCiphertext, &publicKey, &encryptionContext](bool isConnected) {
        if (!isConnected) {
            this_->logger_->error("Неможливо встановити зʼєднання з сервером");
            cb(false);
            return;
        }
        this_->logger_->debug("Зʼєднання з сервером успішно встановлено. Відпрака запита на створення аккаунту...");
        nlohmann::json createBody;

        std::ostringstream contextStream;
        encryptionContext.writeToJSON(contextStream);
        createBody["public_context"] = nlohmann::json::parse(contextStream.str());

        std::ostringstream accountNameStream;
        accountCiphertext.writeToJSON(accountNameStream);
        createBody["account_name"] = nlohmann::json::parse(accountNameStream.str());

        std::ostringstream publicKeyStream;
        publicKey.writeToJSON(publicKeyStream);
        createBody["public_key"] = nlohmann::json::parse(publicKeyStream.str());

        this_->request("create", createBody, [this, cb](bool isError, const std::string& msg) {
            logger_->info("Отримана відповідь від сервера: {}", msg); 
            cb(isError);
        });
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
    requestJson["operation"] = operation;
    requestJson["body"] = body;

    boost::asio::streambuf requestBuffer;
    std::ostream outBuffer(&requestBuffer);
    outBuffer << requestJson << "\n";

    boost::asio::async_write(socket_, requestBuffer, [this_ = shared_from_this(), cb](const boost::system::error_code& err, size_t bytesTransferred ) {
        if (err) {
            this_->logger_->error("Виникла проблема під час відправки повідомлення на сервер: Код: {}, повідомлення: {}", err.value(), err.message());
            if (this_->socket_.is_open()) {
                this_->socket_.close();    
            }
            cb(false, "");
            return;
        }
        this_->logger_->debug("Успішно відправлено {} байтів на сервер", bytesTransferred);
        this_->handleResponse(cb);
    });
}

void Client::handleResponse(ResultWithMessageCb cb) {
    boost::asio::async_read_until(socket_, responseBuffer_, "\n",
            [this_= shared_from_this(), cb](
                const boost::system::error_code& err, size_t bytesTransferred) {
        if (err) {
            this_->logger_->error("Помилка під час отримання результату від сервера: Помикла: {}, код: {}", err.message(), err.value());
            if (this_->socket_.is_open()) {
                this_->socket_.close();
            }
            cb(false, err.message());
            return;
        }
        std::string message( (std::istreambuf_iterator<char>(&this_->responseBuffer_)), std::istreambuf_iterator<char>() );
        this_->logger_->debug("Успішно отримана відповідь від сервер: {}", message);
    });
}
