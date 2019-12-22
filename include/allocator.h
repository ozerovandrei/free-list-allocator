#pragma once

#include <stdlib.h>

#include "block.h"

class Allocator {
public:
    enum AllocationAlgorithm {
        FIRST_FIT,
        NEXT_FIT,
        BEST_FIT
    };

    Allocator(const AllocationAlgorithm algorithm);

    size_t Padding(const size_t initial_size) const;
    size_t Align(const size_t initial_size) const;

    MachineWord *New(const size_t size) const;

    MemoryBlock *FindBlock(const size_t size) const;
    size_t AllocSizeWithBlock(const size_t size) const;
    MemoryBlock *NewFromOS(const size_t size) const;

    MemoryBlock *FirstFit(size_t size) const;
    MemoryBlock *NextFit(size_t size) const;
    MemoryBlock *BestFit(size_t size) const;

    void Free(MachineWord *data) const;
private:
    AllocationAlgorithm algorithm_;
};
