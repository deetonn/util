#pragma once

#define _UTIL_MEMORY_API_BEGIN _UTIL_API
#define _UTIL_MEMORY_API_END _UTIL_API_END

#define _SAFEPTR_NODISCARD [[nodiscard("discarding the return value will instantly free the allocated memory and renders the call pointless")]]
#define _ALLOC_NODISCARD [[nodiscard("the function allocates and gives ownership, discarding this value is a memory leak")]]

#include <utility>
#include <assert.h>

#include "io_util.h"

_UTIL_MEMORY_API_BEGIN

template<class T>
class default_delete {
public:
    using Self = default_delete<T>;
private:
    T* m_ptr{ nullptr };
public:
    default_delete()
        : m_ptr(new T)
    {}

    _IMPLICIT default_delete (T const& _Initializer)
        : m_ptr(new T(_Initializer))
    {}

    ~default_delete()
    {
        delete m_ptr;
    }

    inline BOOL okay() { return m_ptr != nullptr; }
    inline const T& get() const { return *m_ptr; }
    inline T* unsafe_get() { return m_ptr; }

    inline void swap(Self& _Other) {
        if (!_Other.okay() || !this->okay())
            return;
        T _Tmp = *_Other.m_ptr;
        *_Other.m_ptr = *this->m_ptr;
        *this->m_ptr = _Tmp;
    }

    inline T& operator *() {
        //if (!okay()) throw std::exception("attempt to dereference nullptr");
        return *m_ptr;
    }
    inline T* operator ->() { return m_ptr; }
    inline T* operator &() { return m_ptr; }

    auto _Gett() {
        return m_ptr;
    }

    friend std::ostream& operator << (std::ostream& _Ostr, Self const& self) {
        std::cout << self.get();
        return _Ostr;
    }
};

template<typename T>
using safe_ptr = default_delete<T>;

template<typename _Ty>
_SAFEPTR_NODISCARD safe_ptr<_Ty> make_safe() { 
    return safe_ptr<_Ty>(); 
}

template<typename _Ty>
_SAFEPTR_NODISCARD inline safe_ptr<_Ty> make_safe(const _Ty& _Initializer) {
    return safe_ptr<_Ty>(_Initializer); 
}

template<typename _Ty, typename ..._Args>
_SAFEPTR_NODISCARD inline safe_ptr<_Ty> make_safe(_Args&&... args) {
    auto _Initializer = _Ty(_STD forward<_Args>(args)...);
    return make_safe(_STD move(_Initializer));
}

/*
* set each individual byte of _Data
*/
inline size_t memset(void* _Data, uint8_t _Byte, size_t _Length)
{
    auto* _Bytes = static_cast<uint8_t*>(_Data);
    for (auto i = 0u; i < _Length; ++i)
        _Bytes[i] = _Byte;
    return _Length;
}

template<class T>
inline size_t memset(T* _Data, uint8_t _Byte) {
    auto* _Bytes = reinterpret_cast<uint8_t*>(_Data);
    constexpr auto _Length = sizeof(T);
    for (auto i = 0u; i < _Length; ++i)
        _Bytes[i] = _Byte;
    return _Length;
}

template<class T>
inline size_t memset(safe_ptr<T>& _Ptr, uint8_t _Byte) {
    return _UTL memset(_Ptr._Gett(), _Byte);
}

#define nullify(dst, type) util::memset<type>(dst, (uint8_t)0)

template<typename _Ty>
_ALLOC_NODISCARD _Ty* malloc() {
    return static_cast<_Ty*>(_STD malloc(sizeof(_Ty)));
}

template<typename _Ty>
_ALLOC_NODISCARD _Ty* malloc(_Ty const& _Init) {
    auto* _Mem = static_cast<_Ty*>(_STD malloc(sizeof(_Ty)));
    if (!_Mem)
        _UTL panic("failed to allocate");
    *_Mem = _STD move(_Init);
    return _Mem;
}

template<typename _Ty>
void free(_Ty* _Memory) {
    _STD free((void *)_Memory);
}

template<class _Ty>
class never_deleted {
private:
    _Ty* _Al{ nullptr };
public:
    never_deleted()
        : _Al(new _Ty)
    {}
    never_deleted(_Ty const& _Li)
        : _Al(new _Ty{ _STD move(_Li) })
    {}

    _Ty const* ptr() const noexcept {
        return _Al;
    }

    _Ty* unsafe_ptr() const noexcept {
        return _Al;
    }
};

class default_allocator {
private:
    size_t _M_ttl_bytes{ 0 };
public:
    template<typename T>
    inline T* alloc() {
        _M_ttl_bytes += sizeof(T);
        T* _Mem = new(std::nothrow) T;
        if (!_Mem) {
            _UTL panic("failed to allocate");
        }
        return _Mem;
    }

    template<typename T>
    inline T* alloc_many(size_t _Total) {
        _M_ttl_bytes += (_Total * sizeof(T));
        T* _Mem = new(std::nothrow) T[_Total];
        if (!_Mem)
            _UTL panic("failed to allocate");
        return _Mem;
    }

    template<typename T>
    inline void destroy(T* _Mem) {
        delete _Mem;
    }

    inline const size_t& total() const noexcept {
        return _M_ttl_bytes;
    }
};

template<typename T>
class memory_view {
private:
    uint8_t* m_view{ nullptr };
public:
    memory_view(T& _Mref) noexcept {
        m_view = reinterpret_cast<uint8_t*>(&_Mref);
    }

    FTD_CONSTEXPR size_t size() const noexcept(true) {
        return sizeof T;
    }

    auto view() const noexcept -> std::void_t<int> {
        _UTL writeln("Memory View for {} ({} bytes)\n", _UTL type_info<T>::name(), size());
        for (auto i = 0; i < size(); ++i) {
            if (i % 5 == 0) {
                std::cout << '\n' << (void*)(m_view + i) << ": ";
            }
            else {
                std::cout << (int) m_view[i] << "   ";
            }
        }
    }

    uint8_t& operator[](size_t _Pos) noexcept {
#if defined (_DEBUG)
        _STL_VERIFY(_Pos >= 0 && _Pos <= size(), "Out of range access");
#endif
        return *(m_view + _Pos);
    }
};

typedef struct {
    void* address;
    sstring str;
    BOOL x32;
} address_info;

template<typename T>
auto addressof(T& t) -> address_info {
    address_info info = {
        (void*)&t,
        std::format("{}", (void*) &t)
    };
    info.x32 = !(info.str.size() > 10);
    return info;
}

/* not_null<T> */

template<class T>
concept NonNullWorthy = !std::is_pointer_v<T>;

template<NonNullWorthy T>
class non_null {
private:
    T m_value;
public:
    non_null() = delete;
    non_null(const T& _Initializer) noexcept
        : m_value {_Initializer}
    {}
    non_null(T&& _Initializer)
        : m_value{ _STD move(_Initializer) }
    {}

    T& value() noexcept {
        return m_value;
    }
    const T& value() const noexcept {
        return m_value;
    }
};

template<class T>
class non_null_ptr {
private:
    T* pointer{ nullptr };
public:
    non_null_ptr(T* _Ptr) noexcept
    {
        FTD_VERIFY(_Ptr != nullptr, "cannot create a non_null_ptr<T> with a nullptr.");
        this->pointer = _Ptr;
    }
    non_null_ptr(const T& _Value)
    {
        auto* ptr = new (std::nothrow) T(_Value);
        FTD_VERIFY(ptr != nullptr, "failed to allocate space for non_null_ptr");
        this->pointer = ptr;
    }
    non_null_ptr(T&& _Move)
        : non_null_ptr{ _STD move(_Move) }
    {}

    T* value() noexcept {
        return this->pointer;
    }
    const T* value() const noexcept {
        return this->pointer;
    }
};

template<class T>
inline T bit_cast(void* data) noexcept {
    return reinterpret_cast<T>(data);
}

_UTIL_MEMORY_API_END