#pragma once

#include <iostream>
#include <chrono>

#include "allocator.cpp"

void AllocateCountTimes(Allocator& allocator, size_t size, unsigned int count) {
    auto operations = count;
    auto start = std::chrono::system_clock::now();

    // Allocate count times.
    while (operations > 0) {
        allocator.New(size);
        --operations;
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
        AllocateCountTimes(allocator, allocation_sizes[i], 1000);
    }

    std::cout << std::endl;
}

void AllocateFreeCountTimes(Allocator& allocator, size_t size, unsigned int count) {
    unsigned int operations = 0;
    auto start = std::chrono::system_clock::now();

    MachineWord *memory_blocks[count];

    // Allocate count times.
    while (operations < count) {
        memory_blocks[operations] = allocator.New(size);
        ++operations;
    }

    // Free count times.
    while (operations > 0) {
        allocator.Free(memory_blocks[operations]);
        --operations;
    }

    auto end = std::chrono::system_clock::now();

    std::cout << allocator.Algorithm() << ": "
    << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
    << "ns to allocate and then free " << size << " bytes " << count << " times"
    << std::endl;
}

void BenchmarkAllocateFree(Allocator& allocator) {
    size_t allocation_sizes[10] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 80};

    std::cout << "=== RUN BenchmarkAllocateFree for the "
    << allocator.Algorithm() << " algorithm" << std::endl;

    for (auto i = 0; i < 10; ++i) {
        AllocateFreeCountTimes(allocator, allocation_sizes[i], 1000);
    }

    std::cout << std::endl;
}
