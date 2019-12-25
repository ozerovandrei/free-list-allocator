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

    size_t Padding(const size_t initial_size) const noexcept;
    size_t Align(const size_t initial_size) const noexcept;

    MachineWord *New(const size_t size) const noexcept;

    MemoryBlock *FindBlock(const size_t size) const noexcept;
    size_t AllocSizeWithBlock(const size_t size) const noexcept;
    MemoryBlock *NewFromOS(const size_t size) const noexcept;

    MemoryBlock *FirstFit(size_t size) const noexcept;
    MemoryBlock *NextFit(size_t size) const noexcept;
    MemoryBlock *BestFit(size_t size) const noexcept;

    void Free(MachineWord *data) const noexcept;
private:
    AllocationAlgorithm algorithm_;
};
