#pragma once

#include <functional>

namespace logger {

class ExecuteonScopeExit {
public:
    ExecuteonScopeExit() = default;

    ExecuteonScopeExit(ExecuteonScopeExit&&) noexcept = default;

    ExecuteonScopeExit& operator=(ExecuteonScopeExit&&) noexcept = default;

    ExecuteonScopeExit(const ExecuteonScopeExit&) = delete;

    ExecuteonScopeExit& operator=(const ExecuteonScopeExit&) = delete;

    template <typename F, typename... Args>
    ExecuteonScopeExit(F&& f, Args&&... args) {
        func_ = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    }

    ~ExecuteonScopeExit() noexcept {
        if (func_) {
            func_();
        }
    }

private:
    std::function<void()> func_;
}

}

#define _LOG_CONCAT(a, b) a##b
#define _MAKE_DEFER_(line) logger::ExecuteonScopeExit _LOG_CONCAT(defer, line) = [&]()
#undef LOG_DEFER
#define LOG_DEFER _MAKE_DEFER_(__LINE__)