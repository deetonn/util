#pragma once

#ifndef __COMMON_H
#define __COMMON_H

#ifndef _UTIL_API
#define _UTIL_API namespace utl {
#endif

#ifndef _UTIL_API_END
#define _UTIL_API_END }
#endif

#ifndef _UTIL_DETAIL_BEGIN
#define _UTIL_DETAIL_BEGIN namespace utl::detail {
#endif

#ifndef _UTIL_DETAIL_END
#define _UTIL_DETAIL_END }
#endif

#ifndef _DETAIL
#define _DETAIL ::utl::detail:: 
#endif

#ifndef _UTIL_EXPERIMENTAL
#define _UTIL_EXPERIMENTAL namespace utl::future {
#endif

#ifndef _W
#define _W(X) L##X
#endif

#ifndef _IMPLICIT
#define _IMPLICIT
#endif

#ifndef FTD_CONSTEXPR
#define FTD_CONSTEXPR inline constexpr
#endif

#ifndef FTD_NODISCARD
#define FTD_NODISCARD [[nodiscard]]
#endif

#ifndef _MyPtr
#define _MyPtr() *this
#endif

#ifndef _MyMutPtr
#define _MyMutPtr() const_cast<std::remove_const_t<decltype(*this)*>(this)
#endif

#include <string>
#include <format>

#include <iostream>

#include "io_util.h"
#include "types.h"

#include <stacktrace>

#ifndef FTD_NORETURN
#define _NORETURN [[noreturn]]
#endif

#ifndef FTD_LIKELY
#define FTD_LIKELY [[likely]]
#endif

#ifndef FTD_UNLIKELY
#define FTD_UNLIKELY [[unlikely]]
#endif

#ifndef FTD_CONSTEVAL
#define FTD_CONSTEVAL __builtin_is_constant_evaluated()
#endif

_UTIL_API

/*
* All panic functions are templated to avoid them being compiled
* if they are not used.
*/

template<typename _Void = std::void_t<void>>
FTD_NORETURN constexpr inline auto panic(const char* message) -> _Void {
    if (FTD_CONSTEVAL) {
        // constant eval context requires exceptions
        throw std::exception(message);
    }

    auto stack = _STD stacktrace::current();
    auto _Position = 0;
    // Traverse the stack trace backwards
    for (
        auto frame = stack.rbegin();
        frame != stack.rend();
        ++frame, _Position++
        )
    {
        const auto& _Fr = *frame;
        if (_Position == stack.size() - 2) {
            writeln(
                "{} <-- [panic here at line {} in file {}]",
                _Fr.description().c_str(),
                _Fr.source_line(),
                _Fr.source_file().c_str());
        }
        else {
            writeln(
                "[{}] {}\n",
                _Fr.source_line(),
                _Fr.description().c_str());
        }
    }

    writeln(
        "\n[panic]: {}\n", message
    );

    std::unreachable();
}

template<class ...Types>
auto fmt(std::format_string<Types...> _Fmt,
    Types&&... _Args) noexcept -> std::string 
{
    return std::format(_Fmt, _STD move(_Args)...);
}

template<class... Types>
FTD_NORETURN constexpr inline auto vpanic(
    const std::format_string<Types...> _Fmt,
    Types&&... Args) 
{
    auto fmt = std::format<Types...>(_Fmt, _STD move(Args)...);
    panic<void>(fmt.c_str());
}

template<typename _Void = std::void_t<void>>
FTD_NORETURN constexpr inline auto panic_if(BOOL _Cond, const char* _Msg) -> _Void {
    if (_Cond) FTD_UNLIKELY
        _UTL panic<_Void>(_Msg);
}

template<typename _Void = std::void_t<void, size_t>>
FTD_NORETURN constexpr inline auto panic_if_not(BOOL _Cond, const char* _Msg) -> _Void {
    if (!_Cond) FTD_UNLIKELY
        _UTL panic<_Void>(_Msg);
}

FTD_NORETURN constexpr auto quit() -> decltype(auto) {
    _UTL panic("quit() was called");
}

template<typename T>
class iterator {
private:
    T* _Elems{ nullptr };
    size_t _Pos{ 0 };
public:
    iterator(T* _Elems, size_t const& _Size)
        : _Elems(_Elems), _Pos(_Size)
    {}

    using difference_type = uintptr_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    iterator<T>& operator ++() {
        _Elems++, _Pos++;
        return *this;
    }
    iterator<T> operator ++(int) {
        iterator self = *this;
        ++(*this);
        return self;
    }

    BOOL operator <(iterator<T> const& other) {
        return _Pos <= other._Pos;
    }
    BOOL operator >(iterator<T> const& other) {
        return !(*this < other);
    }

    iterator operator +(size_t number) {
        return { _Elems, _Pos + number };
    }

    //auto operator <=>(iterator const&) = default;

    BOOL operator==(iterator<T> other) const { return this->_Elems == other._Elems; }
    BOOL operator!=(iterator<T> other) const { return !(*this == other); }

    reference operator*() {
        return *_Elems;
    }

    inline iterator create(T* _Elems, size_t const& _Size) {
        return iterator(_Elems, _Size);
    }
};

template<class T>
class locked {
public:
    using value_type = T;

    enum class response {
        wait_if_locked,
        skip_if_locked
    };
private:
    value_type m_value;
    std::mutex m_lock;
public:

    locked(T&& initializer) : m_value(_STD move(initializer)) {}
    template<class ...Types>
    locked(Types... args) noexcept (noexcept (T{ args... })) {
        m_value = T{ args... };
    }

    template<class F>
    void edit(const F& editor, 
              const response& response = response::wait_if_locked) {
        if (response == response::skip_if_locked) {
            if (!m_lock.try_lock())
                return;
        }
        else {
            while (!m_lock.try_lock()) {}
        }
        editor(m_value);
        m_lock.unlock();
    }
};

template<class T, class E>
class Result {
private:
    T _Value;
    E _Error;
    BOOL _HasValue;
public:
    Result(T&& _Value) : _Value(std::move(_Value)), _HasValue(true) {}
    Result(E&& _Error) : _Error(std::move(_Error)), _HasValue(false) {}

    BOOL has_value() const {
        return _HasValue;
    }

    T value() const {
        return _Value;
    }

    E error() const {
        return _Error;
    }

    T expect(std::string message) const {
        if (!_HasValue) {
            panic(message.c_str());
        }
        return _Value;
    }
};

_UTIL_API_END

#endif // !__COMMON_H