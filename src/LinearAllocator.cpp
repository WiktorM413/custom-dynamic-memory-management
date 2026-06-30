#include "../include/LinearAllocator.h"

#include <cstddef>
#include <iostream>

LinearAllocator::LinearAllocator(size_t size)
{
	this->start = static_cast<uint8_t*>(malloc(size));

	if(this->start == nullptr){
		std::cout<<"buy some ram \n";
		throw std::bad_alloc();
	}

	this->curr  = this->start;
	this->end   = this->start + size;
	this->lastAlloc = nullptr;
	this->destructStart = reinterpret_cast<DestructorEntry*>(this->end);
	this->lastDestructor = nullptr;

}

LinearAllocator::~LinearAllocator()
{
	this->Reset();
	free(this->start);
}


void LinearAllocator::Reset(){
	DestructorEntry* entry = this->destructStart;
	while (entry < reinterpret_cast<DestructorEntry*>(this->end))
	{
		entry->destructor(entry->ptr, entry->count);
		entry++;
	}

	this->curr = this->start;
	this->lastAlloc = nullptr;
	this->destructStart = reinterpret_cast<DestructorEntry*>(this->end);
};
