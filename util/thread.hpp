#pragma once

#include "common.h"

#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <pthread.h>
#endif

#define FTD_THREAD_API inline
#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif

namespace utl::detail {
    enum class ThreadPlatform {
        Windows, Linux
    };
    enum class ThreadError {
        FailedToSpawn,
        MissingCallback,
        ThreadNotStarted,
    };

    using err_t = size_t;
}

_UTIL_API

template<detail::ThreadPlatform P>
class thread;

#ifdef _WIN32

template<>
class thread<detail::ThreadPlatform::Windows> {
public: /* types */
    using callback = void(*)();
    using handle_type = HANDLE;
    using size_type = DWORD;
    using sec_attributes = SECURITY_ATTRIBUTES;

private:
    handle_type m_handle{ INVALID_HANDLE_VALUE };
    size_type m_id{ 0 };
    callback m_callback{};
    sec_attributes m_sec_attributes{ NULL };
    size_type m_stack_size{ NULL };
    size_type m_creation_flags{ NULL };
public:
    // no information about the thread initially.
    FTD_THREAD_API thread() = default;
    FTD_THREAD_API thread(const callback& _Cb) noexcept {
        this->setcb(_Cb);
    }
    FTD_THREAD_API ~thread() {
        join(0);
    }

    FTD_THREAD_API void setcb(callback cb) noexcept {
        this->m_callback = cb;
    }
    FTD_THREAD_API void setsec_attrs(sec_attributes attrs) noexcept {
        m_sec_attributes = attrs;
    }
    FTD_THREAD_API void set_stacksize(size_type size) noexcept {
        m_stack_size = size;
    }
    FTD_THREAD_API void set_creationflags(size_type flags) noexcept {
        m_creation_flags = flags;
    }

    NODISCARD FTD_THREAD_API const HANDLE& handle() const noexcept {
        return m_handle;
    }
    NODISCARD FTD_THREAD_API size_t id() const noexcept {
        return m_id;
    }
    NODISCARD FTD_THREAD_API size_t stack_size() const noexcept {
        return m_stack_size;
    }
    NODISCARD FTD_THREAD_API size_t creation_flags() const noexcept {
        return m_creation_flags;
    }

    NODISCARD FTD_THREAD_API const sec_attributes& security_attributes() const noexcept {
        return m_sec_attributes;
    }

    NODISCARD bool running() const noexcept {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    NODISCARD FTD_THREAD_API bool spawn() noexcept {
        auto result = this->_Spawn();

        if (result.has_value()) {
            this->m_handle = result.value();
            return true;
        }

        auto spawn_error = result.error();

        // panic if missing required argument (m_callback)
        if (spawn_error == detail::ThreadError::MissingCallback) {
            panic("cannot spawn thread due to no callback being specified.");
            return false; /*unreachable*/
        }
        if (spawn_error == detail::ThreadError::FailedToSpawn) {
            // runtime failure, notify caller.
            return false;
        }

        panic("unknown branch (unhandled error in thread<Windows>::spawn())");
    }
    FTD_THREAD_API DWORD suspend() noexcept {
        auto result = _Pause();

        if (result.has_value())
            return result.value();

        if (result.error() == detail::ThreadError::ThreadNotStarted) {
            _UTL panic("The thread has not been spawned, and cannot be suspended.");
            return (DWORD)-1;
        }

        _UTL panic("Unhandled path");
        return (DWORD)-1;
    }
    FTD_THREAD_API DWORD resume() noexcept {
        auto result = _Resume();

        if (result.has_value())
            return result.value();

        if (result.error() == detail::ThreadError::ThreadNotStarted) {
            _UTL panic("The thread has not been spawned, and cannot be resumed.");
            return (DWORD)-1;
        }

        _UTL panic("Unhandled path");
        return (DWORD)-1;
    }

    FTD_THREAD_API bool join(int exit_code = 0) noexcept {
        if (!running()) {
            return false;
        }
        return TerminateThread(m_handle, exit_code);
    }

private:

    FTD_THREAD_API 
        detail::Result<HANDLE, detail::ThreadError> _Spawn() noexcept 
    {
        if (!m_callback) {
            return detail::ThreadError::MissingCallback;
        }

        auto handle =
            CreateThread(
                &this->m_sec_attributes,
                this->m_stack_size,
                (LPTHREAD_START_ROUTINE)this->m_callback,
                NULL,
                this->m_creation_flags,
                &this->m_id);

        if (handle == INVALID_HANDLE_VALUE) {
            return detail::ThreadError::FailedToSpawn;
        }

        return handle;
    }

    FTD_THREAD_API 
        detail::Result<DWORD, detail::ThreadError> _Pause() noexcept 
    {
        if (!running())
            return detail::ThreadError::ThreadNotStarted;

        return SuspendThread(m_handle);
    }

    FTD_THREAD_API 
        detail::Result<DWORD, detail::ThreadError> _Resume() noexcept 
    {
        if (!running())
            return detail::ThreadError::ThreadNotStarted;

        return ResumeThread(m_handle);
    }
};

using Thread = thread<detail::ThreadPlatform::Windows>;

#endif // #ifdef _WIN32
#ifdef __linux__

template<>
class thread<detail::ThreadPlatform::Linux> {
public:
    using callback = void(*)();
    using handle_type = int;
    using size_type = size_t;
    using sec_attributes = size_t;

private:

public:

    thread() = default;
};

#endif

_UTIL_API_END