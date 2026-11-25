#pragma once

#include "third_party/zstd/lib/zstd.h"

#include "compress/compress.h"

namespace logger {
namespace compress {

class ZstdCompress final : public Compression {
public:
    ZstdCompress();

    ~ZstdCompress() override;

    std::size_t Compress(const void* input, std::size_t input_size, void* output, std::size_t output_size) override;

    std::string Decompress(const void* data, std::size_t size) override;

    void ResetStream() override;

    std::size_t CompressedBound(std::size_t input_size) override;

private:
    void ResetUncompressStream_();

private:
    ZSTD_CCtx* cctx_;
    ZSTD_DCtx* dctx_;
};

}
}
