#include "allocator_test.cpp"

int main() {
    // Initialize all allocators.
    Allocator allocators[1] = {
        Allocator(Allocator::AllocationAlgorithm::FIRST_FIT),
        // Allocator(Allocator::AllocationAlgorithm::NEXT_FIT),
        // Allocator(Allocator::AllocationAlgorithm::BEST_FIT),
    };

    // Run test for every allocator type.
    for (auto i = 0; i < 1; ++i) {
        TestPadding(allocators[i]);
        TestAlign(allocators[i]);
        TestAllocator_1(allocators[i]);
        TestAllocator_2(allocators[i]);
        TestAllocator_3(allocators[i]);
        TestAllocator_4(allocators[i]);
        TestAllocator_5(allocators[i]);
    }

    return 0;
}
