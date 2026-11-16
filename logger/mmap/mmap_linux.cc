#include "mmap_buf.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <defer.h>

namespace logger {
    
bool MMapBuffer::TryMap_(std::size_t capacity) {
    int fd = open(file_path_.string().c_str(), O_RDWR | O_CREAT, 0700);
    LOGD_EFER {
        if (fd != -1) {
            close(fd);
        }
    }

    if (fd == -1) {
        return false;
    } else {
        ftruncate(fd, capacity);
    }

    header_ = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return header_ != MAP_FAILED;
}

void MMapBuffer::Unmap_() {
    if (header_) {
        munmap(header_, capacity_);
    }

    header_ = nullptr;
}

void MMapBuffer::Sync_() {
    if (header_) {
        msync(header_, capacity_, MS_SYNC);
    }
}

} // namespace logger

