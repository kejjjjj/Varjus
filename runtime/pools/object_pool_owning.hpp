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

    T* Acquire() {
        if (available.empty()) {
            pool.emplace_back(std::make_unique<T>());
            available.push(pool.back().get());
        }
        T* obj = available.top();
        available.pop();
        return obj;
    }

    void Release(T* obj) {
        available.push(obj);
        assert(pool.size() >= available.size());
    }

    auto GetInUseCount() const {
        assert(pool.size() >= available.size());
        return pool.size() - available.size();
    }

private:
    std::vector<std::unique_ptr<T>> pool;
    std::stack<T*> available;
};
