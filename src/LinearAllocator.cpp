#include "../include/LinearAllocator.h"

#include <cstddef>

LinearAllocator::LinearAllocator(size_t size)
{
	this->start = static_cast<uint8_t*>(malloc(size));
	this->curr  = this->start;
	this->end   = this->start + size;
}

LinearAllocator::~LinearAllocator()
{
	free(this->start);
}

void LinearAllocator::Reallocate()
{
}
