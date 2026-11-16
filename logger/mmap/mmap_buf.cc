#include "mmap_buf.h"

#include <string.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint8_t、std::uint32_t
#include <algorithm>  //std::max

#include "utils/file_util.h"
#include "utils/sys_util.h"

namespace logger {

static constexpr std::size_t KDefaultCapacity = 512 * 1024;  // 512KB

MMapBuffer::MMapBuffer(fpath file_path) : file_path_(file_path), header_(nullptr), capacity_(0) {
    std::size_t file_size = fs::GetFileSize(file_path);
    std::size_t target_size = std::max(file_size, KDefaultCapacity);
    Reserve_(target_size);
    Init_();
}

void MMapBuffer::Resize(std::size_t new_size) {
    if (!IsValid_()) {
        return;
    }

    EnsureCapacity_(new_size);
    Header_()->size = new_size; 
}

std::uint8_t* MMapBuffer::Data() const {
    if (!IsValid_()) {
        return nullptr;
    }

    return static_cast<uint8_t*>(header_) + sizeof(MMapHeader);
}

std::size_t MMapBuffer::Size() const {
    if (!IsValid_()) {
        return 0;
    }

    return Header_()->size;
}

void MMapBuffer::Clear() {
    if (!IsValid_()) {
        return;
    }

    Header_()->size = 0; 
}

void MMapBuffer::Push(const void* data, std::size_t size) {
    if (!IsValid_()) {
        return;
    }

    std::size_t new_size = Size() + size;
    EnsureCapacity_(new_size);
    memcpy(Data() + Size(), data, size);
    Header_()->size = new_size;
}

double MMapBuffer::GetRatio() const {
    if (!IsValid_()) {
        return 0.0;
    }

    return static_cast<double>(Size()) / (capacity_ - sizeof(MMapHeader));
}

bool MMapBuffer::Empty() const {
    return Size() == 0;
}

static std::size_t GetValidCapacity_(std::size_t size) {
    std::size_t page_size = GetPageSize();
    return (size + page_size - 1) / page_size * page_size;
}

void MMapBuffer::Reserve_(std::size_t new_size) {
    if (new_size <= capacity_) {
        return;
    }

    new_size = GetValidCapacity_(new_size);
    Unmap_();
    TryMap_(new_size);
    capacity_ = new_size;
}

void MMapBuffer::EnsureCapacity_(std::size_t new_size) {
    std::size_t real_size = new_size + sizeof(MMapHeader);

    if (real_size <= capacity_) {
        return;
    }

    std::size_t target_size = capacity_;

    while (target_size < real_size){
        target_size += GetPageSize();
    }

    Reserve_(target_size);
    
}

std::size_t MMapBuffer::Capacity_() const {
    return capacity_;
}

bool MMapBuffer::IsValid_() const {
    MMapHeader* header = Header_();

    if (!header) {
        return false;
    }

    return header->magic == MMapHeader::KMagic;
}

MMapBuffer::MMapHeader* MMapBuffer::Header_() const {
    if (!header_) {
        return nullptr;
    }

    if (capacity_ < sizeof(MMapHeader)) {
        return nullptr;
    }

    return static_cast<MMapHeader*>(header_);
}

void MMapBuffer::Init_() {
    MMapHeader* header = Header_();

    if (!header) {
        return;
    }

    if (header->magic != MMapHeader::KMagic) {
        header->magic = MMapHeader::KMagic;
        header->size = 0;
    }
}

}