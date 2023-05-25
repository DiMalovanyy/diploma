#include <helib/helib.h>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug); 
    spdlog::debug("client started");
    return 0;
}
