#pragma once

#include "common.h"
#include "memory_util.h"

#ifndef FTD_STRING_USES_STRLEN
#define FTD_STRING_USES_STRLEN 1
#endif

#define FTD_STRING_MAYBE_NONNULL_TERMINATED

_UTIL_API

// no support for wide characters

template<typename alloc = _UTL default_allocator>
class basic_string {
public:
    using char_type = char;
    using value_type = char_type;
    using size_type = size_t;
    using self = basic_string;
    using iterator = utl::iterator<char_type>;
    using const_iterator = utl::const_iterator<char_type>;

    static inline std::string default_value_key = "ftd::string::default";
private:
    char_type* m_bytes{ 0 };
    alloc      m_alloc{};
public:
    basic_string() noexcept {
        auto default_string =
            settings::get_dynamic<std::string>(default_value_key);

        if (default_string != nullptr) {

            const auto size = default_string->size() + 1;
            m_bytes = m_alloc.alloc_many<char>(size);
            FTD_VERIFY(m_bytes != nullptr, "failed to allocate space for string contents");
            std::memcpy(m_bytes, default_string->c_str(), default_string->size());
            m_bytes[size - 1] = '\0';
        }
        
    }
    basic_string(const basic_string& s)
        : basic_string(s.c_str())
    {}
    basic_string(basic_string&&) = default;

    basic_string(const char_type* s) {
        const auto size = length(s) + 1;
        m_bytes = m_alloc.alloc_many<char>(size);
        FTD_VERIFY(m_bytes != nullptr, "failed to allocate space for string contents");
        std::memcpy(m_bytes, s, length(s));
        m_bytes[size - 1] = '\0';
    }

    FTD_STRING_MAYBE_NONNULL_TERMINATED 
        basic_string(char_type* s, size_t size) 
    {
        FTD_VERIFY(s != nullptr, "cannot copy a nullptr");
        m_bytes = m_alloc.alloc_many<char>(size);
        FTD_VERIFY(m_bytes != nullptr, "failed to allocate space for string contents");
        std::memcpy(m_bytes, s, size);
    }

    FTD_CONSTEXPR ~basic_string() {
        if (m_bytes) {
            m_alloc.destroy(m_bytes);
        };
    }

    FTD_CONSTEXPR size_t size() const noexcept {
        return (m_bytes == 0) ? 0 : length(m_bytes);
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

    bool is_empty() const noexcept {
        return m_bytes == nullptr || strcmp(m_bytes, "");
    }

    basic_string::self  substr(size_t start, size_t length) const noexcept {
        return basic_string(m_bytes + start, length);
    }
    basic_string::self& append(const char* const other) noexcept {
        const auto _Our_size = size();
        const auto _Their_size = length(other);
        const auto _Required_space =
            (_Our_size + _Their_size) + 1;

        auto& all = allocator();
        char* buff = all.alloc_many<char>(_Required_space);

        FTD_VERIFY(buff != nullptr, "failed to allocate space for string contents");

        std::memcpy(buff, m_bytes, _Our_size);
        std::memcpy(buff + _Our_size, other, _Their_size);

        all.destroy(m_bytes);
        m_bytes = buff;

        return *this;
    }

    FTD_CONSTEXPR static size_t length(const char* s) noexcept {
        if constexpr (FTD_STRING_USES_STRLEN) {
            return std::strlen(s);
        }
        else {
            if (!s)
                return 0;
            size_t l = 0;
            for (; *s == '\0'; ++l) {}
            return l;
        }
    }

    iterator begin() noexcept {
        return iterator(
            m_bytes,
            size()
        );
    }
    iterator end() noexcept {
        auto it = iterator(
            m_bytes + size(),
            size()
        );
        return it;
    }

    //const_iterator begin() const noexcept {
    //    return const_iterator(
    //        m_bytes,
    //        size()
    //    );
    //}
    //const_iterator begin() const noexcept {
    //    return const_iterator(
    //        m_bytes + size(),
    //        size()
    //    );
    //}

    basic_string& operator =(const basic_string& s) {
        this->clear();
        const auto size = length(s.c_str()) + 1;
        m_bytes = m_alloc.alloc_many<char>(size);
        FTD_VERIFY(m_bytes != nullptr, "failed to allocate space for string contents");
        std::memcpy(m_bytes, s.c_str(), size - 1);
        m_bytes[size - 1] = '\0';
        return *this;
    }
    friend std::ostream& operator <<(std::ostream& _Ostr, const basic_string& s) noexcept {
        _Ostr << s.m_bytes;
        return _Ostr;
    }
};

using string = basic_string<_UTL default_allocator>;

_UTIL_API_END