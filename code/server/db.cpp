#include "db.hpp"
#include <utility>
#include <spdlog/sinks/stdout_color_sinks.h>

constexpr long bitSize = 16;

EncryptedDatabase::EncryptedDatabase(): logger_(spdlog::stderr_color_mt("Database")) { 

}
void EncryptedDatabase::Create(std::shared_ptr<helib::Context> context, 
                               const helib::PubKey& publicKey,
                               const helib::Ctxt& keyCipher) {

    long zero = 0;
    const helib::EncryptedArray ea = context->getEA();
    helib::Ctxt scratch(publicKey);
    std::vector<helib::Ctxt> encryptedZero(bitSize, scratch);
    //Encrypt data in binary representation
    for(long i = 0; i < bitSize; i++) {
        std::vector<long> zeroVec(ea.size());
        for (auto& slot: zeroVec) {
            slot = (zero >> i) & 1; 
        }
        ea.encrypt(encryptedZero[i], publicKey, zeroVec);
    }

    EncryptedKey encryptedKey = {
        .publicKey_ = publicKey,
        .encryptionContext_ = context,
        .key_ = keyCipher
    };
    EncryptedEntry encryptedEntry = {
        .encryptedBinary_ = encryptedZero,
    };
    logger_->debug("Контент успішно доданий в базу даних");
}

std::optional<EncryptedDatabase::EncryptedEntry> EncryptedDatabase::Lookup(const helib::Ctxt& key) const {

    std::vector<helib::Ctxt> ciphertextMask;
    ciphertextMask.reserve(encryptedKeyValueDb_.size());
    for(const auto& [encryptedKey, encryptedEntry]: encryptedKeyValueDb_) {
        helib::Ctxt ciphertextMaskEntry = encryptedKey.key_;
        ciphertextMaskEntry -= key; 
        ciphertextMaskEntry.power(encryptedKey.encryptionContext_->getP() - 1);
        ciphertextMaskEntry.negate();
        ciphertextMaskEntry.addConstant(NTL::ZZX(1));

        const helib::EncryptedArray encryptedArray = encryptedKey.encryptionContext_->getEA();

        std::vector<helib::Ctxt> rotatedCiphertextMasks(encryptedArray.size(), ciphertextMaskEntry);
        for(int i = 1; i < rotatedCiphertextMasks.size(); i++) {
            encryptedArray.rotate(rotatedCiphertextMasks[i], i);
        }
        totalProduct(ciphertextMaskEntry, rotatedCiphertextMasks);


//        ciphertextMaskEntry.multiplyBy(encryptedEntry);
//      ciphertextMask.push_back(mask_entry);
    }

    helib::Ctxt value = ciphertextMask[0];
    for(int i = 1; i < ciphertextMask.size(); i++) {
        value += ciphertextMask[i];
    }

    return std::nullopt;

 //   return std::make_optional<EncryptedEntry>{value};
}

void EncryptedDatabase::Add(const helib::Ctxt& key, const helib::Ctxt& value) {


}
