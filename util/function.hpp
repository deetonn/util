#pragma once

#include "common.h"

_UTIL_API

template<class R, class ...Args>
class function;

template<class R, class ...Args>
struct _Func_storage {
    using type = R(*)(Args...);
    using lambda = decltype([](Args...)->R { return R{}; });
    type __function;
};

template<class R, class ...Args>
class function<R(Args...)> {
private:
    _Func_storage<R, Args...> __storage{};
public:
    template<class F>
    // constructor that accepts lambdas that return R and have Args... as arguments
    function(F&& f) noexcept {
        __storage.__function = [&f](Args... args) -> R {
            return f(args...);
        };
    }

    ~function() noexcept {

    }

    // call operator
    R operator()(Args... args) const noexcept {
        return __storage.__function(args...);
    }
};

_UTIL_API_END