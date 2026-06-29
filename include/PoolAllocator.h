#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

template<typename T>
class PoolAllocator
{
public:
	PoolAllocator(std::size_t chunksPerBlock): chunksPerBlock(chunksPerBlock), allocator(nullptr), blocks({}) {}
	
	~PoolAllocator()
	{
		for (int i = 0; i < this->blocks.size(); i++)
		{
			std::destroy_at(blocks[i]);
			free(blocks[i]);
		}
	}

	template<class ... Args>
	T* Allocate(Args&&... args)
	{
		if (! this->allocator)
		{
			this->allocator = this->AllocateBlock(sizeof(T));

			this->blocks.push_back(this->allocator);
		}
		
		Chunk* freeChunk = this->allocator;
		
		this->allocator = this->allocator->next;
		
		T* newVar = reinterpret_cast<T*>(freeChunk);

		newVar = std::construct_at(newVar, std::forward<Args>(args)...);
		
		return newVar;
	}

	void Deallocate(T* ptr)
	{
		Chunk* chunk = reinterpret_cast<Chunk*>(ptr);

		chunk->next = this->allocator;

		allocator = chunk;

		std::destroy_at(ptr);
	}

private:
	struct Chunk
	{
		Chunk* next;
	};

	Chunk* AllocateBlock(std::size_t chunkSize)
	{
		chunkSize = std::max(chunkSize, sizeof(Chunk));
		
		const std::size_t blockSize = this->chunksPerBlock * chunkSize;

		Chunk* begin = reinterpret_cast<Chunk*>(malloc(blockSize));

		if (! begin)
		{
			throw std::bad_alloc();
		}

		Chunk* curr = begin;
		for (std::size_t i = 0; i < this->chunksPerBlock - 1; i++)
		{
			curr->next = reinterpret_cast<Chunk*>(reinterpret_cast<uint8_t*>(curr) + chunkSize);

			curr = curr->next;
		}

		curr->next = nullptr;

		return begin;
	}
	
	std::size_t              chunksPerBlock;
	Chunk*                   allocator;
	std::vector<Chunk*>      blocks;
};