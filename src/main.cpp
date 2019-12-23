#include "allocator_test.cpp"

int main() {
    // Initialize all allocators.
    Allocator allocators[2] = {
        Allocator(Allocator::AllocationAlgorithm::FIRST_FIT),
        Allocator(Allocator::AllocationAlgorithm::NEXT_FIT),
        // Allocator(Allocator::AllocationAlgorithm::BEST_FIT),
    };

    // Run test for every allocator type.
    for (auto i = 0; i < 2; ++i) {
        TestPadding(allocators[i]);
        TestAlign(allocators[i]);
        TestAllocator_common_1(allocators[i]);
        TestAllocator_common_2(allocators[i]);
        TestAllocator_common_3(allocators[i]);
        TestAllocator_common_4(allocators[i]);
        TestAllocator_common_5(allocators[i]);
    }

    // Run tests only for next-fit type.
    TestAllocator_next_fit_1(allocators[1]);
    TestAllocator_next_fit_2(allocators[1]);

    return 0;
}
