#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <helib/helib.h>

#include "client.hpp"

void printHelp() {
    std::cout << "Використання: client [COMMAND] --context-path [FILE_PATH] [ARGUMENTS...]" << std::endl;
    std::cout << "Команди:\n";
    std::cout << "  get           Отримати інформацію про аккаунт\n";
    std::cout << "      Параметри: <Назва аккаунту>\n\n";
    std::cout << "  create        Створює новий аккаунт\n";
    std::cout << "      Параметри: <Назва аккаунту>\n\n";
    std::cout << "  add           Додати баланс на аккаунт\n";
    std::cout << "      Параметри: <Назва аккаунту> <Кількість балансу>\n\n";
    std::cout << "  withdraw      Зняти баланс з аккаунту\n";
    std::cout << "      Параметри: <Назва аккаунту> <Кількість балансу>\n\n";
}

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    if (argc < 4) {
        printHelp();
        return 0;
    }

    std::string command = argv[1];
    std::string contextPath;

    if (std::string(argv[2]) == "--context-path") {
        contextPath = argv[3];
    } else {
        spdlog::error("Missing or invalid --context-path argument");
        return 1;
    }

    std::vector<std::string> additionalArgs;
    for (int i = 4; i < argc; ++i) {
        additionalArgs.push_back(argv[i]);
    }
    if (additionalArgs.empty()) {
        spdlog::error("Missing additional arguments for command");
        return 1;
    }

    spdlog::debug("Зчитування файлу конекста: {}", contextPath);
    std::ifstream fs(contextPath);
    if (!fs.is_open()) {
        spdlog::error("Неможливо зчитати файл контексту {}", contextPath);
        return 1;
    }

    nlohmann::json contextJson;
    fs >> contextJson;
    fs.close();

    std::stringstream contextJsonStream;
    contextJsonStream << contextJson["public_context"].dump();
    helib::Context encryptionContext = helib::Context::readFromJSON(contextJsonStream);

    std::istringstream secretKeyJsonStream(contextJson["private_key"].dump());
    helib::SecKey secretKey = helib::SecKey::readFromJSON(secretKeyJsonStream, encryptionContext);
    
    const helib::PubKey& publicKey = secretKey;

    std::string accountName = additionalArgs[0];
    
    helib::Ptxt<helib::BGV> accountPlaintext(encryptionContext);
    for (int i = 0; i < accountName.size(); ++i) {
        accountPlaintext[i] = accountName[i];
    }
    
    spdlog::debug("Шифрування назви аккаунту...");
    helib::Ctxt accoutCiphertext(publicKey);
    publicKey.Encrypt(accoutCiphertext, accountPlaintext);
    
    auto client = std::make_shared<Client>("127.0.0.1", 7623);

    if (command == "get") {
        spdlog::debug("Викликана команда get(\"{}\")", accountName);

    } else if (command == "create") {
        spdlog::debug("Викликана команда create(\"{}\")", accountName);
        client->Create(accoutCiphertext, [](bool result) {
            std::cout << "Received result: " << result << " from the server" << std::endl;
        });
    } else if (command == "add") {
        if (additionalArgs.size() != 2) {
            spdlog::error("Add command should contains additional argument contains balance amount to add");
            return 1;
        }
    } else if (command == "withdraw") {
        if (additionalArgs.size() != 2) {
            spdlog::error("Add command should contains additional argument contains balance amount to withdraw");
            return 1;
        }
    } else {
        spdlog::error("Unknown command: {}", command);
        return 1;
    }

    return 0;
}
