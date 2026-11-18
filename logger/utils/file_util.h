#pragma once

#include <cstddef>
#include <filesystem>

namespace logger {

namespace fs {

std::size_t GetFileSize(const std::filesystem::path&);

}

}