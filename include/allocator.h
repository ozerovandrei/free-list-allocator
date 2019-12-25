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
    MemoryBlock *FindBlock(size_t size) noexcept;

    static MemoryBlock *NewFromOS(size_t size) noexcept;

    MemoryBlock *FirstFit(size_t size) const noexcept;
    MemoryBlock *NextFit(size_t size) noexcept;
    MemoryBlock *BestFit(size_t size) const noexcept;

    void Free(MachineWord *data) const noexcept;
private:
    AllocationAlgorithm algorithm_;

    // heap_start contains pointer to the start of the heap and it is only updated
    // on the very first allocation.
    MemoryBlock *heap_start_ = nullptr;

    // heap_end points to the current end of the heap and it's updated on the new
    // allocation from the OS.
    MemoryBlock *heap_end_ = heap_start_;

    // next_fit_start_block points to the block that should be used in the NextFit.
    MemoryBlock *next_fit_start_block_ = heap_start_;
};
