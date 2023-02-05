
#include "common.h"

_UTIL_API

inline static constexpr ptrdiff_t _Diff =
#if defined (X64)
8;
#else
4;
#endif

inline static constexpr ptrdiff_t _SizeT_size =
#if defined (X64)
64;
#else
32;
#endif

class _Bit_Field_Proxy {
private:
    uint8_t* _Bit_Proxy{ nullptr };
    ptrdiff_t _Bit_Loc{ 0 };
public:
    _Bit_Field_Proxy(uint8_t* _Byte, ptrdiff_t const& _Diff) {
        _Bit_Proxy = _Byte;
        _Bit_Loc = _Diff * ::util::_Diff;
    }

    bool is_set() const noexcept { 
        return (*(_Bit_Proxy) << _Bit_Loc) & 1; 
    }

    void set() noexcept {
        auto& _Proxy = *_Bit_Proxy;
        _Proxy |= (1 << _Bit_Loc);
    }
};

class bit_field {
private:
    uint8_t* m_bits{ nullptr };
    size_t m_capacity{ NULL };
    size_t m_real_bytecount{ 0 };
public:
    using difference = ptrdiff_t;

    constexpr bit_field(size_t _Capacity) noexcept {
        if (!(_Capacity % _Diff == 0)) {
            panic("capacity is unaligned");
        }

        m_capacity = _Capacity;
        m_real_bytecount = (_Capacity / _Diff);
        m_bits = new uint8_t[m_real_bytecount];
    }

    virtual ~bit_field() noexcept {
        delete m_bits;
    }

    difference count() const noexcept {
        return m_real_bytecount * _Diff;
    }

    _Bit_Field_Proxy at(difference const& index) {
        const auto size = count();
        if (index < 0 || index > size) {
            panic("out of range index access inside a bit_field");
        }
        auto Bit_Spot = (index * _Diff);
        auto* Bit = &m_bits[index / _Diff];
        return { Bit, Bit_Spot };
    }

    void set(difference const& index) {
        const auto _Size = count();
        if (index < 0 || index > _Size) {
            panic("out of range index access inside a bit_field");
        }
        auto _Bit_Spot = (index * _Diff);
        auto* _Bit = &m_bits[index / _Diff];
        auto _Proxy = _Bit_Field_Proxy{ _Bit, _Bit_Spot };
        _Proxy.set();
    }
};

_UTIL_API_END