#include "common.h"

#include <functional>

#define STRING_JOIN(arg1, arg2) DO_STRING_JOIN2(arg1, arg2)
#define DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2

_UTIL_EXPERIMENTAL

template<typename _Fty>
class defer_context {
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
                if constexpr (std::is_constant_evaluated()) {
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
class singular_defer_context {
private:
    F f;
public:
    constexpr singular_defer_context(F f) : f(f) {}
    ~singular_defer_context() { f(); }

    constexpr auto deferable() const noexcept { return true; }
    constexpr F& instance() noexcept { return f; }
};

#define defer(c) auto STRING_JOIN(_$$DfrCon_, __LINE__) = _UTL future::singular_defer_context {[&]{c;}}
#define defer_with_access(n, c) auto n = _UTL future::singular_defer_context{[&]{c;}}

_UTIL_API_END