#pragma once

#include <stdlib.h>

// MachineWord represents a size to which all allocations should be aligned.
using MachineWord = uintptr_t;

// MemoryBlock contains header with additional info and the actual data.
struct MemoryBlock {
    // Header fields.
    size_t Size;
    bool Used;
    MemoryBlock *Next;

    // Actual data.
    MachineWord Data[1];
};

size_t SizeOfData();

MemoryBlock* GetHeader(MachineWord *data);
