#pragma once

#include <vector>
#include <memory>
#include <stack>
#include <ranges>
#include <cassert>

template <typename T>
class COwningObjectPool {
public:

    COwningObjectPool(std::size_t initialSize) {
        for ([[maybe_unused]] const auto i : std::views::iota(size_t(0), initialSize)) {
            pool.emplace_back(std::make_unique<T>());
            available.push(pool.back().get());
        }
    }

    [[nodiscard]] T* Acquire() {
        if (available.empty()) {
            pool.emplace_back(std::make_unique<T>());
            available.push(pool.back().get());
        }
        T* obj = available.top();
        available.pop();
        return obj;
    }

    [[nodiscard]] VectorOf<T*> Acquire(std::size_t count) {

        VectorOf<T*> result;
        result.reserve(count);

        for ([[maybe_unused]] const auto i : std::views::iota(size_t(0), count))
            result.emplace_back(Acquire());

        return result;
    }

    void Release(T* obj) {
        available.push(obj);
        assert(pool.size() >= available.size());
    }

    [[nodiscard]] auto GetInUseCount() const noexcept {
        assert(pool.size() >= available.size());
        return pool.size() - available.size();
    }

    void ResetPool()
    {
        while (available.size())
            available.pop();

        pool.clear();
    }

private:
    std::vector<std::unique_ptr<T>> pool;
    std::stack<T*> available;
};
