#pragma once

#include <iostream>

#include "allocator.cpp"

void PrintTestRunning(const std::string& test_name) {
    std::cout << "=== RUN " << test_name << std::endl;
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
    PrintTestRunning(test_name);

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
    PrintTestRunning(test_name);

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

void TestAllocator_1(const Allocator& allocator) {
    std::string test_name = "TestAllocator_1";
    bool fail = false;
    PrintTestRunning(test_name);

    auto block = allocator.New(3);
    auto block_header = GetHeader(block);

    AssertUsedBlock(block_header, fail, test_name);
    AssertAllocatedSize(block_header, sizeof(MachineWord), fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_2(const Allocator& allocator) {
    std::string test_name = "TestAllocator_2";
    bool fail = false;
    PrintTestRunning(test_name);

    auto block = allocator.New(8);
    auto block_header = GetHeader(block);

    AssertUsedBlock(block_header, fail, test_name);
    AssertAllocatedSize(block_header, 8, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_3(const Allocator& allocator) {
    std::string test_name = "TestAllocator_3";
    bool fail = false;
    PrintTestRunning(test_name);

    auto block = allocator.New(12);
    auto block_header = GetHeader(block);

    AssertUsedBlock(block_header, fail, test_name);
    AssertAllocatedSize(block_header, 16, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_4(const Allocator& allocator) {
    std::string test_name = "TestAllocator_4";
    bool fail = false;
    PrintTestRunning(test_name);

    auto block = allocator.New(12);
    auto block_header = GetHeader(block);
    allocator.Free(block);

    AssertFreeBlock(block_header, fail, test_name);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}

void TestAllocator_5(const Allocator& allocator) {
    std::string test_name = "TestAllocator_5";
    bool fail = false;
    PrintTestRunning(test_name);

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

    // AssertUsedBlock(block_2_header, fail);
    AssertUsedBlock(block_3_header, fail, test_name);
    // AssertBlocksEqual(block_2_header, block_3_header, fail);

    if (!fail) {
        PrintTestPass(test_name);
    }

    std::cout << std::endl;
}
