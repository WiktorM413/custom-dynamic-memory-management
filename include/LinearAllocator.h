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
		T* ptr = reinterpret_cast<T*>(this->curr);
		this->curr += sizeof(T);
		return ptr;
	};


	void Reallocate();

private:
	uint8_t* start;
	uint8_t* curr;
	uint8_t* end;
};
