#include <iostream>
#include "skiplist.h"

int main() {
    skiplist<int, 4> sklist;
    for (int i = 0; i < 100; i++) {
        sklist.insert(i);
        sklist.insert(i);
    }
    for (auto elem : sklist) {
        std::cout << elem << std::endl;
    }

    auto it = sklist.find(50);
    while (it != sklist.end()) {
        std::cout << *it << std::endl;
        ++it;
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}