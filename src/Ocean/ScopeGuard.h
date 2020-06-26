#pragma once

class ScopeGuard {
public:
    ScopeGuard() = delete; // no guards without a function
    ScopeGuard(std::function<void()> f_);
    ~ScopeGuard();
private:
    std::function<void()> f;
};
