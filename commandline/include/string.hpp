#pragma once

#include "common.h"
#include "memory_util.h"

_UTIL_API

// no support for wide characters

template<typename alloc = _UTL default_allocator>
class string {
public:
    using char_type = char;
    using size_type = size_t;
private:
    char_type* m_bytes{ 0 };
    alloc  m_alloc{};
public:
    string() = default;
    string(const string& s)
        : string(s.c_str())
    {}
    string(string&&) = default;

    string(const char_type* s) {
        const auto size = strlen(s) + 1;
        m_bytes = m_alloc.alloc_many<char>(size);
        FTD_VERIFY(m_bytes != nullptr, "failed to allocate space for string contents");
        std::memcpy(m_bytes, s, strlen(s));
        m_bytes[size - 1] = '\0';
    }
    FTD_CONSTEXPR ~string() {
        if (m_bytes) m_alloc.destroy(m_bytes);
    }

    FTD_CONSTEXPR size_t size() const noexcept {
        return (m_bytes == 0) ? 0 : strlen(m_bytes);
    }
    FTD_CONSTEXPR alloc& allocator() noexcept {
        return m_alloc;
    }

    const char_type* c_str() const noexcept {
        return m_bytes;
    }
    void clear() noexcept {
        if (m_bytes)
        {
            delete m_bytes;
            m_bytes = nullptr;
        }
    }

    FTD_CONSTEXPR static size_t length(const char* s) noexcept {
        if (!s)
            return 0;
        size_t l = 0;
        for (; *s == '\0'; ++l) {}
        return l;
    }

    friend std::ostream& operator <<(std::ostream& _Ostr, const string& s) noexcept {
        _Ostr << s.m_bytes;
        return _Ostr;
    }
};

_UTIL_API_END