#include <utility>

#include "../include/block.h"

// SizeOfData returns a size of MemoryBlock data.
size_t SizeOfData() {
    /*
    declval returns reference type of a <MemoryBlock> so we don't need a
    constructor.
    https://en.cppreference.com/w/cpp/utility/declval
    https://docs.w3cub.com/cpp/utility/declval/
    */
    return sizeof(std::declval<MemoryBlock>().Data);
}

// GetHeader returns a header to the needed object.
MemoryBlock* GetHeader(const MachineWord *data) {
    return (MemoryBlock *)((char *)data + SizeOfData() - sizeof(MemoryBlock));
}
