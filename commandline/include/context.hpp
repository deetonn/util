#pragma once

/*
* Anything context related.
* 
* I.E: executions contexts, IO contexts, etc...
*/

#include "common.h"

_UTIL_API

template<typename F, typename... Args>
class exception_environment {
public:
    using return_type =
        typename variadic_defer_t<F, Args...>::return_type;

    static_assert(!std::is_same<return_type, void>::value, 
        "cannot save void within an exception environment");
private:
    F _Func;
    return_type* _Result{ nullptr };
    std::exception* _Err{ nullptr };
public:
    exception_environment(F&& _Func)
        : _Func(_Func)
    {}
    virtual ~exception_environment() noexcept {
        if (_Result)
            delete _Result;
        if (_Err)
            delete _Err;
    }

    void execute(Args... _Args) noexcept {
        try {
            auto _Res = _Func(_Args...);
            _Result = new(std::nothrow) return_type{ _Res };
        }
        catch (const std::exception& _Ex) {
            _Err = new (std::nothrow) std::exception(_Ex);
        }
    }
    void reload(F&& _Func) { 
        this->_Func = _Func;

        if (_Result) {
            delete _Result;
            _Result = nullptr;
        }
        if (_Err) {
            delete _Err;
            _Err = nullptr;
        }
    }

    BOOL is_exception() const noexcept { return _Err != nullptr; }

    optional<std::exception> exception() const noexcept {
        if (!is_exception())
            return {};
        return *_Err;
    }
    optional<return_type> result() const noexcept {
        if (is_exception())
            return {};
        return *_Result;
    }
};

template<class F, class ...Args>
FTD_NODISCARD
auto
make_environment(F&& Function)
-> exception_environment<F, Args...> {
    return exception_environment(std::move(Function));
}

_UTIL_API_END