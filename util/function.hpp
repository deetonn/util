#pragma once

#include "common.h"

#include <utility>

_UTIL_API

template<class R, class ...Args>
class function;

template<class R, class ...Args>
class function<R(Args...)> {
public:
    using function_type = decltype([&](Args...) -> R {});
private:
    function_type* _Ff;
public:
    function(function_type const& _Ff) {
        this->_Ff = new(std::nothrow) function_type(_Ff);
    };
    ~function() {
        delete _Ff;
    }

    R operator()(Args... args) {
        if constexpr (std::is_same_v<R, void>) {
            (*_Ff)(args...);
        }
        else {
            return (*_Ff)(args...);
        }
    }
};

_UTIL_API_END