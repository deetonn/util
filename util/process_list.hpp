#pragma once

/* 
   an in application process list.

   launch processes and become the kernel above them.
   The process in question will be a virtual class.
*/

#include "common.h"

#include <unordered_map>

_UTIL_API

class process_manager;

/* The process class, to be inherited */
class process {
private:
    std::string name{"<unknown>"};
protected:
    std::mutex m_lock{};
public:
    process() = delete;
    process(std::string _Name) : name(_Name) {}

    virtual int execute(std::vector<std::string> args) noexcept = 0;

    const process_manager& master() noexcept {
        return process_manager::request();
    }
};

struct process_security_token_t {
    // layout
    /*
    * first byte is sid, last 4 are process id
    * 
    * X X X X
    */
    char bytes[5];
};

struct process_info {
    uint8_t sid;                           // process security id 
    uint32_t pid;                          // process unique id
    struct process_security_token_t token; // security information
};

struct process_security_token_t convert(process_info* pInfo) noexcept {
    return *reinterpret_cast<struct process_security_token_t*>(pInfo);
}

class process_manager {
private:
    std::vector<process*> m_processes{};
    std::unordered_map<uintptr_t, process_info> m_internals{};
    
    static inline process_manager* s_self;
public:
    process_manager() {
        s_self = this;
    }

    void push_back(process* proc) noexcept {
        if (proc == nullptr) {
            panic("cannot add null process");
        }
        m_processes.push_back(proc);
    }

    static inline const process_manager& request() noexcept {
        if (process_manager::s_self == nullptr) {
            panic("process_manager has not been constructed");
        }
        return *process_manager::s_self;
    }
};

_UTIL_API_END