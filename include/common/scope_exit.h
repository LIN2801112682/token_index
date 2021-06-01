#include <functional>

class ScopeExit
{
public:
    ScopeExit() = default;

    ScopeExit(const ScopeExit &) = delete;
    void operator=(const ScopeExit &) = delete;

    ScopeExit(ScopeExit &&) = default;
    ScopeExit &operator=(ScopeExit &&) = default;

    template <typename F, typename... Args>
    ScopeExit(F &&f, Args &&...args)
    {
        func_ = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    }

    ~ScopeExit()
    {
        if (func_)
            func_();
    };

private:
    std::function<void()> func_;
};

#define _CONCAT(a, b) a##b
#define _MAKE_SCOPE_(line) ScopeExit _CONCAT(defer, line) = [&]()

#undef SCOPE_GUARD
#define SCOPE_GUARD _MAKE_SCOPE_(__LINE__)