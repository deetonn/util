#pragma once

#include "common.h"

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
};

template<typename T>
class ErrorOr
{
private:
    Error* m_problem{ nullptr };
    T* m_value{ nullptr };
public:
    ErrorOr(Error&& _Problem) {
        m_problem = static_cast<Error*>(_STD malloc(sizeof(Error)));
        if (!m_problem) {
            throw std::exception("out of memory");
        }
        *m_problem = _STD move(_Problem);
    }
    ErrorOr(T&& _Value) {
        m_value = static_cast<T*>(_STD malloc(sizeof(T)));
        if (!m_value) {
            throw std::exception("out of memory");
        }
        *m_value = _STD move(_Value);
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

    bool has_value() {
        return m_value != nullptr;
    }
    bool has_problem() {
        return m_problem != nullptr;
    }
};

// Need Statement Expressions
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html

#define TRY(name, expression) auto _Temp_$##name = expression; if (_Temp_$##name.has_problem()) { return _Temp_$##name.error(); } auto name = _Temp_$##name.unwrap();

_UTIL_API_END