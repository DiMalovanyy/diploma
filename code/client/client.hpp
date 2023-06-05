#ifndef DIPLOMA_CLIENT_HPP
#define DIPLOMA_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <helib/helib.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <memory>

class Client: public boost::noncopyable, public std::enable_shared_from_this<Client> {
    using ResultCb = std::function<void(bool)>;
    using ResultWithMessageCb = std::function<void(bool, const std::string& message)>;
    using JsonCb = std::function<void(const nlohmann::json&)>;
public:
    explicit Client(const std::string& serverIp, uint32_t port);

    void Create(const helib::PubKey& publicKey, 
                const helib::Context& encryptionContext,
                const helib::Ctxt& accountCiphertext, ResultCb cb);
    void Withdraw(const helib::Ctxt& accountCiphertext, ResultCb cb);
    void Add(const helib::Ctxt& accountCiphertext, ResultCb cb);
    void Get(const helib::PubKey& publicKey, const helib::Ctxt& accountCiphertext, ResultCb cb);
private:
    void connect(ResultCb onConnect);
    void request(const std::string& operation, const nlohmann::json& body, ResultWithMessageCb cb);
    void handleResponse(ResultWithMessageCb cb);

    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::streambuf responseBuffer_;

    std::shared_ptr<spdlog::logger> logger_;
};



#endif
