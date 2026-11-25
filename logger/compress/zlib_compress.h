#pragma once

#include "third_party/zlib/include/zlib.h"

#include "compress/compress.h"

namespace logger {
namespace compress {

struct ZStreamDeflateDeleter {
    void operator()(z_stream* stream) {
        if (stream) {
            deflateEnd(stream);
            delete stream;
        }
    }
};

struct ZStreamInflateDeleter {
    void operator()(z_stream* stream) {
        if (stream) {
            inflateEnd(stream);
            delete stream;
        }
    }
};

class ZlibCompress final : public Compression {
public:
    ~ZlibCompress() override = default;

    std::size_t Compress(const void* input, std::size_t input_size, void* output, std::size_t output_size) override;

    std::string Decompress(const void* data, std::size_t size) override;

    void ResetStream() override;

    std::size_t CompressedBound(std::size_t input_size) override;

private:
    void ResetUncompressStream_();

private:
    std::unique_ptr<z_stream, ZStreamDeflateDeleter> compress_stream_;
    std::unique_ptr<z_stream, ZStreamInflateDeleter> uncompress_stream_;
};

}
}