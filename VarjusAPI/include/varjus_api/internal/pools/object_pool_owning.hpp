#pragma once

#include <vector>
#include <memory>

template <typename T>
class COwningObjectPool final {
public:
    COwningObjectPool() = default;
    explicit COwningObjectPool(std::size_t initialSize) {
        Grow(initialSize);
    }

    ~COwningObjectPool() {
        for (T* ptr : pool) {
            delete ptr;
        }
    }

    void Grow(std::size_t size) {
        pool.reserve(pool.size() + size);
        available.reserve(available.size() + size);

        for (size_t i = size_t{ 0 }; i < size; ++i) {
            T* obj = new T();
            pool.push_back(obj);
            available.push_back(obj);
        }
    }

    [[nodiscard]] T* Acquire() {
        if (available.empty()) {
            Grow(1);
        }
        T* obj = available.back();
        available.pop_back();
        return obj;
    }

    [[nodiscard]] VectorOf<T*> Acquire(std::size_t count) {
        if (available.size() < count) {
            Grow(count - available.size());
        }

        VectorOf<T*> result;
        result.reserve(count);

        for (size_t i = size_t{ 0 }; i < count; ++i) {
            result.push_back(available.back());
            available.pop_back();
        }

        return result;
    }

    void Release(T* obj) {
        available.push_back(obj);
    }

    [[nodiscard]] auto GetInUseCount() const noexcept {
        return pool.size() - available.size();
    }

    void ResetPool() {
        for (T* ptr : pool) {
            delete ptr;
        }
        pool.clear();
        available.clear();
    }

    [[nodiscard]] constexpr auto GetPoolSize() const noexcept {
        return pool.size();
    }

private:
    std::vector<T*> pool;       // owns objects
    std::vector<T*> available;  // stack of free objects
};
