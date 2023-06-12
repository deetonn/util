#pragma once

#include "common.h"

#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <sstream>

_UTIL_DETAIL_BEGIN

namespace std_fs = std::filesystem;

class path {
private:
    std::string full_path{};
    bool is_rooted{ false };
public:
    path() = default;
    path(const char* _Path)
        : path(std::string(_Path)) {}
    path(const std::string& data) noexcept {
        std::string fp = data;
        is_rooted = data.starts_with("C:\\");
        if (!is_rooted) {
            auto current = path::current();
            if (data.starts_with("\\") || data.starts_with("/")) {
                fp = current.data() + data;
            }
            else {
                fp = fp.substr(0, fp.size() - 2);
                fp = current.data() + fp;
            }
        }
        else {
            full_path = fp;
        }
    }

    FTD_CONSTEXPR bool is_empty() const noexcept {
        return full_path.empty();
    }

    BOOL rooted() const noexcept {
        return is_rooted;
    }
    const std::string& data() const noexcept {
        return full_path;
    }

    static inline path current() {
        return std_fs::current_path().string();
    }
};

_UTIL_DETAIL_END

/* currently windows only, because I only use windows */

_UTIL_API

namespace std_fs = std::filesystem;

enum class FsErrorCode {
    Success,
    PathNotFound,
    ExpectedFileGotDirectory,
    FailedToOpenFile
};

struct FsError {
    FsErrorCode code{ FsErrorCode::Success };
    std::string msg;
};

auto make_fs_error(FsErrorCode code, const std::string& msg) noexcept {
    return FsError{ code, msg };
}

class filesystem {
public:
    static std::vector<_DETAIL path> read_directory(
        const _DETAIL path& _Path) noexcept 
    {
        std::vector<_DETAIL path> results = {};
        for (const auto& entry 
            : std_fs::directory_iterator(_Path.data())) {
            results.push_back(entry.path().string());
        }
        return results;
    }

    static Result<std::string, FsError> read_file(const _DETAIL path& _Path) noexcept {

        /* cannot read a file that does not exist */
        if (!std_fs::exists(_Path.data())) {
            return make_fs_error(FsErrorCode::PathNotFound, fmt("The specified file does not exist. ({})",_Path.data().c_str()));
        }

        /* cannot read the contents of a directory */
        if (std_fs::is_directory(_Path.data())) {
             return make_fs_error(
                 FsErrorCode::ExpectedFileGotDirectory,
                 fmt("The path ({}) is a directory and cannot be read.",
                    _Path.data().c_str()));
        }

        std::ifstream handle(_Path.data());

        if (!handle.is_open()) {
            return make_fs_error(
                FsErrorCode::FailedToOpenFile, 
                fmt("Failed to open specified file (Errno: {})",
                    (int)handle.rdstate()));
        }

        std::stringstream result = {};
        result << handle.rdbuf();
        return result.str();
    }
};

auto _Mfs_details() {

}

_UTIL_API_END