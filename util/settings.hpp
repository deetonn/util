#pragma once

#include "common.h"

#include <unordered_map>

#define SETTINGS_DECL static inline 

#ifndef SETTINGS_HAS_DYNAMICS
#define SETTINGS_HAS_DYNAMICS 1
#endif

#if SETTINGS_HAS_DYNAMICS
typedef struct {
    void* data;
} dynamic_setting;
#endif

_UTIL_API

class settings {
    /* this class is static */
private:
    SETTINGS_DECL std::unordered_map<std::string, bool> s_flags = {};
#if SETTINGS_HAS_DYNAMICS
    SETTINGS_DECL std::unordered_map<std::string, dynamic_setting> s_dynamics = {};
#endif
public:
    settings()                = delete;
    settings(settings const&) = delete;
    settings(settings&&)      = delete;

    SETTINGS_DECL bool __cdecl get(std::string const& name) noexcept {
        if (!s_flags.contains(name))
            return false;
        return s_flags[name];
    }
    SETTINGS_DECL void __cdecl set(std::string const& name, bool flag) noexcept {
        s_flags[name] = flag;
    }

#if SETTINGS_HAS_DYNAMICS

    SETTINGS_DECL void* __cdecl get_dynamic(std::string const& name) noexcept {
        if (!s_dynamics.contains(name)) {
            return nullptr;
        }
        return s_dynamics.at(name).data;
    }
    template<typename T>
    SETTINGS_DECL T* __cdecl get_dynamic(std::string const& name) noexcept {
        return static_cast<T*>(settings::get_dynamic(name));
    }

    SETTINGS_DECL void __cdecl set_dynamic(std::string const& name, void* data) noexcept {
        s_dynamics[name] = dynamic_setting{ data };
    }
    template<typename T>
    SETTINGS_DECL void __cdecl set_dynamic(std::string const& name, T* data) {
        settings::set_dynamic(name, static_cast<void*>(data));
    }

#endif

    SETTINGS_DECL size_t __cdecl count() noexcept {
        return s_flags.size();
    }
};

static inline bool enabled(const std::string& s) noexcept {
    return settings::get(s);
}

_UTIL_API_END