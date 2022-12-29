#pragma once

#include "utility.h"

#include <iostream>
#include <vector>

using namespace util::typedefs;

int main()
{
    util::dict<std::string, int> thing = {};

    thing.insert("funny word", 69);
    thing.insert("other word", 1);
    const auto& funny = thing["non existant"];

    printf("%i -- length: %zu", funny, thing.size());
}