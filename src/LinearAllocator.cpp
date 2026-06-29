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

}

LinearAllocator::~LinearAllocator()
{
	free(this->start);
}


void LinearAllocator::Reset(){
	this->curr  = this->start;
	this->lastAlloc = nullptr;
};
