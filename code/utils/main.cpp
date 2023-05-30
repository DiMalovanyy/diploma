#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <helib/helib.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
  if (argc < 10) {
    std::cerr << "Insufficient arguments provided." << std::endl;
    std::cerr << "Usage: ./program p m r bits c mvec_size gen_size ords_size output_file [mvec_elements] [gens_elements] [ords_elements]" << std::endl;
    return 1;
  }

  long p = std::atol(argv[1]);
  long m = std::atol(argv[2]);
  long r = std::atol(argv[3]);
  long bits = std::atol(argv[4]);
  long c = std::atol(argv[5]);
  long mvecSize = std::atol(argv[6]);
  long genSize = std::atol(argv[7]);
  long ordsSize = std::atol(argv[8]);

  std::string outputFileName = argv[9];
  std::ofstream outputFile(outputFileName);

  if (!outputFile) {
    std::cerr << "Failed to open the output file." << std::endl;
    return 1;
  }

  std::vector<long> mvec(mvecSize);
  std::vector<long> gens(genSize);
  std::vector<long> ords(ordsSize);

  // Fill in the mvec vector
  for (int i = 0; i < mvecSize; ++i) {
    // Read the next argument and convert it to a long
    mvec[i] = std::atol(argv[i + 10]);
  }

  // Fill in the gens vector
  for (int i = 0; i < genSize; ++i) {
    // Read the next argument and convert it to a long
    gens[i] = std::atol(argv[i + 10 + mvecSize]);
  }

  // Fill in the ords vector
  for (int i = 0; i < ordsSize; ++i) {
    // Read the next argument and convert it to a long
    ords[i] = std::atol(argv[i + 10 + mvecSize + genSize]);
  }
  spdlog::debug("\n" 
        "Plaintext prime modulus (p):                             {}\n"
        "Cyclotomic polynomial (m):                               {}\n"
        "Hensel lifting (r):                                      {}\n"
        "Number of bits of the modulus chain (bits):              {}\n"
        "Number of columns of Key-Switching matrix (c):           {}\n"
        "Factorisation of m required for bootstrapping (mvec):    {}\n"
        "Generating set of Zm* group (gens):                      {}\n"
        "Orders of the previous generators (ords):                {}\n",
        p, m, r, bits, c, fmt::join(mvec, ", "), fmt::join(gens, ", "), fmt::join(ords, ", "));
  spdlog::info("Генерація контексту з вказаними параметрами...");


  helib::Context context = helib::ContextBuilder<helib::BGV>()
                               .m(m)
                               .p(p)
                               .r(r)
                               .gens(gens)
                               .ords(ords)
                               .bits(bits)
                               .c(c)
                               .bootstrappable(true)
                               .mvec(mvec)
                               .build();

  spdlog::info("Рівень захищенності контексту шифрування: {}", context.securityLevel());
  spdlog::info("Створення приватного ключа...");

  helib::SecKey secretKey(context);

  secretKey.GenSecKey();

  std::ostringstream secureKeyStream;
  secretKey.genRecryptData();
  secretKey.writeToJSON(secureKeyStream);


  std::ostringstream contextStream;
  context.writeToJSON(contextStream);

  nlohmann::json resultJson;
  resultJson["public_context"] = nlohmann::json::parse(contextStream.str());
  resultJson["private_key"] = nlohmann::json::parse(secureKeyStream.str());

  outputFile << resultJson;
  if (outputFile.is_open()) {
    outputFile.close();
  }
  spdlog::info("Контекст був записаний до файлу {}.", outputFileName);

  return 0;
}
