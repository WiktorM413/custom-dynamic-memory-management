#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>


class LinearAllocator
{
public:
	LinearAllocator(std::size_t size);

	~LinearAllocator();


	template<typename T, typename... Args>
	T* Allocate(Args&&... args){
		void* ptr  = this->curr;
		size_t space = this->end - this->curr;

		if (std::align(alignof(T), sizeof(T), ptr, space) == nullptr){
			return nullptr;
		}

		uint8_t* uint8_tPtr = reinterpret_cast<uint8_t*>(ptr);

		if (uint8_tPtr + sizeof(T) > reinterpret_cast<uint8_t*>(this->destructStart) - sizeof(DestructorEntry)){
			return nullptr;
		}

		this->lastAlloc = uint8_tPtr;
		this->curr = uint8_tPtr + sizeof(T);


		T* res = std::construct_at(reinterpret_cast<T*>(ptr), std::forward<Args>(args)...);
		this->RegisterDestructor<T>(res, 1);
		return res;
	};

	template<typename T>
	T* AllocateArray(std::size_t count, std::initializer_list<T> args = {}){
		if (args.size()>count){
			throw std::out_of_range("number of args: " + std::to_string(args.size()) + " exceeds allocated count of " + std::to_string(count));

		}

		void* ptr  = this->curr;
		size_t space = this->end - this->curr;
		size_t size = sizeof(T)*count;

		if (std::align(alignof(T), size, ptr, space) == nullptr){
			return nullptr;
		}

		uint8_t* uint8_tPtr = reinterpret_cast<uint8_t*>(ptr);

		if (uint8_tPtr + size > reinterpret_cast<uint8_t*>(this->destructStart) - sizeof(DestructorEntry)){
			return nullptr;
		}

		this->lastAlloc = uint8_tPtr;
		this->curr = uint8_tPtr + size;


		T* res = reinterpret_cast<T*>(ptr);


		for (std::size_t i = 0; i < count; i++){
			if (i < args.size())
				std::construct_at(res + i, *(args.begin() + i));
			else
				std::construct_at(res + i);
		}


		this->RegisterDestructor<T>(res, count);
		return res;
	};


	// only works on last allocation
	template<typename T>
	T* Reallocate(void* reallocatedC, std::size_t arraySize = 1){
		if (reinterpret_cast<uint8_t*>(reallocatedC) != this->lastAlloc)
		{
			return nullptr;
		}

		std::size_t newSize = sizeof(T) * arraySize;
		uint8_t* nextCurr = this->lastAlloc + newSize;

		if (nextCurr > this->end)
		{
			return nullptr;
		}
		if (nextCurr > reinterpret_cast<uint8_t*>(this->destructStart)){
			return nullptr;
		}

		this->curr = nextCurr;
		this->lastDestructor->count = arraySize;
		return reinterpret_cast<T*>(this->lastAlloc);
	};


	void Reset();

private:
	struct DestructorEntry
	{
		void* ptr;
		std::size_t count;
		void(*destructor)(void*, std::size_t);
	};

	uint8_t* start;
	uint8_t* curr;
	uint8_t* end;
	uint8_t* lastAlloc;
	DestructorEntry* destructStart;
	DestructorEntry* lastDestructor;

	template<typename T>
	static void DestructorWrapper(void* ptr, std::size_t count)
	{
		std::destroy_n(static_cast<T*>(ptr), count);
	}

	template<typename T>
	void RegisterDestructor(void* ptr, std::size_t count)
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			this->destructStart--;
			this->destructStart->ptr = ptr;
			this->destructStart->count = count;
			this->destructStart->destructor = &DestructorWrapper<T>;
			this->lastDestructor = this->destructStart;
		}
	}
};
