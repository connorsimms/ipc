#include "spsc_queue.h"

#include <iostream>
#include <ios>

int main()
{
    SHMSegment segment{};
    segment.attach("test_shm");

    SPSCQueue<int> queue{};
    queue.attach(&segment);

    int x;
    while (queue.pop(x))
    {
        std::cout << "Popped " << x << '\n';
    }
    std::cout << "Queue empty.\n";

    return 0;
}
