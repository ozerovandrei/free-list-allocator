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

    Allocator(AllocationAlgorithm algorithm);

    static size_t Padding(size_t initial_size) noexcept;
    static size_t Align(size_t initial_size) noexcept;

    static size_t AllocSizeWithBlock(size_t size) noexcept;

    MachineWord *New(size_t size) noexcept;
    MemoryBlock *FindBlock(size_t size) const noexcept;

    static MemoryBlock *NewFromOS(size_t size) noexcept;

    MemoryBlock *FirstFit(size_t size) const noexcept;
    MemoryBlock *NextFit(size_t size) const noexcept;
    MemoryBlock *BestFit(size_t size) const noexcept;

    void Free(MachineWord *data) const noexcept;
private:
    AllocationAlgorithm algorithm_;
};
