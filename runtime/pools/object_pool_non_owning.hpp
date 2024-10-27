#include <vector>
#include <memory>
#include <ranges>

template <typename T>
using VectorOf = std::vector<T>;


template <typename T>
class CNonOwningObjectPool {
public:
    CNonOwningObjectPool(std::size_t initialSize) {
        for ([[maybe_unused]]const auto i : std::views::iota(size_t(0), initialSize)) {
            pool.emplace_back(std::make_unique<T>());
        }
    }

    std::unique_ptr<T> acquire() {
        if (pool.empty()) {
            return std::make_unique<T>();
        }

        auto obj = std::move(pool.back());
        pool.pop_back();
        return obj;
    }

    VectorOf<std::unique_ptr<T>> acquire(std::size_t count) {

        VectorOf<std::unique_ptr<T>> result;

        for ([[maybe_unused]] const auto i : std::views::iota(size_t(0), count))
            result.emplace_back(acquire());

        return result;
    }

    void release(std::unique_ptr<T>&& obj) {
        pool.push_back(std::move(obj));
    }

private:
    VectorOf<std::unique_ptr<T>> pool;
};
