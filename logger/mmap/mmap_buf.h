#pragma once

#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint8_t、std::uint32_t
#include <filesystem>

namespace logger {

class MMapBuffer {
public:
    using fpath = std::filesystem::path;

    explicit MMapBuffer(fpath file_path);

    ~MMapBuffer() = default;

    MMapBuffer(const MMapBuffer&) = delete;

    MMapBuffer& operator=(const MMapBuffer&) = delete;

    void Resize(std::size_t new_size);

    std::uint8_t* Data() const;

    std::size_t Size() const;

    void Clear();

    void Push(const void* data, std::size_t size);

    double GetRatio() const;

    bool Empty() const;

private:
    struct MMapHeader {
        static constexpr std::uint32_t KMagic = 0xdeadbeef;
        std::uint32_t magic = KMagic;
        std::uint32_t size;
    };
    
private:
    void Reserve_(std::size_t new_size);

    void EnsureCapacity_(std::size_t new_size);

    std::size_t Capacity_() const;

    bool TryMap_(std::size_t capacity);

    void Unmap_();

    void Sync_();

    bool IsValid_() const;

    MMapHeader* Header_() const;

    void Init_();

private:
    fpath file_path_;
    void* header_;
    std::size_t capacity_;
};

}