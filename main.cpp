#include <iostream>
#include <random>
#include <set>
#include "skiplist.h"

std::random_device rd;
std::mt19937 gen(rd());

int main() {
    std::uniform_int_distribution<> d;
    skiplist<int, 16> s;
//    std::set<int> s;
    for (int i = 0; i < 1 << 16; i++) {
        s.insert(d(gen));
    }
    return 0;
}
