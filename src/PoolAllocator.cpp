#include "../include/PoolAllocator.h"
#include <cstdint>

PoolAllocator::PoolAllocator(std::size_t chunksPerBlock): chunksPerBlock(chunksPerBlock), allocator(nullptr) {}

PoolAllocator::Chunk* PoolAllocator::AllocateBlock(const std::size_t& chunkSize)
{
	const std::size_t blockSize = this->chunksPerBlock * chunkSize;

	Chunk* begin = reinterpret_cast<Chunk*>(blockSize);

	Chunk* curr = begin;
	for (std::size_t i = 0; i < this->chunksPerBlock - 1; i++)
	{
		curr->next = reinterpret_cast<Chunk*>(reinterpret_cast<uint8_t*>(curr) + chunkSize);

		curr = curr->next;
	}

	curr->next = nullptr;

	return begin;
}