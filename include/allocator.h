#pragma once

#include <stdlib.h>
#include <string>
#include <mutex>

#include "block.h"

class Allocator {
public:
    mutable std::mutex _mtx;

    enum class AllocationAlgorithm {
        FIRST_FIT,
        NEXT_FIT,
        BEST_FIT
    };

    Allocator(AllocationAlgorithm algorithm) noexcept;
    ~Allocator() noexcept;

    std::string Algorithm() const noexcept;

    static size_t Align(size_t initial_size) noexcept;

    MachineWord *New(size_t size) noexcept;
    void Free(MachineWord *data) noexcept;

    // Disable move and copy semantics.
    Allocator(const Allocator&) = delete;
    Allocator(Allocator&&) = delete;
    Allocator& operator=(const Allocator&) = delete;
    Allocator& operator=(Allocator&&) = delete;
private:
    AllocationAlgorithm algorithm_;

    // heap_start contains pointer to the start of the heap and it is only updated
    // on the very first allocation.
    MemoryBlock *heap_start_;

    // heap_end points to the current end of the heap and it's updated on the new
    // allocation from the OS.
    MemoryBlock *heap_end_;

    // next_fit_start_block points to the block that should be used in the NextFit.
    MemoryBlock *next_fit_start_block_;

    static size_t AllocSizeWithBlock(size_t size) noexcept;

    MemoryBlock *FindBlock(size_t size) noexcept;

    static MemoryBlock *NewFromOS(size_t size) noexcept;

    static void SplitBlock(MemoryBlock *memory_block, size_t size) noexcept;
    void MergeBlocks(MemoryBlock *memory_block) noexcept;

    void ListAllocate(MemoryBlock *memory_block, size_t size) const noexcept;

    MemoryBlock *FirstFit(size_t size) const noexcept;
    MemoryBlock *NextFit(size_t size) noexcept;
    MemoryBlock *BestFit(size_t size) const noexcept;
};
