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

    size_t initial_sizes[30] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 16, 21, 22,
        23, 24, 32, 36, 40, 42, 47, 48, 60, 62, 63, 64, 65, 100, 101, 104};
    size_t expected_padding[30] = {7, 6, 5, 4, 3, 2, 1, 8, 7, 6, 1, 8, 3, 2,
        1, 8, 8, 4, 8, 6, 1, 8, 4, 2, 1, 8, 7, 4, 3, 8};

    for (auto i = 0; i < 30; ++i) {
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

    size_t initial_sizes[30] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 16, 21, 22,
        23, 24, 32, 36, 40, 42, 47, 48, 60, 62, 63, 64, 65, 100, 101, 104};
    size_t expected_alignment[30] = {8, 8, 8, 8, 8, 8, 8, 8, 16, 16, 16, 16, 24,
        24, 24, 24, 32, 40, 40, 48, 48, 48, 64, 64, 64, 64, 72, 104, 104, 104};

    for (auto i = 0; i < 30; ++i) {
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
