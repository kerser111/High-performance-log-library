#include "mmap_buf.h"

#include <windows.h>

#include <memory>
#include <type_traits>

namespace logger {

class FileHandleDeleter {
public:
    void operator()(HANDLE h) {
        if (h != nullptr) {
            CloseHandle(h);
        }
    }
};

using FileHandlePtr = std::unique_ptr<std::remove_pointer_t<HANDLE>, FileHandleDeleter>;
bool MMapBuffer::TryMap_(std::size_t capacity) {
    FileHandlePtr file_handle = FileHandlePtr(CreateFileW(file_path_.wstring().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
    
    if (file_handle.get() == INVALID_HANDLE_VALUE) {
        return false;
    }

    FileHandlePtr file_mapping_handle;
    file_mapping_handle.reset(CreateFileMapping(file_handle.get(), NULL, PAGE_READWRITE, 0, capacity, NULL));

    if (!file_mapping_handle) {
        return false;
    }

    header_ = MapViewOfFile(file_mapping_handle.get(), FILE_MAP_ALL_ACCESS, 0, 0, capacity);
    return header_ != nullptr;
}

void MMapBuffer::Unmap_() {
    if (header_) {
        UnmapViewOfFile(header_);
    }

    header_ = nullptr;
}

void MMapBuffer::Sync_() {
    if (header_) {
        FlushViewOfFile(header_, capacity_);
    }
}

}