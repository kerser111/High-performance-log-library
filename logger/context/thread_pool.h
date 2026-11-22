#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <future>
#include <memory>
#include <functional>
#include <cstdint>

namespace logger {
namespace ctx {

class ThreadPool {
public:
    explicit ThreadPool(std::uint32_t thread_count) {
        this->thread_count_.store(thread_count);
        this->is_shutdown_.store(false);
        this->is_available_.store(false);
    }

    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;

    ~ThreadPool() {
        Stop();
    }

    bool Start();

    void Stop();

    template <typename F, typename... Args>
    void SubmitTask(F&& f, Args&&... args) {
        if (this->is_shutdown_.load() || !this->is_available_.load()) {
            return;
        }

        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::lock_guard<std::mutex> lock(this->task_mutex_);
            this->tasks_.emplace([task]() {
                task();
            });
        }

        this->task_cv_.notify_all();
    }

    template <typename F, typename... Args>
    auto SubmitTaskWithResult(F&& f, Args&&... args) -> std::shared_ptr<std::future<std::result_of_t<F(Args...)>>> {
        if (this->is_shutdown_.load() || !this->is_available_.load()) {
            return nullptr;
        }

        using result_type = std::result_of_t<F(Args...)>;
        auto task = std::make_shared<std::packaged_task<result_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<result_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(task_mutex_);
            this->tasks_.emplace([task]() {
                (*task)();
            });
        }

        this->task_cv_.notify_all();
        return std::make_shared<std::future<result_type>>(std::move(res));
    }

private:
    void AddThread();

private:
    using ThreadPtr = std::shared_ptr<std::thread>;
    using Task = std::function<void()>;

    struct ThreadInfo {
        ThreadInfo() = default;
        ~ThreadInfo();

        ThreadPtr ptr{nullptr};
    };
    
    using ThreadInfoPtr = std::shared_ptr<ThreadInfo>;

private:
    std::vector<ThreadInfoPtr> worker_threads_;
    std::queue<Task> tasks_;

    std::mutex task_mutex_;
    std::condition_variable task_cv_;

    std::atomic<std::uint32_t> thread_count_;
    std::atomic<bool> is_shutdown_;
    std::atomic<bool> is_available_;
};

}
}