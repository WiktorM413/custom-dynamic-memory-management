#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>

class LinearAllocator
{
public:	
	LinearAllocator(std::size_t size);

	~LinearAllocator();

	template<typename T> T* Allocate();
	void Reallocate();

private:
	uint8_t* start;
	uint8_t* curr;
	uint8_t* end;
};