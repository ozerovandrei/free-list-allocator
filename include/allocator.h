#pragma once

#include <stdlib.h>

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

private:
    AllocationAlgorithm algorithm_;
};
