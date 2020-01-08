#pragma once

// sbrk is deprecated on MacOS clang++ so we need to add those lines.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <unistd.h>

#include "block.cpp"
#include "../include/allocator.h"

// Allocator constructor.
Allocator::Allocator(AllocationAlgorithm algorithm) noexcept :
algorithm_(algorithm),
heap_start_(nullptr),
heap_end_(heap_start_),
last_allocated_block_(heap_start_),
next_fit_start_block_(heap_start_) {}

// Allocator destructor.
Allocator::~Allocator() noexcept {
    if (heap_start_ == nullptr) {
        return;
    }

    // Reset the current allocation via brk: https://linux.die.net/man/2/brk
    // https://stackoverflow.com/questions/6988487/what-does-the-brk-system-call-do
    brk(heap_start_);
}

// Return algorithm type.
std::string Allocator::Algorithm() const noexcept {
    switch (algorithm_) {
        case AllocationAlgorithm::FIRST_FIT:
            return "first fit";
        case AllocationAlgorithm::NEXT_FIT:
            return "next fit";
        case AllocationAlgorithm::BEST_FIT:
            return "best fit";
    }
}

// Align performs an alignment of the initial size to the machine word size.
// Word size is usually 8 bytes on a 64 bit architecture.
// Examples:
//  - Align(3) -> 8
//  - Align(8) -> 8
//  - Align(9) -> 16
size_t Allocator::Align(size_t initial_size) noexcept {
    // Return 0 for 0.
    if (initial_size == 0) {
        return 0;
    }

    auto alignment = sizeof(MachineWord);

    // If initial size is less than machine word alignment just return the
    // machine word alignment.
    if (initial_size < alignment) {
        return alignment;
    }

    // Calculate padding if initial size is bigger than machine word alignment.
    size_t multiplier = (initial_size / alignment) + 1;
    size_t aligned_size = multiplier * alignment;
    size_t padding = aligned_size - initial_size;

    // Padding equals to initial size - we don't need to align it.
    if (padding == initial_size) {
        return initial_size;
    }

    // Padding is less than initial size - we need to align the initial size
    // to the machine word.
    if (padding < initial_size) {
        auto difference = initial_size - padding;

        if (difference % alignment > 0) {
            return padding + initial_size;
        }

        return alignment + alignment * (difference / alignment);
    }

    // Padding is bigger than initial size - just return their sum.
    return initial_size + padding;
}

// New allocates new block of memory from OS of at least needed_size bytes.
MachineWord *Allocator::New(size_t needed_size) noexcept {
    // Lock mutex.
    std::lock_guard<std::mutex> lock(_mtx);

    auto size = Allocator::Align(needed_size);
    MemoryBlock *memory_block;

    // Search for the needed size of a block in the free-list.
    memory_block = Allocator::FindBlock(size);
    if (memory_block) {
        return memory_block->Data;
    }

    // Allocate a new block if we can't find a block in the free-list.
    memory_block = Allocator::NewFromOS(size);
    memory_block->Size = size;
    memory_block->Used = true;

    // Update information about heap start if it's a new allocation.
    if (heap_start_ == nullptr) {
        heap_start_ = memory_block;
    }

    // Update information about heap end.
    if (heap_end_ != nullptr) {
        heap_end_->Next = memory_block;
    }

    // Chain blocks.
    heap_end_ = memory_block;

    // Return new data pointer.
    return memory_block->Data;
}

// AllocSizeWithBlock returns allocation size plus MemoryBlock header and first
// Data element.
// We remove size of the Data field since user can allocate one word.
size_t Allocator::AllocSizeWithBlock(size_t size) noexcept {
    return sizeof(MemoryBlock) + size - SizeOfData();
}

// NewFromOS allocates new block from OS or returns a nullptr if a new block
// can't be allocated (memory error).
MemoryBlock *Allocator::NewFromOS(size_t size) noexcept {
    // Get the current heap end via sbrk: https://linux.die.net/man/2/sbrk
    // https://stackoverflow.com/questions/6988487/what-does-the-brk-system-call-do
    auto memory_block = (MemoryBlock *)sbrk(0);

    // Memory error.
    if (sbrk(AllocSizeWithBlock(size)) == (void *)-1) {
        return nullptr;
    }

    return memory_block;
}

// SplitBlock splits a big block of memory to retrieve smaller block of the
// needed size.
MemoryBlock *Allocator::SplitBlock(MemoryBlock *memory_block, size_t size) noexcept {
    // Block that is left after splitting.
    auto left_part = (MemoryBlock *)((char *)memory_block + AllocSizeWithBlock(size));
    left_part->Size = memory_block->Size - size;
    left_part->Used = false;
    left_part->Next = memory_block->Next;

    // Update current block and chain left part and block.
    memory_block->Size = size;
    memory_block->Next = left_part;

    return memory_block;
}

// MergeBlocks merges the selected block with the next one.
MemoryBlock *Allocator::MergeBlocks(MemoryBlock *memory_block) noexcept {
    // Next block could be last allocated block - update that pointer.
    if (memory_block->Next == last_allocated_block_) {
        last_allocated_block_ = memory_block;
    }

    // Merge blocks.
    memory_block->Size += memory_block->Next->Size;
    memory_block->Next = memory_block->Next->Next;

    return memory_block;
}

// FindBlock searches for the next free block that can be used.
// It uses different algorithm based on selected algorithm of the allocator.
MemoryBlock *Allocator::FindBlock(size_t size) noexcept {
    switch (algorithm_) {
        case AllocationAlgorithm::FIRST_FIT:
            return FirstFit(size);
        case AllocationAlgorithm::NEXT_FIT:
            return NextFit(size);
        case AllocationAlgorithm::BEST_FIT:
            return BestFit(size);
    }
}

// ListAllocate implements common block allocation function.
// It will try to split a free block if it's bigger than provided size.
MemoryBlock *Allocator::ListAllocate(MemoryBlock *memory_block, size_t size) const noexcept {
    // We can't split block if requested size is smaller than allocated size for
    // a new block.
    if (sizeof(MemoryBlock) <= (AllocSizeWithBlock(memory_block->Size) - size)) {
        memory_block = SplitBlock(memory_block, size);
    }

    // Block is allocated and ready to use.
    memory_block->Used = true;
    memory_block->Size = size;

    return memory_block;
}

/*
FirstFit will use the first cell it finds that can satisfy the request.

Pseudo-code:

firstFitAllocate(n):
    prev <- addressOf(head)
    loop
        curr <- next(prev)
        if curr == null
            return null
        else if size(curr) < n
            prev <- curr
        else
            return listAllocate(prev, curr, n)

listAllocate(prev, curr, n):
    result <- curr
    if shouldSplit(size(curr), n)
        remainder <- result + n
        next(remainder) <- next(curr)
        size(remainder) <- size(curr) - n
        next(prev) <- next(curr)
    else
        next(prev) <- next(curr)
    return result
*/
MemoryBlock *Allocator::FirstFit(size_t size) const noexcept {
    auto memory_block = heap_start_;

    while (memory_block != nullptr) {
        if (memory_block->Used || memory_block->Size < size) {
            memory_block = memory_block->Next;

            // Check the next block.
            continue;
        }

        // Found the needed block.
        return ListAllocate(memory_block, size);
    }

    return nullptr;
}

/*
NextFit is a variation of first-fit that starts the search for a cell of
suitable size from the point in the list where the last search succeeded.

Pseudo-code:

nextFitAllocate(n):
    start <- prev
    loop
        curr <- next(prev)
        if curr == null
            prev <- addressOf(head)
            curr <- next
        if prev = start
            return null
        else if size(curr) < n
            prev <- curr
        else
            return listAllocate(prev, curr, n)
*/
MemoryBlock *Allocator::NextFit(size_t size) noexcept {
    // Reset start block to start of the heap if it's empty.
    if (next_fit_start_block_ == nullptr) {
        next_fit_start_block_ = heap_start_;
    }

    // Initial start block to check for cycles.
    auto initial_start_block = next_fit_start_block_;

    // Result memory block.
    auto memory_block = initial_start_block;

    while (memory_block != nullptr) {
        if (memory_block->Used || memory_block->Size < size) {
            memory_block = memory_block->Next;

            // Return to the begining of the list since we use circular first-fit
            // allocation.
            if (memory_block == nullptr) {
                memory_block = heap_start_;
            }

            // Initial start block and current block are equal => cycle.
            if (memory_block == initial_start_block) {
                break;
            }

            // Check the next block.
            continue;
        }

        // Found the needed block.
        next_fit_start_block_ = memory_block;
        return ListAllocate(memory_block, size);
    }

    return nullptr;
}

/*
Best-fit allocation finds the cell whose size most closely matches the request.
The idea is to minimise waste, as well as to avoid splitting large cells
unnecessarily.

Pseudo-code:

bestFitAllocate(n):
    best <- null
    bestSize <- INFINITY
    prev <- addressOf(head)
    loop
        curr <- next(prev)
        if curr == null || size(curr) = n
            if curr != null
                bestPrev <- prev
                best <- curr
            else if best = null
                return null
            return listAllocate(bestPrev, best, n)
        else if size(curr) < n || bestSize < size(curr)
            prev <- curr
        else
            best <- curr
            bestPrev <- prev
            bestSize <- size(curr)
*/
MemoryBlock *Allocator::BestFit(size_t size) const noexcept {
    auto memory_block = heap_start_;
    MemoryBlock *best_block = nullptr;

    while (memory_block != nullptr) {
        if (memory_block->Used || memory_block->Size < size) {
            memory_block = memory_block->Next;
            continue;
        }

        // Found a block with smaller size than best_block.
        if (best_block == nullptr || memory_block->Size < best_block->Size) {
            best_block = memory_block;
        }

        memory_block = memory_block->Next;
    }

    // Memory error.
    if (best_block == nullptr) {
        return nullptr;
    }

    // Found the needed block.
    return ListAllocate(best_block, size);
}

// Free deallocates previously created MemoryBlock.
void Allocator::Free(MachineWord *data) noexcept {
    // Lock mutex.
    std::lock_guard<std::mutex> lock(_mtx);

    auto memory_block = GetHeader(data);

    // Merge the found block with the next one if next block is exist and it's
    // not used.
    if (memory_block->Next && !memory_block->Next->Used) {
        memory_block = MergeBlocks(memory_block);
    }

    memory_block->Used = false;
}
