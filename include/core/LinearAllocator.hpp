#pragma once

#include <cstddef>
#include <utility>
#include <new>

namespace vectma {

class LinearAllocator {
public:
    LinearAllocator(size_t capacity = 2 * 1024 * 1024); // 2MB default
    ~LinearAllocator();

    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;

    void* allocateBytes(size_t bytes, size_t alignment = alignof(std::max_align_t));

    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        void* ptr = allocateBytes(sizeof(T), alignof(T));
        return ::new (ptr) T(std::forward<Args>(args)...);
    }

    template<typename T>
    T* allocateArray(size_t count) {
        if (count == 0) return nullptr;
        void* ptr = allocateBytes(sizeof(T) * count, alignof(T));
        T* arrayPtr = static_cast<T*>(ptr);
        for (size_t i = 0; i < count; ++i) {
            ::new (&arrayPtr[i]) T();
        }
        return arrayPtr;
    }

    void reset();
    size_t getUsedBytes() const { return m_offset; }
    size_t getCapacity() const { return m_capacity; }

    static LinearAllocator& getThreadLocal();

private:
    char* m_buffer = nullptr;
    size_t m_capacity = 0;
    size_t m_offset = 0;
};

} // namespace vectma
