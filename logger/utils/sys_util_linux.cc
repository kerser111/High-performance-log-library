#include "sys_util.h"

#include <unistd.h>

namespace logger {

std::size_t GetPageSize() {
    return getpagesize();
}

std::size_t GetProcessId() {
    return static_cast<std::size_t>(::getpid());
}

std::size_t GetThreadId() {
    return static_cast<std::size_t>(::gettid());
}

void LocalTime(std::tm* tm, std::time_t* now) {
    localtime_r(now, tm);
}

}