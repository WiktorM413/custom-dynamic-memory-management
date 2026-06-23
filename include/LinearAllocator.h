#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>

class LinearAllocator
{
	uint8_t* start;
	uint8_t* curr;
	uint8_t* end;
	
	LinearAllocator(std::size_t size);

	~LinearAllocator();

	template<typename T> T* Allocate();
	void Reallocate();
};