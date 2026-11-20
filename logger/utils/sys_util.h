#pragma once

#include <cstddef>
#include <ctime>

namespace logger {

std::size_t GetPageSize();

std::size_t GetProcessId();

std::size_t GetThreadId();

void LocalTime(std::tm* tm, std::time_t* now);

}