#include "spsc_queue.h"

#include <iostream>

int main()
{
    SHMSegment segment{};
    segment.create("test_shm", 1024);

    SPSCQueue<int> queue{};
    queue.init(&segment, 10u);

    int x;
    while (std::cin >> x && x != -1)
    {
        if (queue.push(x))
            std::cout << "Pushed " << x << '\n';
        else
            std::cout << "Queue full.\n";
    }

    return 0;
}
