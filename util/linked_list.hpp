#pragma once

#include "common.h"

_UTIL_API

template<typename T>
struct list_node {
public:
    using node_type = list_node<T>*;
    using value_type = T*;

    node_type  prev;
    value_type current;
    node_type  next;

    bool is_last() const noexcept {
        return next == nullptr;
    }
    bool is_root() const noexcept {
        return prev == nullptr;
    }
};

template<typename T>
struct list_node_view {
public:
    list_node<T>::node_type   prev;
    list_node<T>::value_type* current;
    list_node<T>::node_type   next;
};

template<typename T>
class linked_list {
private:
    list_node<T> m_nodes;
public:
    linked_list(T&& first) {
        this->root = first;
        this->nodes.current = &root;
        this->nodes.prev = nullptr;
        this->nodes.next = nullptr;
    }

    list_node<T> const* const root() const noexcept {
        return m_nodes;
    }
};

_UTIL_API_END