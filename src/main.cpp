#include "allocator_test.cpp"

int main() {
    auto allocator = Allocator(Allocator::AllocationAlgorithm::FIRST_FIT);

    TestPadding(allocator);
    TestAlign(allocator);

    return 0;
}
