#include "allocator_test.cpp"

int main() {
    // Create aliases for enum values.
    Allocator::AllocationAlgorithm algorithms[3] = {
        Allocator::AllocationAlgorithm::FIRST_FIT,
        Allocator::AllocationAlgorithm::NEXT_FIT,
        Allocator::AllocationAlgorithm::BEST_FIT,
    };

    // Run common tests for all allocator algorithms.
    for (auto i = 0; i < 3; ++i) {
        // Tests are running in different scope to check if we have any memory
        // errors in the allocator destructor.
        {
            auto allocator = Allocator(algorithms[i]);
            TestPadding(allocator);
            TestAlign(allocator);
        }
        {
            auto allocator = Allocator(algorithms[i]);
            TestAllocator_common_1(allocator);
        }
        {
            auto allocator = Allocator(algorithms[i]);
            TestAllocator_common_2(allocator);
        }
        {
            auto allocator = Allocator(algorithms[i]);
            TestAllocator_common_3(allocator);
        }
        {
            auto allocator = Allocator(algorithms[i]);
            TestAllocator_common_4(allocator);
        }
        {
            auto allocator = Allocator(algorithms[i]);
            TestAllocator_common_5(allocator);
        }
        {
            auto allocator = Allocator(algorithms[i]);
            TestAllocator_common_6(allocator);
        }
    }

    // Run the specific next-fit algorithm tests.
    {
        auto allocator = Allocator(Allocator::AllocationAlgorithm::NEXT_FIT);
        TestAllocator_next_fit_1(allocator);
    }
    {
        auto allocator = Allocator(Allocator::AllocationAlgorithm::NEXT_FIT);
        TestAllocator_next_fit_2(allocator);
    }

    // Run the specific best-fit algorithm tests.
    {
        auto allocator = Allocator(Allocator::AllocationAlgorithm::BEST_FIT);
        TestAllocator_best_fit_1(allocator);
    }

    return 0;
}
