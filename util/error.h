#pragma once

#include "common.h"

#include <cassert>
#include <format>

_UTIL_API

class Error {
private:
    const char* m_what;
public:
    Error(const char* _What)
        : m_what(_What)
    {}

    virtual const char* what() const noexcept {
        return m_what;
    }

    static inline Error from(const char* msg) {
        return Error(msg);
    }
};

inline auto make_error(const char* _Msg) noexcept -> Error {
    return Error::from(_Msg);
}

template<class... Types>
inline auto make_formatted_error(std::format_string<Types...> _Fmt,
    Types&&... _Args) -> Error 
{
    std::string _F = std::format<Types...>(_Fmt, _STD move(_Args)...);
    return Error::from(_F.c_str());
}

template<typename T>
class ErrorOr
{
private:
    Error* m_problem{ nullptr };
    T* m_value{ nullptr };
public:
    _IMPLICIT ErrorOr(const Error& _Problem) {
        m_problem = static_cast<Error*>(_STD malloc(sizeof(Error)));
        if (!m_problem) {
            _UTL panic("cannot allocate for ErrorOr due to lack of memory.");
        }
        *m_problem = _Problem;
    }
    _IMPLICIT ErrorOr(const T& _Value) {
        m_value = static_cast<T*>(_STD malloc(sizeof(T)));
        if (!m_value) {
            _UTL panic("cannot allocate for ErrorOr due to lack of memory.");
        }
        *m_value = _Value;
    }

    ~ErrorOr() {
        if (m_problem)
            _STD free(m_problem);
        if (m_value)
            _STD free(m_value);
    }

    T unwrap() {
        return *m_value;
    }
    T unwrap_or(const T& _Other) {
        if (!has_value())
            return _Other;
        return unwrap();
    }

    Error error() {
        return *m_problem;
    }

    BOOL has_value() {
        return m_value != nullptr;
    }
    BOOL has_problem() {
        return m_problem != nullptr;
    }
};

// Need Statement Expressions
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html

#define TRY(name, expression) auto _Temp_$##name = expression; if (_Temp_$##name.has_problem()) { return _Temp_$##name.error(); } auto name = _Temp_$##name.unwrap();
#define MUST(name, expression) auto _Temp_$##name = expression; if (_Temp_$##name.has_problem()) { util::panic(_Temp_$##name.error().what()); } auto name = _Temp_$##name.unwrap();

template<typename _Ty>
_Ty unwrap(ErrorOr<_Ty>& _Eo) {
    return _Eo.unwrap();
}

/// <summary>
/// Maybe T
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class maybe {
private:
    T* tee{ nullptr };
public:
    maybe() = default;
    maybe(maybe const&) = delete;
    maybe(maybe&&) = default;
    maybe(T value) {
        tee = new T;
        *tee = value;
    }

    /// <summary>
    /// Get the value and release the memory containing it.
    /// </summary>
    /// <returns></returns>
    FTD_NODISCARD T&& unwrap() noexcept {
        assert(this->tee != nullptr && "cannot unwrap `maybe` with no value");
        T copy = *tee;
        delete tee;
        return _STD move(copy);
    }
    FTD_NODISCARD T unwrap_or(T const& _Other) noexcept {
        if (!this->tee) {
            return _Other;
        }
        T copy = *tee;
        delete tee;
        return _STD move(copy);
    }
    FTD_NODISCARD BOOL has_value() const noexcept {
        return (tee != nullptr);
    }
    FTD_NODISCARD BOOL is_empty() const noexcept {
        return !has_value();
    }
};

template<typename T>
class compile_time_storage {
private:
    T m_t;
public:
    constexpr compile_time_storage(T const& _Val)
        : m_t(_Val)
    {}
    constexpr const T& value() const noexcept {
        return m_t;
    }
};

_UTIL_API_END