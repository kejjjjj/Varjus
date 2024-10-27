#include <vector>
#include <memory>
#include <stack>
#include <ranges>

template <typename T>
class COwningObjectPool {
public:

    COwningObjectPool(std::size_t initialSize) {
        for ([[maybe_unused]] const auto i : std::views::iota(size_t(0), initialSize)) {
            pool.emplace_back(std::make_unique<T>());
            available.push(pool.back().get());
        }
    }

    T* acquire() {
        if (available.empty()) {
            pool.emplace_back(std::make_unique<T>());
            available.push(pool.back().get());
        }
        T* obj = available.top();
        available.pop();
        return obj;
    }

    void release(T* obj) {
        available.push(obj);
    }

private:
    std::vector<std::unique_ptr<T>> pool;
    std::stack<T*> available;
};
