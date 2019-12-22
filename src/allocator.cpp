#pragma once

#include <unistd.h>

#include "../include/allocator.h"

// machine_word represents a size to which all allocations should be aligned.
using machine_word = uintptr_t;

// Allocator constructor.
Allocator::Allocator(const AllocationAlgorithm algorithm) {
    algorithm_ = algorithm;
}

// Padding calculates the size that is needed to align the provided initial
// size with the machine word.
size_t Allocator::Padding(const size_t initial_size) const {
    auto alignment = sizeof(machine_word);

    size_t multiplier = (initial_size / alignment) + 1;
    size_t aligned_size = multiplier * alignment;

    return aligned_size - initial_size;
}

// Align performs an alignment of the initial size to the machine word size.
// Word size is usually 8 bytes on a 64 bit architecture.
// Examples:
//  - Align(3) -> 8
//  - Align(8) -> 8
//  - Align(9) -> 16
size_t Allocator::Align(const size_t initial_size) const {
    auto alignment = sizeof(machine_word);

    // If initial size is less than machine word alignment just return the
    // machine word alignment.
    if (initial_size < alignment) {
        return alignment;
    }

    // Calculate padding if initial size is bigger than machine word alignment.
    auto padding = Allocator::Padding(initial_size);

    // Padding equals to initial size - we don't need to align it.
    if (padding == initial_size) {
        return initial_size;
    }

    // Padding is less than initial size - we need to align the initial size
    // to the machine word.
    if (padding < initial_size) {
        auto difference = initial_size - padding;
        auto alignment = sizeof(machine_word);

        if (difference % alignment > 0) {
            return padding + initial_size;
        }

        return alignment + alignment * (difference / alignment);
    }

    // Padding is bigger than initial size - just return their sum.
    return initial_size + padding;
}
