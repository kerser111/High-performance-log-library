#include "sys_util.h"

#include <windows.h>

namespace logger {

std::size_t GetPageSize() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

std::size_t GetProcessID() {
    return static_cast<std::size_t>(::GetCurrentProcessId());
}

std::size_t GetThreadID() {
    return static_cast<std::size_t>(::GetCurrentThreadId());
}

void LocalTime(std::tm* tm, std::time_t* now) {
    localtime_s(tm, now);
}

}