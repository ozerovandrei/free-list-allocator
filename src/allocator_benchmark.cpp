#pragma once

#include <iostream>
#include <chrono>

#include "allocator.cpp"

void Allocate(Allocator& allocator, size_t size, unsigned int count) {
    auto operaions = count;
    auto start = std::chrono::system_clock::now();

    while (operaions > 0) {
        allocator.New(size);
        --operaions;
    }

    auto end = std::chrono::system_clock::now();

    std::cout << allocator.Algorithm() << ": "
    << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
    << "ns to allocate " << size << " bytes " << count << " times"
    << std::endl;
}

void BenchmarkAllocate(Allocator& allocator) {
    size_t allocation_sizes[10] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 80};

    std::cout << "=== RUN BenchmarkAllocate for the "
    << allocator.Algorithm() << " algorithm" << std::endl;

    for (auto i = 0; i < 10; ++i) {
        Allocate(allocator, allocation_sizes[i], 1000);
    }

    std::cout << std::endl;
}
