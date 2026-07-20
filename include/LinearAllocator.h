#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

enum class AllocMode
{
	GrowOut,
	CutIn
};

template<AllocMode Mode = AllocMode::GrowOut>
class LinearAllocator
{
	public:
		LinearAllocator(std::size_t size, std::size_t maxDestructors = 64)
		{
			std::size_t registrySize = maxDestructors * sizeof(DestructorEntry);

			if constexpr (Mode == AllocMode::GrowOut)
			{
				this->start = static_cast<uint8_t*>(malloc(size + registrySize));
				this->end = this->start + size;
				this->realEnd = this->start + size + registrySize;
			}
			else
			{
				this->start = static_cast<uint8_t*>(malloc(size));
				this->end = this->start + size;
				this->realEnd = this->start + size;
			}

			if (this->start == nullptr)
			{
				std::cout << "buy some ram\n";
				throw std::bad_alloc();
			}

			this->curr = this->start;
			this->lastAlloc = nullptr;
			this->lastDestructor = nullptr;
			this->destructStart = reinterpret_cast<DestructorEntry*>(this->realEnd);
			this->destructCap = reinterpret_cast<DestructorEntry*>(this->realEnd) - maxDestructors;
		}

		~LinearAllocator()
		{
			this->Reset();
			free(this->start);
		}

		LinearAllocator(const LinearAllocator&) = delete;
		LinearAllocator& operator=(const LinearAllocator&) = delete;



		//														Allocate
		template<typename T, typename... Args>
		T* Allocate(Args&&... args)
		{
			void*  ptr   = this->curr;
			size_t space = this->end - this->curr;

			if (std::align(alignof(T), sizeof(T), ptr, space) == nullptr) [[unlikely]]
			{
				return nullptr;
			}

			uint8_t* uint8_tPtr = reinterpret_cast<uint8_t*>(ptr);

			if constexpr (std::is_trivially_destructible_v<T>)
			{
				if constexpr (Mode == AllocMode::GrowOut)
				{
					if (uint8_tPtr + sizeof(T) > this->end) [[unlikely]]
					{
						return nullptr;
					}
				}
				else
				{
					if (uint8_tPtr + sizeof(T) > reinterpret_cast<uint8_t*>(this->destructCap)) [[unlikely]]
					{
						return nullptr;
					}
				}

				this->lastAlloc = uint8_tPtr;
				this->curr = uint8_tPtr + sizeof(T);
				this->lastDestructor = nullptr;

				return std::construct_at(reinterpret_cast<T*>(ptr), std::forward<Args>(args)...);
			}
			else
			{
				if (uint8_tPtr + sizeof(T) > reinterpret_cast<uint8_t*>(this->destructStart) - sizeof(DestructorEntry)) [[unlikely]]
					return nullptr;

				if (this->destructStart <= this->destructCap) [[unlikely]]
					return nullptr;

				this->lastAlloc = uint8_tPtr;
				this->curr = uint8_tPtr + sizeof(T);

				T* res = std::construct_at(reinterpret_cast<T*>(ptr), std::forward<Args>(args)...);
				this->RegisterDestructor<T>(res, 1);
				return res;
			}
		}

		//																				Allocate Array
		template<typename T>
		T* AllocateArray(std::size_t count, std::initializer_list<T> args = {})
		{
			if (args.size() > count)
			{
				throw std::out_of_range(
					"number of args: " + std::to_string(args.size()) +
					" exceeds allocated count of " + std::to_string(count));
			}

			void*  ptr = this->curr;
			size_t space = this->end - this->curr;
			size_t size = sizeof(T) * count;

			if (std::align(alignof(T), size, ptr, space) == nullptr) [[unlikely]]
			{
				return nullptr;
			}

			uint8_t* uint8_tPtr = reinterpret_cast<uint8_t*>(ptr);

			if constexpr (std::is_trivially_destructible_v<T>)
			{
				if constexpr (Mode == AllocMode::GrowOut)
				{
					if (uint8_tPtr + size > this->end) [[unlikely]]
					{
						return nullptr;
					}
				}
				else
				{
					if (uint8_tPtr + size > reinterpret_cast<uint8_t*>(this->destructCap)) [[unlikely]]
					{
						return nullptr;
					}
				}

				this->lastAlloc = uint8_tPtr;
				this->curr = uint8_tPtr + size;
				this->lastDestructor = nullptr;
			}
			else
			{
				if (uint8_tPtr + size > reinterpret_cast<uint8_t*>(this->destructStart) - sizeof(DestructorEntry)) [[unlikely]]
				{
					return nullptr;
				}

				if (this->destructStart <= this->destructCap) [[unlikely]]
				{
					return nullptr;
				}

				this->lastAlloc = uint8_tPtr;
				this->curr = uint8_tPtr + size;
			}

			T* res = reinterpret_cast<T*>(ptr);

			for (std::size_t i = 0; i < count; i++)
			{
				if (i < args.size()){
					std::construct_at(res + i, *(args.begin() + i));
				}
				else{
					std::construct_at(res + i);
				}
			}

			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				this->RegisterDestructor<T>(res, count);
			}

			return res;
		}

		//																		Reallocate
		template<typename T>
		T* Reallocate(void* reallocatedC, std::size_t arraySize = 1)
		{
			if (reinterpret_cast<uint8_t*>(reallocatedC) != this->lastAlloc) [[unlikely]]
			{
				return nullptr;
			}

			std::size_t newSize  = sizeof(T) * arraySize;
			uint8_t*    nextCurr = this->lastAlloc + newSize;

			if constexpr (Mode == AllocMode::GrowOut)
			{
				if (nextCurr > this->end) [[unlikely]]
				{
					return nullptr;
				}
			}
			else
			{
				if (nextCurr > reinterpret_cast<uint8_t*>(this->destructCap)) [[unlikely]]
				{
					return nullptr;
				}
			}

			if (nextCurr > reinterpret_cast<uint8_t*>(this->destructStart)) [[unlikely]]
			{
				return nullptr;
			}

			this->curr = nextCurr;

			if (this->lastDestructor != nullptr)
			{
				this->lastDestructor->count = arraySize;
			}

			return reinterpret_cast<T*>(this->lastAlloc);
		}


		//																			Reset
		void Reset()
		{
			DestructorEntry* entry = this->destructStart;
			while (entry < reinterpret_cast<DestructorEntry*>(this->realEnd))
			{
				entry->destructor(entry->ptr, entry->count);
				entry++;
			}

			this->curr = this->start;
			this->lastAlloc = nullptr;
			this->lastDestructor = nullptr;
			this->destructStart = reinterpret_cast<DestructorEntry*>(this->realEnd);
		}

	private:
		//																				Destructo=r
		struct DestructorEntry
		{
			void* ptr;
			std::size_t count;
			void(*destructor)(void*, std::size_t);
		};

		template<typename T>
		static void DestructorWrapper(void* ptr, std::size_t count)
		{
			std::destroy_n(static_cast<T*>(ptr), count);
		}

		template<typename T>
		void RegisterDestructor(void* ptr, std::size_t count)
		{
			this->destructStart--;
			this->destructStart->ptr = ptr;
			this->destructStart->count = count;
			this->destructStart->destructor = &DestructorWrapper<T>;
			this->lastDestructor = this->destructStart;
		}

		//																				Firlds
		uint8_t* start;
		uint8_t* curr;
		uint8_t* end;
		uint8_t* realEnd;
		uint8_t* lastAlloc;
		DestructorEntry* destructCap;
		DestructorEntry* destructStart;
		DestructorEntry* lastDestructor;
};


using FrameAllocator = LinearAllocator<AllocMode::GrowOut>;
using TightAllocator = LinearAllocator<AllocMode::CutIn>;
