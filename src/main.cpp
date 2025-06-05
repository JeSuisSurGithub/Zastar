#include <ctx.hpp>
#include <chrono>

#include <iostream>

int main()
{
    std::cout << "build " << __TIME__ << ' ' << __DATE__ << std::endl;
    try {
        zsl::ctx context{true, (zsl::u32)std::chrono::system_clock::now().time_since_epoch().count()};
        zsl::run(context);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
    }
}