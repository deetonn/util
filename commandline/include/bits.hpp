
#include "common.h"

#define _VERIFY_BIT_POS(p, m) _STL_VERIFY(p <= m && 0 <= m, "out of range")

_UTIL_API

template<class _Ty>
class _Bit_Field_proxy {
private:
    _Ty* _Bitfield{ nullptr };
    size_t _Pos{ 0 };
public:
    using flag_type = typename _Ty::flag_type;
    using self = _Bit_Field_proxy<_Ty>;

    FTD_CONSTEXPR _Bit_Field_proxy(_Ty& _Bf, size_t _Pos) {
        _Bitfield = &_Bf;
        this->_Pos = _Pos;
    }

    FTD_CONSTEXPR typename flag_type value() const noexcept {
#if defined (_DEBUG)
        _STL_VERIFY(_Bitfield != nullptr, "dead proxy");
#endif
        auto& _Fl = _Bitfield->_mainf();
        return static_cast<flag_type>(_Fl & _Pos);
    }

    FTD_CONSTEXPR self& operator=(const flag_type& _Fl) {
        this->_Bitfield->set_at(this->_Pos, _Fl);
        return *this;
    }
};

/// <summary>
/// A bitfield using a 64-bit integer as its bit field.
/// </summary>
class bit_field64 {
private:
    uint64_t m_Bits{ 0 };
    uint8_t m_Pos{ 0 };

public:
    using difference = ptrdiff_t;
    using flag_type = BOOL;
    using proxy = _Bit_Field_proxy<bit_field64>;

    FTD_CONSTEXPR auto set_at(difference const& _Pos, flag_type const& _Flag) {
#if defined (_DEBUG)
        _STL_VERIFY(64 >= _Pos && 0 <= _Pos, "position is out of range");
#endif
        m_Bits |=
            (static_cast<unsigned long long>(1) << _Pos);
    }

    FTD_CONSTEXPR auto set_next(BOOL _Fl) noexcept {
        m_Bits |= 
            (static_cast<uint64_t>(_Fl) << 
                (m_Pos == 64 ? 64 : ++m_Pos));
    }

    FTD_CONSTEXPR proxy at(const size_t& _Pos) {
#if defined (_DEBUG)
        _VERIFY_BIT_POS(_Pos, this->capacity());
#endif
        return proxy{ *this, static_cast<size_t>(_Pos) };
    }

    FTD_CONSTEXPR size_t capacity() const noexcept {
        return 64;
    }

    uint64_t& _mainf() {
        return m_Bits;
    }
};

_UTIL_API_END