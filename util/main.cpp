#ifndef __FTD
#include "utility.h"
#endif

using namespace utl::typedefs;
using namespace std::chrono_literals;

void bellend(size_t left, size_t right, bool print) {
    ftd::print("left: {}, right: {}", left, right);
}

int main(int argc, char** argv)
{
    auto context = ftd::make_defer_context(
            [&](size_t left, size_t right, bool print) {
                ftd::print("left: {}, right: {}", left, right);
            },
            10, 20, true
       );

    auto caller = context.create_caller();
    auto fn = context.function();
    auto& args = context.arguments();

    caller.call(fn, args);
}