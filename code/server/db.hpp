#ifndef DIPLOMA_SERVER_DB_HPP
#define DIPLOMA_SERVER_DB_HPP

#include <helib/helib.h>
#include <boost/noncopyable.hpp>
#include <optional>
#include <vector>
#include <utility>

class EncryptedDatabase: public boost::noncopyable {
private:
    struct EncryptedEntry: public boost::noncopyable {
        helib::Ctxt encryptedValue_;
    };

    struct EncryptedKey: public boost::noncopyable {
        uint32_t plaintextPrimeModulus_;
        uint32_t slotsNumber_;

        helib::Ctxt key_;
    };
public:

    explicit EncryptedDatabase();

    static void Dump(std::ofstream& outStream, const EncryptedDatabase& db);
    static EncryptedDatabase Load(std::istream& inSream);

    void Create(const helib::Context& context, const helib::Ctxt& keyCipher);
    std::optional<EncryptedEntry> Lookup(const helib::Ctxt& key) const;
    void Add(const helib::Ctxt& key, const helib::Ctxt& value);

private:
    std::vector<std::pair<EncryptedKey, EncryptedEntry>> encryptedKeyValueDb_;
};

#endif
