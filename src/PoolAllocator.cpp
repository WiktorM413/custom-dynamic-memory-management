#include "../include/PoolAllocator.h"

PoolAllocator::PoolAllocator(std::size_t chunksPerBlock): chunksPerBlock(chunksPerBlock), allocator(nullptr) {}