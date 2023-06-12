#pragma once

#include "common.h"

#ifndef __NODISCARD
#define __NODISCARD nodiscard
#endif

#ifndef _NODISCARD
#define _NODISCARD [[__NODISCARD]]
#endif

#ifndef _NODISCARD_MSG
#define _NODISCARD_MSG(_Msg) [[__NODISCARD(_Msg)]]
#endif

#define _ALGO_NODISCARD _NODISCARD_MSG("Discard the result of this algorithm function will cause an un-needed enumeration. This enumeration can be linear, which is a waste of cpu cycles.")

#ifndef NODISCARD
#define NODISCARD _NODISCARD
#endif

_UTIL_API

template<class Container, 
    typename Element = typename Container::value_type,
    typename Iterator = typename Container::iterator>
_ALGO_NODISCARD Iterator find(Container& container, Element& element) noexcept {
    auto end = container.end();
    for (
        auto it = container.begin();
        it != container.end();
        it++
        ) {
        auto& value = *it;
        if (value == element) {
            return it;
        }
    }
    return end;
}

template<class Container, typename Element = typename Container::value_type>
_ALGO_NODISCARD bool contains(
    const Container& container,
    const Element& value) noexcept
{
    return find(container, value) != container.end();
}

template<class Container, typename Element = typename Container::value_type>
FTD_CONSTEXPR _ALGO_NODISCARD Element& at(
    Container& container, 
    size_t offset) noexcept {
    return container.at(offset);
}

template<class Container, typename Element = typename Container::value_type>
FTD_CONSTEXPR const Element& at(
    const Container& container,
    size_t offset
) noexcept {
    return container.at(offset);
}

template<class Container, class _Fx, typename Element = typename Container::value_type>
FTD_CONSTEXPR auto for_each(
    const Container& container,
    _Fx _Fn) noexcept 
{
    try {
        for (const auto& _Element : container) {
            _Fn(_Element);
        }
    }
    catch (...) {

    }
}

template<class Container, class _Fx, typename Element = typename Container::value_type>
FTD_CONSTEXPR auto for_each(
    Container& container,
    _Fx _Fn) noexcept
{
    try {
        for (auto& _Element : container) {
            _Fn(_Element);
        }
    }
    catch (...) {

    }
}

template<class Container, class _Fx, class Element = typename Container::value_type>
FTD_CONSTEXPR _ALGO_NODISCARD bool any(
    const Container& container,
    _Fx predicate) noexcept
{
    try {
        for (const Element& element : container) {
            if (predicate(element)) {
                return true;
            }
        }
    }
    catch (...) {

    }

    return false;
}

enum class FirstIndexOfError {
    ElementNotFound,
    NotComparable
};

template<class Container, 
         class Element = typename Container::value_type,
         class Size = typename Container::size_type>
FTD_CONSTEXPR _ALGO_NODISCARD 
_DETAIL Result<Size, FirstIndexOfError> first_index_of(
    const Container& container,
    const Element& element) 
noexcept(true)
{
    if constexpr (!std::equality_comparable<Element>) {
#if _DEBUG
        _UTL panic("unable to compare element type (type {} has no `==` operator)",
            _UTL nameof<Element>());
#else
        return FirstIndexOfError::NotComparable;
#endif
    }

    try {
        for (auto index = 0;
            index < container.size();
            ++index) 
        {
            const auto& saved_element = container.at(index);
            if (saved_element == element) {
                return index;
            }
        }
    }
    catch (...) {}

    return FirstIndexOfError::ElementNotFound;
}

_UTIL_API_END