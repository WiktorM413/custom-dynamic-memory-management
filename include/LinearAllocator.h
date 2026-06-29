#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>

class LinearAllocator
{
public:
	LinearAllocator(std::size_t size);

	~LinearAllocator();


	//TODO:	 allocate<string>("content") in 1 line
	template<typename T, typename... Args>
	T* Allocate(Args&&... arguments){
		void* ptr  = this->curr;
		size_t space = this->end - this->curr;

		if ((std::align(alignof(T), sizeof(T), ptr, space)) == nullptr){
			return nullptr;
		}
		uint8_t* uint8_tPtr = reinterpret_cast<uint8_t*>(ptr);
		this->lastAlloc = uint8_tPtr;
		this->curr = uint8_tPtr + sizeof(T);

		T* res = reinterpret_cast<T*>(ptr);

		return std::construct_at(res, std::forward<Args>(arguments)...);
	};

	template<typename T>
	T* AllocateArray(std::size_t count){
		void* ptr  = this->curr;
		size_t space = this->end - this->curr;
		size_t size = sizeof(T)*count;

		if ((std::align(alignof(T), size, ptr, space)) == nullptr){
			return nullptr;
		}
		uint8_t* uint8_tPtr = reinterpret_cast<uint8_t*>(ptr);
		this->lastAlloc = uint8_tPtr;
		this->curr = uint8_tPtr + size;

		T* res = reinterpret_cast<T*>(ptr);

		return res;
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
