#pragma once

#include <cstddef>
class PoolAllocator
{
public:
	PoolAllocator(std::size_t chunksPerBlock);

	template<typename T>
	T* Allocate()
	{
		
	}

	template<typename T>
	void Deallocate(T* ptr)
	{
		
	}

private:
	struct Chunk
	{
		Chunk* next;
	};

	Chunk* AllocateBlock();
	
	std::size_t chunksPerBlock;
	Chunk*      allocator;
};