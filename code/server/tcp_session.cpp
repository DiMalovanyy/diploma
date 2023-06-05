#include "tcp_session.hpp"

#include <boost/bind.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

TcpSession::TcpSession(boost::asio::io_context& ioc, EncryptedDatabase& dbRef): socket_(ioc), dbRef_(dbRef) {}

boost::asio::ip::tcp::socket& TcpSession::GetConnectionSocket() {
    return socket_;
}

void TcpSession::HandleRequest() {
    boost::asio::async_read_until(socket_, inputBuffer_, "\n",
        boost::bind(&TcpSession::handleRead, shared_from_this(), 
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)); 
}

void TcpSession::handleRead(const boost::system::error_code& err, size_t bytes) {
    if (err) {
        spdlog::error("Помилка під час отримання повідомлення від клієнта: {}:{}. Помилка: {}",
                socket_.remote_endpoint().address().to_string(),
                socket_.remote_endpoint().port(),
                err.message());
        return;
    }

    logger_ = initLogger(fmt::format("{}:{}", 
                         socket_.remote_endpoint().address().to_string(),
                         socket_.remote_endpoint().port()));

    std::string query( (std::istreambuf_iterator<char>(&inputBuffer_)), std::istreambuf_iterator<char>() );
    logger_->debug("Отриманий запит від клієнта. Прочитано {} байтів.", bytes);

    try {
        nlohmann::json inputJson = nlohmann::json::parse(query);
        handleQuery(inputJson["operation"], inputJson["body"]);
    } catch(const std::exception& err){
        sendError(err.what());
    }
}


void TcpSession::handleQuery(const std::string& query, const nlohmann::json& body) {
    spdlog::info("Обробка запиту від клієнта. Запит: {}", query);
   
    try {
        if (query == "get") {


        } else if (query == "create") {
            std::stringstream jsonContextStream;
            jsonContextStream << body["public_context"];
            auto encryptionContext = std::shared_ptr<helib::Context>(helib::Context::readPtrFromJSON(jsonContextStream));

            std::stringstream jsonPublicKeyStream;
            jsonPublicKeyStream << body["public_key"];
            helib::PubKey publicKey = helib::PubKey::readFromJSON(jsonPublicKeyStream, *encryptionContext.get());

            std::stringstream jsonAccountNameStream;
            jsonAccountNameStream << body["account_name"];
            helib::Ctxt accountCiphertext = helib::Ctxt::readFromJSON(jsonAccountNameStream, publicKey);

            dbRef_.Create(encryptionContext, publicKey, accountCiphertext);
        }
        sendSuccess({});
    } catch (std::exception& ex) {
        logger_->error("Помилка під час обробки запиту від клієнта. Помилка: {}", ex.what());
        sendError(ex.what());
    }
}

void TcpSession::sendError(const std::string& msg) {
    logger_->error("Помилка під час обробки запиту від клієнта. Помилка: {}", msg); 
    nlohmann::json outputJson;
    outputJson["status"] = "error";
    outputJson["errorMsg"] = msg;
    sendResponse(outputJson);
}

void TcpSession::sendSuccess(const nlohmann::json& body) {
    logger_->info("Запит від клієнта успішно обролений");
    nlohmann::json outputJson;
    outputJson["status"] = "success";
    outputJson["body"] = body;
    sendResponse(outputJson);
}

void TcpSession::sendResponse(const nlohmann::json& responseJson) {
    boost::asio::streambuf responseBuffer;
    std::ostream outBuffer(&responseBuffer);
    outBuffer << responseJson << "\n";
    boost::asio::async_write(socket_, responseBuffer, [this_ = shared_from_this()](const boost::system::error_code& err, size_t bytesTransferred ) {
        if (err) {
            this_->logger_->error("Виникла проблема під час відправки відповіді клієнту: Код: {}, повідомлення: {}", err.value(), err.message());
        } else {
            this_->logger_->debug("Успішно відправлено {} байтів клієнту", bytesTransferred);
        }
        if (this_->socket_.is_open()) {
            this_->socket_.close();
        }
    });
}

std::shared_ptr<spdlog::logger> TcpSession::initLogger(const std::string& clientAddress) {
    std::string logName = fmt::format("Client {}", clientAddress);
    return spdlog::stderr_color_mt(logName);
}

