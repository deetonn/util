#ifndef __FTD
#include "utility.h"
#endif

using namespace utl::typedefs;
using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    auto args_defer =
        future::make_defer_context(
            [&](size_t left, size_t right, bool print) {
                ftd::print("left: {}, right: {}", left, right);
            },
            10, 20, true
       );
}