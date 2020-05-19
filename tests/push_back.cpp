#include <vector>
#include <iostream>
#include "../vector.hpp"
#include "timer.h"

void measure (unsigned int sz, bool std) {
    std::vector<int> real;
    vector<int> fake;
    Timer T;
    int reallocCount = 0, capacity;
    bool didReallocate = false;

    std::cout << (std ? "std::vector\n" : "vector\n");

    T.set();
    if (std) {
        for (int i = 1; i <= sz; ++i) {
            if (real.size() == real.capacity()) {
                reallocCount++; 
                didReallocate = true;
            }
            real.push_back(i);
            if (didReallocate) {
                capacity = real.capacity();
                didReallocate = false;
            }
        }
    } else {
        for (int i = 1; i <= sz; ++i) {
        if (fake.size() == fake.capacity()) {
            reallocCount++; 
            didReallocate = true;
        }
        fake.push_back(i);
        if (didReallocate) {
            capacity = fake.capacity();
            didReallocate = false;
        }
    }
    }
    std::cout << "time: " << T.elapsed() << "s\n";
    std::cout << "reallocated: " << reallocCount << " times\n";
    std::cout << "reached capacity: " << capacity << "\n\n";
}

int main() {
    unsigned int sz = 100000000;
    measure (sz, true);
    measure (sz, false);
    return 0;
}