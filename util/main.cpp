#include "utility.h"

#include <iostream>
#include <vector>

using namespace util::typedefs;

int main()
{
    auto info = util::type_info<const char*>::raw_name();
    std::cout << info << "\n";
}