#include "../include/LinearAllocator.h"

#include <cstddef>

LinearAllocator::LinearAllocator(size_t size)
{
		this->start = static_cast<uint8_t*>(malloc(size));
		this->curr  = start;
		this->end   = start + size;
}

LinearAllocator::~LinearAllocator()
{
	free(this->start);
}

template<typename T>
T* Allocate()
{
	
}

void Reallocate()
{
	
}