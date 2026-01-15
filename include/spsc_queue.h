#pragma once

// #include "shm_header.h"
#include "shm_segment.h"
#include <bit>

template <typename T>
class SPSCQueue
{
public:
    void init(SHMSegment* segment, std::size_t capacity)
    {
        std::size_t adjusted = std::bit_ceil(capacity);

        header_ = new (segment->get_address()) SharedHeader{0, 0xC0FFEE, 0, adjusted, sizeof(T), 0};
        
        buffer_ = reinterpret_cast<T*>(header_ + 1);
    }

    void attach(SHMSegment* segment)
    {
        header_ = reinterpret_cast<SharedHeader*>(segment->get_address());

        if (!header_ || header_->magic != 0xC0FFEE)
        {
            throw std::runtime_error("Queue uninitialized.");
        }

        buffer_ = reinterpret_cast<T*>(header_ + 1);
    }

    bool push(const T& val)
    {
        auto rIdx = header_->read_index.load(std::memory_order_acquire);
        auto wIdx = header_->write_index.load(std::memory_order_acquire);
        auto cap = header_->capacity;

        if (wIdx - rIdx == cap)
        {
            return false; // at capacity
        }

        buffer_[wIdx & (cap - 1)] = val;

        header_->write_index.store(++wIdx, std::memory_order_release);

        return true;
    }

    bool pop(T& val)
    {
        auto rIdx = header_->read_index.load(std::memory_order_acquire);
        auto wIdx = header_->write_index.load(std::memory_order_acquire);
        auto cap = header_->capacity;

        if (rIdx == wIdx)
        {
            return false;
        }

        val = buffer_[rIdx & (cap - 1)];

        header_->read_index.store(++rIdx, std::memory_order_release);

        return true;
    }

private:
    SharedHeader* header_{ nullptr };  // bytes 0 - 64
    T* buffer_{ nullptr };             // bytes 64 + 
};
