#ifndef __FTD
#include "utility.h"
#endif

#include <array>

using namespace utl::typedefs;
using namespace std::chrono_literals;

int main()
{
    auto* name = new std::string("Deeton");

    defer {
        delete name;
    };
}