#pragma once

#include <cstddef>
#include <ctime>

namespace logger {

std::size_t GetPageSize();

std::size_t GetProcessID();

std::size_t GetThreadID();

void LocalTime(std::tm* tm, std::time_t* now);

}