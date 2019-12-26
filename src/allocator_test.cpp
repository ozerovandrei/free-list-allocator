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

void TestPadding(const Allocator& allocator) {
    std::string test_name = "TestPadding";
    bool fail = false;
    PrintTestRunning(test_name, allocator);

    size_t initial_sizes[35] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 21, 22, 23, 24, 32, 36, 40, 42, 47, 48, 60, 62, 63, 64, 65,
        100, 101, 104};

    size_t expected_padding[35] = {8, 7, 6, 5, 4, 3, 2, 1, 8, 7, 6, 5, 4, 3,
        2, 1, 8, 3, 2, 1, 8, 8, 4, 8, 6, 1, 8, 4, 2, 1, 8, 7, 4, 3, 8};

    for (auto i = 0; i < 35; ++i) {
        auto actual = allocator.Padding(initial_sizes[i]);
        if (expected_padding[i] != actual) {
            fail = true;
            PrintTestFail(test_name);
            std::cerr << "Expected " << expected_padding[i]
            << " from Padding(" << initial_sizes[i] << "), but got: "
            << actual << std::endl;
        }
    }

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
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

    auto block = allocator.New(3);
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

    auto block = allocator.New(12);
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

    auto block = allocator.New(12);
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

    auto block_1 = allocator.New(12);
    auto block_2 = allocator.New(6);
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

    auto block_1 = allocator.New(6);
    auto block_2 = allocator.New(60);
    auto block_3 = allocator.New(60);
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
    auto block_4 = allocator.New(31);
    auto block_5 = allocator.New(30);
    auto block_4_header = GetHeader(block_4);
    auto block_5_header = GetHeader(block_5);

    AssertAllocatedSize(block_4_header, 32, fail, test_name);
    AssertAllocatedSize(block_5_header, 32, fail, test_name);

    AssertUsedBlock(block_2_header, fail, test_name);
    AssertFreeBlock(block_3_header, fail, test_name);
    AssertBlocksEqual(block_2_header, block_4_header, fail, test_name);

    // Allocate additional smaller block and check that it reused the block_3
    // place.
    auto block_6 = allocator.New(28);
    auto block_6_header = GetHeader(block_6);

    AssertAllocatedSize(block_6_header, 32, fail, test_name);

    AssertUsedBlock(block_3_header, fail, test_name);
    AssertBlocksEqual(block_3_header, block_6_header, fail, test_name);

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
