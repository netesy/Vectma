#include "core/LinearAllocator.hpp"
#include <algorithm>
#include <stdexcept>

namespace vectma {

LinearAllocator::LinearAllocator(size_t capacity)
    : m_capacity(capacity), m_offset(0) {
    m_buffer = new char[m_capacity];
}

LinearAllocator::~LinearAllocator() {
    delete[] m_buffer;
}

void* LinearAllocator::allocateBytes(size_t bytes, size_t alignment) {
    size_t currentAddress = reinterpret_cast<size_t>(m_buffer + m_offset);
    size_t padding = (alignment - (currentAddress % alignment)) % alignment;

    if (m_offset + padding + bytes > m_capacity) {
        throw std::bad_alloc();
    }

    m_offset += padding;
    void* ptr = m_buffer + m_offset;
    m_offset += bytes;
    return ptr;
}

void LinearAllocator::reset() {
    m_offset = 0;
}

LinearAllocator& LinearAllocator::getThreadLocal() {
    thread_local LinearAllocator instance;
    return instance;
}

} // namespace vectma
