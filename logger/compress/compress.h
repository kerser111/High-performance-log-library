#pragma once

#include <cstddef>
#include <memory>
#include <string>

namespace logger {
namespace compress {

class Compression {
public:
    virtual ~Compression() = default;

    virtual std::size_t Compress(const void* input, std::size_t input_size, void* output, std::size_t output_size) = 0;

    virtual std::size_t CompressedBound(std::size_t input_size) = 0;

    virtual std::string Decompress(const void* data, std::size_t size) = 0;

    virtual void ResetStream() = 0;
};

}
}