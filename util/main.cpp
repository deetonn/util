#ifndef __FTD
#include "utility.h"
#endif

using namespace ftd::typedefs;
using namespace std::chrono_literals;

#include "thread.hpp"
#include <array>

template<class Array,
    class Size = typename Array::size_type,
    class Elem = typename Array::value_type>
auto test_compatability(Array& arg)
{
    for (Size s = 0; s < arg.size(); ++s) {
        ftd::print("Got value '{}' at index {}", arg[s], s);
    }

    ftd::print("Array Max Size: {}", arg.max_size());
    arg.fill(Elem{});

    ftd::print("Filled with default value of '{}'",
        ftd::nameof<Elem>());
}

struct A {
    size_t x, y, z;
    bool operator==(A const& other) const noexcept {
        return x == other.x
            && y == other.y
            && z == other.z;
    }
};

int main(u32 argc, ftd::NativeArray<char*> argv)
{
    ftd::array<A, 6> f1 = {
        A {1, 2, 3},
        A {2, 4, 6},
        A {3, 6, 9},
        A {4, 8, 12},
        A {5, 10, 15},
        A {6, 12, 18}
    };

    auto indx = ftd::first_index_of(f1, A{ 3, 6, 9 });
    
    if (indx.has_value()) {
        ftd::print("Index is {}", indx.value());
    }
    else {
        std::string sRepr = {};
        switch (indx.error()) {
        case ftd::FirstIndexOfError::ElementNotFound:
            sRepr = "Failed to find that element";
            break;
        case ftd::FirstIndexOfError::NotComparable:
            sRepr = "That type is not comparable";
            break;
        default:
            ftd::panic("implement me");
        }
        ftd::print("Error: {}", sRepr.c_str());
    }
}