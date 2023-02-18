#pragma once

#include "common.h"

_UTIL_EXPERIMENTAL

template<class K>
struct singleton_key {
    // The memory held by the singleton.
    // Access to this provides a way to delete.
    K* lock;
    size_t bits;
};

template<typename T>
class singleton {
private:
    static inline T* s_instance;
public:
    singleton() = delete;
    singleton(const singleton&) = delete;
    singleton(singleton&&) = delete;

    template<typename ...Args>
    static singleton_key<T> initialize(const Args&... args) {
        s_instance = new T(args...);
        return singleton_key<T> {
            s_instance,
            sizeof(T) * hashcode_of<T>()
        };
    }
    static bool initialized() noexcept {
        return s_instance != nullptr;
    }

    static bool destroy(singleton_key<T>& _Lock) noexcept {
        if (_Lock.lock != s_instance)
            return false;
        if (_Lock.bits != sizeof(T) * hashcode_of<T>())
            return false;
        delete s_instance;
        return true;
    }

    static T& the() {
#if defined _DEBUG
        if (!initialized()) {
            utl::panic("attempt to access uninitialized singleton");
        }
#endif
        return *s_instance;
    }
};

_UTIL_API_END