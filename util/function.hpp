#pragma once

#include "common.h"

_UTIL_API

template<class R, class ...Args>
class function;

template<class R, class ...Args>
class function<R(Args...)> {
public:
    using function_type = decltype([&](Args...) {} -> R);
private:
    function_type* _Ff;
public:
    function(function_type _Ff) {
        this->_Ff = new(std::nothrow) function_type(_Ff);
    };
    ~function() {
        delete _Ff;
    }
};

_UTIL_API_END