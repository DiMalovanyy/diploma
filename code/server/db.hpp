#ifndef DIPLOMA_SERVER_DB_HPP
#define DIPLOMA_SERVER_DB_HPP

#include <helib/helib.h>
#include <boost/noncopyable.hpp>
#include <spdlog/logger.h>

#include <optional>
#include <vector>
#include <utility>

class EncryptedDatabase: public boost::noncopyable {
private:
    struct EncryptedEntry {
        std::vector<helib::Ctxt> encryptedBinary_; 
    };

    struct EncryptedKey {
        helib::PubKey publicKey_;
        std::shared_ptr<helib::Context> encryptionContext_;
        helib::Ctxt key_;
    };
public:

    explicit EncryptedDatabase();

    static void Dump(std::ofstream& outStream, const EncryptedDatabase& db);
    static EncryptedDatabase Load(std::istream& inSream);

    void Create(std::shared_ptr<helib::Context> context, 
                const helib::PubKey& publicKey,
                const helib::Ctxt& keyCipher);
    std::optional<EncryptedEntry> Lookup(const helib::Ctxt& key) const;
    void Add(const helib::Ctxt& key, const helib::Ctxt& value);

private:
    std::vector<std::pair<EncryptedKey, EncryptedEntry>> encryptedKeyValueDb_;
    std::shared_ptr<spdlog::logger> logger_;
};

#endif
