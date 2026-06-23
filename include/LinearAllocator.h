#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>

class LinearAllocator
{
public:
	LinearAllocator(std::size_t size);

	~LinearAllocator();

	template<typename T>
	T* Allocate(){
		if (this->curr + sizeof(T) > this->end){
			return nullptr;
		}
		this->lastAlloc = this->curr; // update last alloc

		T* ptr = reinterpret_cast<T*>(this->curr);
		this->curr += sizeof(T);
		return ptr;
	};


	// only works on last allocation
	template<typename T>
	T* Reallocate(T* reallocatedC, std::size_t newSize){
		if (reinterpret_cast<uint8_t*>(reallocatedC) != this->lastAlloc) {
			return nullptr;
		}
		uint8_t* nextCurr = this->lastAlloc + newSize;
		if (nextCurr > this->end) {
			return nullptr;
		}
		this->curr = nextCurr;
		return reallocatedC;
	};

	void Reset();

private:
	uint8_t* start;
	uint8_t* curr;
	uint8_t* end;
	uint8_t* lastAlloc;
};
