#include "common.h"

#include <functional>
#include <tuple>
#include <type_traits>

#define STRING_JOIN(arg1, arg2) DO_STRING_JOIN2(arg1, arg2)
#define DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2

#define DEFER_NODISCARD [[nodiscard("always store the value of a defer context, otherwise the constructor is called instantly")]]
#define DEFER_DEPRECATED [[deprecated("use defer instead")]]

_UTIL_API

template<typename _Fty>
class DEFER_DEPRECATED defer_context {
private:
    std::vector<std::function<_Fty()>> _Funcs{};
public:
    using function_type = std::function<_Fty()>;

    constexpr defer_context() {}
    constexpr inline ~defer_context() noexcept(true) {
        for (const auto& _Def : _Funcs) {
            try {
                _Def();
            }
            catch (...) {
                if (std::is_constant_evaluated()) {
                    throw;
                }
            };
        }
    }

    constexpr void queue(const function_type& _Fu) noexcept {
        _Funcs.push_back(_Fu);
    }
};

template<typename F>
class DEFER_NODISCARD singular_defer_context {
private:
    F f;
public:
    constexpr singular_defer_context(F f) : f(f) {}
    constexpr ~singular_defer_context() { f(); }

    constexpr auto deferable() const noexcept { return true; }
    constexpr F& instance() noexcept { return f; }
};

template<size_t N, typename F, typename... Types>
class variadic_defer_context_caller;
//{
//   auto call(F&, std::tuple<Types>);
//}

template<typename F, typename ...Types>
class variadic_defer_context_caller<1, F, Types...> {
public:
    auto call(F& _Fn, std::tuple<Types...> _Ag) {
        _Fn(std::get<0>(_Ag));
    }
};

template<typename F, typename ...Types>
class variadic_defer_context_caller<2, F, Types...> {
public:
    auto call(F& _Fn, std::tuple<Types...> _Ag) {
        _Fn(std::get<0>(_Ag), std::get<1>(_Ag));
    }
};

template<typename F, typename ...Types>
class variadic_defer_context_caller<3, F, Types...> {
public:
    auto call(F& _Fn, std::tuple<Types...> _Ag) {
        _Fn(std::get<0>(_Ag),
            std::get<1>(_Ag),
            std::get<2>(_Ag));
    }
};

template<typename F, typename ...Types>
class variadic_defer_context_caller<4, F, Types...> {
public:
    auto call(F& _Fn, std::tuple<Types...> _Ag) {
        _Fn(std::get<0>(_Ag),
            std::get<1>(_Ag),
            std::get<2>(_Ag),
            std::get<3>(_Ag));
    }
};

template<typename F, typename ...Types>
class variadic_defer_context_caller<5, F, Types...> {
public:
    auto call(F& _Fn, std::tuple<Types...> _Ag) {
        _Fn(std::get<0>(_Ag),
            std::get<1>(_Ag),
            std::get<2>(_Ag),
            std::get<3>(_Ag),
            std::get<4>(_Ag));
    }
};

template<typename F, class ...Args>
class DEFER_NODISCARD singular_variadic_defer_context {
private:
    std::tuple<Args...> __Args{};
    F __Func;
public:
    using specialization = variadic_defer_context_caller<sizeof ...(Args), F, Args...>;
    using return_type = typename std::invoke_result<F, Args...>::type;

    constexpr singular_variadic_defer_context(const F& f, Args&&... _Args)
        : __Func(f) 
    {
        __Args = std::make_tuple<Args...>(std::move(_Args)...);
    }
    constexpr ~singular_variadic_defer_context() noexcept {
        auto caller = this->create_caller();
        caller.call(__Func, __Args);
    }

    FTD_CONSTEXPR size_t arg_count() const noexcept {
        return sizeof ...(Args);
    }
    FTD_CONSTEXPR const std::tuple<Args...>& arguments() const noexcept {
        return __Args;
    }
    FTD_CONSTEXPR F& function() noexcept {
        return __Func;
    }

    static 
        FTD_CONSTEXPR specialization create_caller() noexcept {
        return {};
    }
};

template<typename F>
FTD_NODISCARD
auto 
make_defer_context(const F& _Fty) -> singular_defer_context<F> {
    return singular_defer_context<F>(_Fty);
}

template<class F, class ...Types>
using svdc = singular_variadic_defer_context<F, Types...>;

template<typename F, class ...Args>
FTD_NODISCARD
auto
make_defer_context(const F& _Fty, Args&&... _Args)
-> singular_variadic_defer_context<F, Args...> {
    return svdc<F, Args...>(_Fty, std::move(_Args)...);
}

template<class F>
using defer_t = singular_defer_context<F>;

#define defer ftd::singular_defer_context STRING_JOIN(__0_defer, __LINE__) = [&] 

template<class F, class ...Args>
using variadic_defer_t = singular_variadic_defer_context<F, Args...>;

_UTIL_API_END