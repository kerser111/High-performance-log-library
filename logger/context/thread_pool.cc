#include "context/thread_pool.h"

namespace logger {
namespace ctx {

ThreadPool::ThreadInfo::~ThreadInfo() {
    if (ptr && ptr->joinable()) {
        ptr->join();
    }
}

void ThreadPool::AddThread() {
    ThreadInfoPtr thread_info_ptr = std::make_shared<ThreadInfo>();
    auto func = [this]() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(task_mutex_);
                this->task_cv_.wait(lock, [this]() {
                    return this->is_shutdown_.load() || !this->tasks_.empty();
                });

                if (this->is_shutdown_.load()) {
                    break;
                }

                task = std::move(this->tasks_.front());
                this->tasks_.pop();
            }

            task();          
        }
    };

    thread_info_ptr->ptr = std::make_shared<std::thread>(std::move(func));
    this->worker_threads_.emplace_back(std::move(thread_info_ptr));
}

void ThreadPool::Stop() {
    if (this->is_available_.load()) {
        this->is_shutdown_.store(true);
        this->task_cv_.notify_all();
        this->is_available_.store(false);
    }

    this->worker_threads_.clear();
}

bool ThreadPool::Start() {
    if (this->is_available_.load()) {
        return false;
    }

    this->is_available_.store(true);
    this->is_shutdown_.store(false);
    std::uint32_t thread_count = this->thread_count_.load();

    for (std::uint32_t i = 0; i < thread_count; ++i) {
        AddThread();
    }

    return true;
}

}
}