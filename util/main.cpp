#ifndef __FTD
#include "utility.h"
#endif

using namespace utl::typedefs;
using namespace std::chrono_literals;

#include <print>

int main()
{
    auto result = ftd::filesystem::read_file("C:\\Tests\\Test.txt");

    if (result.has_value()) {
        ftd::print("Contents:\n{}", result.value());
    }
    else {
        ftd::print("Failure: {}", result.error().msg);
    }
}