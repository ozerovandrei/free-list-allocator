#pragma once

#include <iostream>

#include "allocator.cpp"

void PrintTestRunning(const std::string& test_name, const Allocator& allocator) {
    std::cout << "=== RUN " << test_name << " for the "
    << allocator.Algorithm() << " algorithm" << std::endl;
}

void PrintTestPass(const std::string& test_name) {
    std::cout << "--- PASS: " << test_name << std::endl;
}

void PrintTestFail(const std::string& test_name) {
    std::cerr << "--- FAIL: " << test_name << std::endl;
}

void TestAlign(const Allocator& allocator) {
    std::string test_name = "TestAlign";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    size_t initial_sizes[35] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 21, 22, 23, 24, 32, 36, 40, 42, 47, 48, 60, 62, 63, 64, 65,
        100, 101, 104};

    size_t expected_alignment[35] = {0, 8, 8, 8, 8, 8, 8, 8, 8, 16, 16, 16, 16,
        16, 16, 16, 16, 24, 24, 24, 24, 32, 40, 40, 48, 48, 48, 64, 64, 64, 64,
        72, 104, 104, 104};

    for (auto i = 0; i < 35; ++i) {
        auto actual = allocator.Align(initial_sizes[i]);
        if (expected_alignment[i] != actual) {
            fail = true;
            PrintTestFail(test_name);
            std::cerr << "Expected " << expected_alignment[i]
            << " from Align(" << initial_sizes[i] << "), but got: "
            << actual << std::endl;
        }
    }

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void AssertUsedBlock(const MemoryBlock* data, bool& fail_flag, const std::string& test_name) {
    if (!data->Used) {
        fail_flag = true;
        PrintTestFail(test_name);
        std::cerr << "Expected used block but it is free" << std::endl;
    }
}

void AssertFreeBlock(const MemoryBlock* data, bool& fail_flag, const std::string& test_name) {
    if (data->Used) {
        fail_flag = true;
        PrintTestFail(test_name);
        std::cerr << "Expected free block but it is in use" << std::endl;
    }
}

void AssertAllocatedSize(const MemoryBlock* data, size_t expected_size, bool& fail_flag, const std::string& test_name) {
    auto actual_size = data->Size;

    if (actual_size != expected_size) {
        fail_flag = true;
        PrintTestFail(test_name);
        std::cerr << "Expected " << expected_size
        << ", but got: " << actual_size << std::endl;
    }
}

void AssertBlocksEqual(const MemoryBlock* a, const MemoryBlock* b, bool& fail_flag, const std::string& test_name) {
    if (a != b) {
        fail_flag = true;
        PrintTestFail(test_name);
        std::cerr << "Memory blocks aren't equal" << std::endl;
    }
}

void TestAllocator_common_1(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_1";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block = allocator.New(3); // 8
    auto block_header = GetHeader(block);

    AssertUsedBlock(block_header, fail, test_name);
    AssertAllocatedSize(block_header, sizeof(MachineWord), fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_common_2(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_2";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block = allocator.New(8);
    auto block_header = GetHeader(block);

    AssertUsedBlock(block_header, fail, test_name);
    AssertAllocatedSize(block_header, 8, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_common_3(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_3";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block = allocator.New(12); // 16
    auto block_header = GetHeader(block);

    AssertUsedBlock(block_header, fail, test_name);
    AssertAllocatedSize(block_header, 16, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_common_4(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_4";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block = allocator.New(12); // 16
    auto block_header = GetHeader(block);
    allocator.Free(block);

    AssertFreeBlock(block_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_common_5(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_5";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block_1 = allocator.New(12); // 16
    auto block_2 = allocator.New(6);  // 8
    auto block_1_header = GetHeader(block_1);
    auto block_2_header = GetHeader(block_2);

    AssertUsedBlock(block_1_header, fail, test_name);
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertAllocatedSize(block_1_header, 16, fail, test_name);
    AssertAllocatedSize(block_2_header, 8, fail, test_name);

    allocator.Free(block_2);
    AssertFreeBlock(block_2_header, fail, test_name);

    auto block_3 = allocator.New(8);
    auto block_3_header = GetHeader(block_3);
    AssertAllocatedSize(block_3_header, 8, fail, test_name);

    // Check that block 2 is reused.
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertUsedBlock(block_3_header, fail, test_name);
    AssertBlocksEqual(block_2_header, block_3_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_common_6(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_6";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block_1 = allocator.New(6);  // 8
    auto block_2 = allocator.New(60); // 64
    auto block_3 = allocator.New(60); // 64
    auto block_1_header = GetHeader(block_1);
    auto block_2_header = GetHeader(block_2);
    auto block_3_header = GetHeader(block_3);

    AssertUsedBlock(block_1_header, fail, test_name);
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertUsedBlock(block_3_header, fail, test_name);
    AssertAllocatedSize(block_1_header, 8, fail, test_name);
    AssertAllocatedSize(block_2_header, 64, fail, test_name);
    AssertAllocatedSize(block_3_header, 64, fail, test_name);

    // Free big blocks.
    allocator.Free(block_2);
    allocator.Free(block_3);
    AssertFreeBlock(block_2_header, fail, test_name);
    AssertFreeBlock(block_3_header, fail, test_name);

    // Allocate two smaller blocks and check that block_2 is reused and
    // block_3 is not.
    auto block_4 = allocator.New(31); // 32
    auto block_5 = allocator.New(30); // 32
    auto block_4_header = GetHeader(block_4);
    auto block_5_header = GetHeader(block_5);

    AssertAllocatedSize(block_4_header, 32, fail, test_name);
    AssertAllocatedSize(block_5_header, 32, fail, test_name);

    AssertUsedBlock(block_2_header, fail, test_name);
    AssertFreeBlock(block_3_header, fail, test_name);
    AssertBlocksEqual(block_2_header, block_4_header, fail, test_name);

    // Allocate additional smaller block and check that it reused the block_3
    // place.
    auto block_6 = allocator.New(28); // 32
    auto block_6_header = GetHeader(block_6);

    AssertAllocatedSize(block_6_header, 32, fail, test_name);

    AssertUsedBlock(block_3_header, fail, test_name);
    AssertBlocksEqual(block_3_header, block_6_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_common_7(Allocator& allocator) {
    std::string test_name = "TestAllocator_common_7";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    int *i;
    unsigned long *j;
    signed long *k;
    char *c;
    bool *b;

    auto i_block = allocator.New(sizeof(int));
    auto j_block = allocator.New(sizeof(unsigned long));
    auto k_block = allocator.New(sizeof(unsigned long long));
    auto c_block = allocator.New(sizeof(char));
    auto b_block = allocator.New(sizeof(bool));

    i = (int *)GetHeader(i_block);
    j = (unsigned long *)GetHeader(j_block);
    k = (signed long *)GetHeader(k_block);
    c = (char *)GetHeader(c_block);
    b = (bool *)GetHeader(b_block);

    AssertUsedBlock(GetHeader(i_block), fail, test_name);
    AssertUsedBlock(GetHeader(j_block), fail, test_name);
    AssertUsedBlock(GetHeader(k_block), fail, test_name);
    AssertUsedBlock(GetHeader(c_block), fail, test_name);
    AssertUsedBlock(GetHeader(b_block), fail, test_name);

    *i = 20;
    if (*i != 20) {
        fail = true;
        PrintTestFail(test_name);
        std::cerr << "Expected to get 20 from i but got: " << *i << std::endl;
    }

    *j = 1'100'000'000'000'000'111;
    if (*j != 1'100'000'000'000'000'111) {
        fail = true;
        PrintTestFail(test_name);
        std::cerr << "Expected to get 1'100'000'000'000'000'111 from j but got: " << *j << std::endl;
    }

    *k = -9'100'000'000'000'000'000;
    if (*k != -9'100'000'000'000'000'000) {
        fail = true;
        PrintTestFail(test_name);
        std::cerr << "Expected to get -9'100'000'000'000'000'000 from k but got: " << *k << std::endl;
    }

    *c = 'Z';
    if (*c != 'Z') {
        fail = true;
        PrintTestFail(test_name);
        std::cerr << "Expected to get 'Z' from c but got: " << *c << std::endl;
    }

    *b = true;
    if (*b != true) {
        fail = true;
        PrintTestFail(test_name);
        std::cerr << "Expected to get true from b but got: " << *b << std::endl;
    }

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_next_fit_1(Allocator& allocator) {
    std::string test_name = "TestAllocator_next_fit_1";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block_1 = allocator.New(3);
    auto block_2 = allocator.New(4);
    auto block_3 = allocator.New(5);
    auto block_4 = allocator.New(20);

    auto block_1_header = GetHeader(block_1);
    auto block_2_header = GetHeader(block_2);
    auto block_3_header = GetHeader(block_3);
    auto block_4_header = GetHeader(block_4);

    AssertUsedBlock(block_1_header, fail, test_name);
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertUsedBlock(block_3_header, fail, test_name);
    AssertUsedBlock(block_4_header, fail, test_name);
    AssertAllocatedSize(block_1_header, 8, fail, test_name);
    AssertAllocatedSize(block_2_header, 8, fail, test_name);
    AssertAllocatedSize(block_3_header, 8, fail, test_name);
    AssertAllocatedSize(block_4_header, 24, fail, test_name);

    allocator.Free(block_3);
    AssertFreeBlock(block_3_header, fail, test_name);

    auto block_5 = allocator.New(8);
    auto block_5_header = GetHeader(block_5);
    AssertAllocatedSize(block_5_header, 8, fail, test_name);

    // Check that block 3 is reused.
    AssertUsedBlock(block_3_header, fail, test_name);
    AssertUsedBlock(block_5_header, fail, test_name);
    AssertBlocksEqual(block_3_header, block_5_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_next_fit_2(Allocator& allocator) {
    std::string test_name = "TestAllocator_next_fit_2";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block_1 = allocator.New(16);
    auto block_2 = allocator.New(15);
    auto block_3 = allocator.New(14);
    auto block_4 = allocator.New(30);
    auto block_5 = allocator.New(31);

    auto block_1_header = GetHeader(block_1);
    auto block_2_header = GetHeader(block_2);
    auto block_3_header = GetHeader(block_3);
    auto block_4_header = GetHeader(block_4);
    auto block_5_header = GetHeader(block_5);

    AssertUsedBlock(block_1_header, fail, test_name);
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertUsedBlock(block_3_header, fail, test_name);
    AssertUsedBlock(block_4_header, fail, test_name);
    AssertUsedBlock(block_5_header, fail, test_name);
    AssertAllocatedSize(block_1_header, 16, fail, test_name);
    AssertAllocatedSize(block_2_header, 16, fail, test_name);
    AssertAllocatedSize(block_3_header, 16, fail, test_name);
    AssertAllocatedSize(block_4_header, 32, fail, test_name);
    AssertAllocatedSize(block_5_header, 32, fail, test_name);

    allocator.Free(block_4);
    allocator.Free(block_5);
    AssertFreeBlock(block_4_header, fail, test_name);
    AssertFreeBlock(block_5_header, fail, test_name);

    auto block_6 = allocator.New(31);
    auto block_6_header = GetHeader(block_6);
    AssertAllocatedSize(block_6_header, 32, fail, test_name);

    // Check that block 4 is reused.
    AssertUsedBlock(block_4_header, fail, test_name);
    AssertUsedBlock(block_6_header, fail, test_name);
    AssertBlocksEqual(block_4_header, block_6_header, fail, test_name);

    auto block_7 = allocator.New(27);
    auto block_7_header = GetHeader(block_7);

    // Check that block 5 is reused.
    AssertUsedBlock(block_5_header, fail, test_name);
    AssertUsedBlock(block_7_header, fail, test_name);
    AssertBlocksEqual(block_5_header, block_7_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_best_fit_1(Allocator& allocator) {
    std::string test_name = "TestAllocator_best_fit_1";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    auto block_1 = allocator.New(3);  // 8
    auto block_2 = allocator.New(50); // 56
    auto block_3 = allocator.New(10); // 16
    auto block_4 = allocator.New(29); // 32

    auto block_1_header = GetHeader(block_1);
    auto block_2_header = GetHeader(block_2);
    auto block_3_header = GetHeader(block_3);
    auto block_4_header = GetHeader(block_4);

    AssertUsedBlock(block_1_header, fail, test_name);
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertUsedBlock(block_3_header, fail, test_name);
    AssertUsedBlock(block_4_header, fail, test_name);
    AssertAllocatedSize(block_1_header, 8, fail, test_name);
    AssertAllocatedSize(block_2_header, 56, fail, test_name);
    AssertAllocatedSize(block_3_header, 16, fail, test_name);
    AssertAllocatedSize(block_4_header, 32, fail, test_name);

    allocator.Free(block_4); // 32
    allocator.Free(block_2); // 56
    AssertFreeBlock(block_4_header, fail, test_name);
    AssertFreeBlock(block_2_header, fail, test_name);

    auto block_5 = allocator.New(32);
    auto block_5_header = GetHeader(block_5);
    AssertAllocatedSize(block_5_header, 32, fail, test_name);

    // Check that block 4 is reused.
    AssertUsedBlock(block_4_header, fail, test_name);
    AssertUsedBlock(block_5_header, fail, test_name);
    AssertBlocksEqual(block_4_header, block_5_header, fail, test_name);

    // Reuse 56 bytes by splitting free block_2 by 16 and 32.
    auto block_6 = allocator.New(15);
    auto block_7 = allocator.New(30);
    auto block_6_header = GetHeader(block_6);
    auto block_7_header = GetHeader(block_7);
    AssertAllocatedSize(block_6_header, 16, fail, test_name);
    AssertAllocatedSize(block_7_header, 32, fail, test_name);

    // Check that block_2 is reused by block_6.
    AssertUsedBlock(block_6_header, fail, test_name);
    AssertUsedBlock(block_2_header, fail, test_name);
    AssertBlocksEqual(block_6_header, block_2_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}
