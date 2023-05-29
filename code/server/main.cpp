#include <helib/helib.h>
#include <spdlog/spdlog.h>
#include "server.hpp"

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug); 

    Server server(7623);
    server.Start();

    return 0;
}
