#pragma once

#define _UTIL_MEMORY_API_BEGIN namespace util {
#define _UTIL_MEMORY_API_END }

#define _NODISCARD [[nodiscard]]
#define _SAFEPTR_NODISCARD [[nodiscard("discarding the return value will instantly free the allocated memory and renders the call pointless")]]
#define _ALLOC_NODISCARD [[nodiscard("the function allocates and gives ownership, discarding this value is a memory leak")]]

#include <utility>
#include <assert.h>

_UTIL_MEMORY_API_BEGIN

template<class T>
class auto_deleted_pointer {
public:
    using Self = auto_deleted_pointer<T>;
private:
    T* m_ptr{ nullptr };
public:
    auto_deleted_pointer()
        : m_ptr(new T)
    {}

    _IMPLICIT auto_deleted_pointer (T const& _Initializer)
        : m_ptr(new T(_Initializer))
    {}

    ~auto_deleted_pointer()
    {
        delete m_ptr;
    }

    inline bool okay() { return m_ptr != nullptr; }
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

    friend std::ostream& operator << (std::ostream& _Ostr, Self const& self) {
        std::cout << self.get();
        return _Ostr;
    }
};

template<typename T>
using safe_ptr = auto_deleted_pointer<T>;

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

#define nullify(dst, type) util::memset<type>(dst, (uint8_t)0)

template<typename _Ty>
_ALLOC_NODISCARD _Ty* malloc() {
    return static_cast<_Ty*>(_STD malloc(sizeof(_Ty)));
}

template<typename _Ty>
_ALLOC_NODISCARD _Ty* malloc(_Ty const& _Init) {
    auto* _Mem = static_cast<_Ty*>(_STD malloc(sizeof(_Ty)));
    *_Mem = _STD move(_Init);
    return _Mem;
}

template<typename _Ty>
void free(_Ty* _Memory) {
    _STD free((void *)_Memory);
}

_UTIL_MEMORY_API_END