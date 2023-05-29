#include "db.hpp"

EncryptedDatabase::EncryptedDatabase() {

}

void EncryptedDatabase::Create(const helib::Ctxt& key) {

}

std::optional<EncryptedDatabase::EncryptedEntry> EncryptedDatabase::Lookup(const helib::Ctxt& key) const {
/*
    std::vector<helib::Ctxt> ciphertextMask;
    ciphertextMask.reserve(encryptedKeyValueDb_.size());
    for(const auto& [encryptedKey, encryptedEntry]: encryptedKeyValueDb_) {
        helib::Ctxt ciphertextMaskEntry = encryptedKey.key_;
        ciphertextMaskEntry -= key; 
        ciphertextMaskEntry.power(encryptedKey.plaintextPrimeModulus_ - 1);
        ciphertextMaskEntry.negate();
        ciphertextMaskEntry.addConstant(NTL::ZZX(1));

        std::vector<helib::Ctxt> rotatedCiphertextMasks(encryptedKey.encryptedArray.size(), ciphertextMaskEntry);
        for(int i = 1; i < rotatedCiphertextMasks.size(); i++) {
            encryptedArray.rotate(rotatedCiphertextMasks[i], i);
        }
        totalProduct(mask_entry, rotatedCiphertextMasks);
        mask_entry.multiplyBy(encryptedEntry);
        ciphertextMask.push_back(mask_entry);
    }

    helib::Ctxt value = ciphertextMask[0];
    for(int i = 1; i < ciphertextMask.size(); i++) {
        value += ciphertextMask[i];
    }

    return std::make_optional<EncryptedEntry>{value};
*/
    return std::nullopt;
}

void EncryptedDatabase::Add(const helib::Ctxt& key, const helib::Ctxt& value) {


}
