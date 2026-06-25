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
		uintptr_t aligned = (reinterpret_cast<uintptr_t>(this->curr + alignof(T) -1)) & ~(alignof(T)-1);
		if (reinterpret_cast<uint8_t*>(aligned)+ sizeof(T) > this->end)
		{
			return nullptr;
		}

		this->lastAlloc = reinterpret_cast<uint8_t*>(aligned);
		
		T* ptr = reinterpret_cast<T*>(aligned);
		this->curr = reinterpret_cast<uint8_t*>(aligned) + sizeof(T);

		return ptr;
	};


	// only works on last allocation
	template<typename TTo>
	TTo* Reallocate(void* reallocatedC, std::size_t arraySize = 1){
		if (reinterpret_cast<uint8_t*>(reallocatedC) != this->lastAlloc)
		{
			return nullptr;
		}

		std::size_t newSize = sizeof(TTo) * arraySize;

		uint8_t* nextCurr = this->lastAlloc + newSize;
		if (nextCurr > this->end)
		{
			return nullptr;
		}

		this->curr = nextCurr;
		return reinterpret_cast<TTo*>(this->lastAlloc);
	};

	void Reset();

private:
	uint8_t* start;
	uint8_t* curr;
	uint8_t* end;
	uint8_t* lastAlloc;
};
