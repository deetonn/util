#pragma once

#include "common.h"

#define _Conditional_Noexcept(_Condition_) noexcept (_Condition_)
#define _Maybe_Constantdef template<bool _Const_Init = false>
#define UTIL_DEF

#define _Nodiscard nodiscard
#ifndef _NODISCARD
#define _NODISCARD [[_Nodiscard]]
#endif
#define _UTIL_NODISCARD(x) [[_Nodiscard(x)]]
#define UTIL_NODISCARD _UTIL_NODISCARD("ignoring this return value is not recommended.")

#define _Likely_
#define _Unlikely_
#define _FORBIDDEN [[deprecated("This api is forbidden")]]

_UTIL_API

_Maybe_Constantdef
UTIL_DEF class String {
private:
    std::string m_str{};
public:
    using size_type = std::size_t;
    using value_type = char;
    using underlying_type = std::string;

    String() = default;
    constexpr String(std::string _Str)
        : m_str(_Str)
    {}
    _Unlikely_ constexpr String(const char* _Str)
        : m_str(_Str)
    {}
    constexpr String(String&&) = default;

    constexpr size_type size() 
        _Likely_ const
        noexcept 
    {
        return m_str.size();
    }

    UTIL_NODISCARD constexpr const value_type* data()
        _Likely_ const
        _Conditional_Noexcept(_Const_Init) 
    {
        return m_str.data();
    }

    UTIL_NODISCARD constexpr underlying_type::const_iterator begin()
        _Likely_ const
        _Conditional_Noexcept(_Const_Init) 
    {
        return m_str.begin();
    }

    UTIL_NODISCARD underlying_type::const_iterator end()
        _Likely_ const
        _Conditional_Noexcept(_Const_Init)  
    {
        return m_str.end();
    }

    _FORBIDDEN inline bool destroy() {}
};

_Maybe_Constantdef
class wString {
private:
    std::wstring m_str{};
public:
    using size_type = std::size_t;
    using value_type = wchar_t;
    using underlying_type = std::wstring;

    wString() = default;
    constexpr wString(std::wstring _Str)
        : m_str(_Str)
    {}
    constexpr wString(wString&&) = default;

    constexpr size_type size()
        const
        noexcept
    {
        return m_str.size();
    }

    constexpr const value_type* data()
        const
        _Conditional_Noexcept(_Const_Init)
    {
        return m_str.data();
    }

    constexpr underlying_type::const_iterator begin()
        const
        _Conditional_Noexcept(_Const_Init)
    {
        return m_str.begin();
    }

    constexpr underlying_type::const_iterator end()
        const
        _Conditional_Noexcept(_Const_Init)
    {
        return m_str.end();
    }
};

class uInt32 {
private:
    uint32_t m_value{ 0 };
public:
    uInt32() = default;
    uInt32(const uInt32&) = default;
    //uInt32(const uint32_t& _Other)
    //    : m_value(_Other)
    //{}
    //uInt32(const double& _Other)
    //    : m_value((uint32_t)_Other)
    //{}

    uInt32(uint32_t _Initializer)
        : m_value(_STD move(_Initializer))
    {}

    uint32_t const&     native() const noexcept {
        return m_value;
    }
    uInt32              clamp(uint32_t max, uint32_t min) {
        return ::std::clamp(m_value, min, max);
    }
    std::string         to_string() {
        return std::format("{}", m_value);
    }

    constexpr static inline uint32_t zero{ 0 };

    uInt32 operator =(uint32_t& _Right) noexcept {
        m_value = _Right;
        return *this;
    }

    inline static uInt32 parse(std::string const& text) {
        try {
            auto ts = std::stoi(text);
            return (uint32_t)ts;
        }
        catch (...) {
            return uInt32::zero;
        }
    }
    inline static bool   try_parse(std::string const& text, uInt32* location) {
        if (!location)
            return false;
        try {
            auto ts = std::stoi(text);
            *location = uInt32{ (uint32_t) ts };
        }
        catch (...) {
            return false;
        }

        return true;
    }
};

class uInt64 {

};

_UTIL_API_END