#include <iostream>
#include "skiplist.h"

int main() {
    skiplist<int, 4> sklist;
    sklist.insert(0);
    sklist.find(0);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}