#include "spsc_queue.h"

#include <iostream>

int main()
{
    SHMSegment segment{};
    segment.create("test_shm", 1024);

    SPSCQueue<Trade> queue{};
    queue.init(&segment, 10u);

    
    std::cout << "Offset info: ";
    std::cout << offsetof(SharedHeader, write_index) << '\n';
    std::cout << offsetof(SharedHeader, magic) << '\n';
    std::cout << offsetof(SharedHeader, version) << '\n';
    std::cout << offsetof(SharedHeader, capacity) << '\n';
    std::cout << offsetof(SharedHeader, element_size) << '\n';
    std::cout << offsetof(SharedHeader, schema) << '\n';
    std::cout << offsetof(SharedHeader, read_index) << '\n';

    std::cout << sizeof(SharedHeader) << '\n';

    return 0;
}
