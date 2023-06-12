#pragma once

#include "common.h"

#include <future>
#include <functional>

#ifndef NODISCARD_LAZY
#define NODISCARD_LAZY [[nodiscard("you should not discard a lazy instance as its a waste of cpu-time")]]
#endif

_UTIL_API

template<class T>
class lazy {
public:
    using func = std::function<T()>;
private:
    std::future<T> _Promise{};
    func _Fc{};

    inline T _Fcall() {
        return _Fc();
    }
public:
    inline lazy(const func& _Initializer) {
        _Fc = _Initializer;
        _Promise = std::async(std::launch::async, &lazy::_Fcall, this);
    }

    lazy(const lazy&) = delete;
    lazy(lazy&&) = default;

    inline BOOL done() const noexcept {
        return ready(*this);
    }
    inline T get() {
        return _Promise.get();
    }
    inline T get_now() noexcept {
        _Promise.wait();
        return _Promise.get();
    }

    static inline BOOL ready(lazy& _Pr) noexcept {
        return _Pr._Promise.wait_for(std::chrono::seconds(0))
            == std::future_status::ready;
    }
};

template<class F>
NODISCARD_LAZY auto 
lazy_load(const typename lazy<F>::func& f) noexcept -> lazy<F> {
    return lazy<F>{f};
}

template<class F>
auto unwrap(lazy<F>& p) noexcept -> F {
    return p.get_now();
}

_UTIL_API_END