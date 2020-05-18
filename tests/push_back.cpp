#include <vector>
#include <iostream>
#include "../vector.hpp"
#include "timer.h"

void real (unsigned int sz) {
    std::vector<int> v;
    Timer T;
    int realloc = 0;
    T.set();
    for (int i = 1; i <= sz; ++i) {
        if (v.size() == v.capacity())
            realloc++;
        v.push_back(i);
    }
    std::cout << "std::vector\n";
    std::cout << "time: " << T.elapsed() << "s\n";
    std::cout << "reallocated: " << --realloc << " times\n\n";
}

void fake (unsigned int sz) {
    vector<int> v;
    Timer T;
    int realloc = 0;
    T.set();
    for (int i = 1; i <= sz; ++i) {
        if (v.size() == v.capacity()) 
            realloc++;
        v.push_back(i);
    }        
    std::cout << "vector\n";
    std::cout << "time: " << T.elapsed() << "s\n";
    std::cout << "reallocated: " << --realloc << " times";
}

int main() {
    unsigned int sz = 100000000;
    real(sz);
    fake(sz);
    return 0;
}