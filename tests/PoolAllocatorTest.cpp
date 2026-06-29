#include <cstdint>
#include <gtest/gtest.h>
#include "../include/PoolAllocator.h"

namespace
{
	struct Point
	{
		int x;
		int y;
	};
	
	template<typename T>
	class PoolAllocatorTypedTest : public testing::Test
	{	
	protected:
		const std::size_t size = 8;
		
		std::optional<PoolAllocator<T>> allocator;
	
		void SetUp() override
		{
			allocator.emplace(size);
		}
	};
}

using TestedTypes = testing::Types<int, double, Point>;
TYPED_TEST_SUITE(PoolAllocatorTypedTest, TestedTypes);

TYPED_TEST(PoolAllocatorTypedTest, TestSingleAllocation)
{
	TypeParam* p = this->allocator->Allocate();

	ASSERT_FALSE(p == nullptr);
}

TYPED_TEST(PoolAllocatorTypedTest, TestManyAllocations)
{
	TypeParam* p1 = this->allocator->Allocate();
	TypeParam* p2 = this->allocator->Allocate();

	ASSERT_NE(p1, p2);
}

TYPED_TEST(PoolAllocatorTypedTest, TestUniquenessWithinBlock)
{
	std::vector<TypeParam*> ptrs;
	for (std::size_t i = 0; i < this->size; i++)
	{
		ptrs.push_back(this->allocator->Allocate());
	}

	for (std::size_t i = 0; i < ptrs.size(); i++)
	{
		for (std::size_t j = i + 1; j < ptrs.size(); j++)
		{
			EXPECT_NE(ptrs[i], ptrs[j]);
		}
	}
}

TYPED_TEST(PoolAllocatorTypedTest, TestCrossBlockAllocation) 
{
	const std::size_t       total = this->size + 1;
	std::vector<TypeParam*> ptrs;
	
	for (std::size_t i = 0; i < total; i++)
	{
		ptrs.push_back(this->allocator->Allocate());
	}

	for (TypeParam* p : ptrs)
	{
		ASSERT_FALSE(p == nullptr);
	}

	for (std::size_t i = 0; i < ptrs.size(); i++)
	{
		for (std::size_t j = i + 1; j < ptrs.size(); j++)
		{
			EXPECT_NE(ptrs[i], ptrs[j]);
		}
	}
}

TYPED_TEST(PoolAllocatorTypedTest, TestAlignment)
{
	TypeParam* p = this->allocator->Allocate();

	bool aligned = (reinterpret_cast<std::uintptr_t>(p) % alignof(TypeParam)) == 0;

	EXPECT_TRUE(aligned);
}

TYPED_TEST(PoolAllocatorTypedTest, TestDeallocateReusesBlock)
{
	TypeParam* p1 = this->allocator->Allocate();

	this->allocator->Deallocate(p1);

	TypeParam* p2 = this->allocator->Allocate();

	EXPECT_EQ(p1, p2);
}

TYPED_TEST(PoolAllocatorTypedTest, TestDeallocateIsLIFO)
{
	TypeParam* p1 = this->allocator->Allocate();
	TypeParam* p2 = this->allocator->Allocate();

	this->allocator->Deallocate(p1);
	this->allocator->Deallocate(p2);

	TypeParam* r1 = this->allocator->Allocate();
	TypeParam* r2 = this->allocator->Allocate();

	EXPECT_EQ(p1, r2);
	EXPECT_EQ(p2, r1);
}

TYPED_TEST(PoolAllocatorTypedTest, TestDeallocateAllAndReallocate)
{
	std::vector<TypeParam*> ptrs;

	for (std::size_t i = 0; i < this->size; i++)
	{
		ptrs.push_back(this->allocator->Allocate());
	}

	for (TypeParam* p : ptrs)
	{
		this->allocator->Deallocate(p);
	}

	std::vector<TypeParam*> reallocated;

	for (std::size_t i = 0; i < this->size; i++)
	{
		reallocated.push_back(this->allocator->Allocate());
	}

	for (TypeParam* p : reallocated)
	{
		ASSERT_FALSE(p == nullptr);
	}

	for (std::size_t i = 0; i < reallocated.size(); i++)
	{
		for (std::size_t j = i + 1; j < reallocated.size(); j++)
		{
			EXPECT_NE(reallocated[i], reallocated[j]);
		}
	}
}

namespace
{
	class PoolAllocatorTest : public testing::Test
	{
	protected:
			const std::size_t size = 8;
			std::optional<PoolAllocator<int>> allocator;
	
			void SetUp() override
			{
				allocator.emplace(size);
			}
	};
}

TEST_F(PoolAllocatorTest, TestWritable)
{
	int* p = this->allocator->Allocate();

	*p = 100;
	ASSERT_EQ(*p, 100);
}

TEST_F(PoolAllocatorTest, TestDeallocatedMemoryIsWritableAfterReuse)
{
	int* p1 = this->allocator->Allocate();

	*p1 = 100;

	this->allocator->Deallocate(p1);

	int* p2 = this->allocator->Allocate();

	*p2 = 200;

	EXPECT_EQ(*p2, 200);
}

TEST_F(PoolAllocatorTest, TestConstructing)
{
	int* p = this->allocator->Allocate(20);

	EXPECT_EQ(*p, 20);
}

namespace
{
	class PoolAllocatorDestructingTest : public testing::Test
	{
	protected:
			struct Spy
			{
				int* count;
	
				Spy(int* c): count(c) {}
				~Spy()
				{
					(*count)++;
				}
			};
	
			const std::size_t size = 8;
			std::optional<PoolAllocator<Spy>> allocator;
			int destroyCount = 0;
	
			void SetUp() override
			{
				allocator.emplace(size);
				destroyCount = 0;
			}
	};
}

TEST_F(PoolAllocatorDestructingTest, TestDestructingSingleObject)
{
	Spy* p = this->allocator->Allocate(&this->destroyCount);
	
	EXPECT_EQ(this->destroyCount, 0);
	
	this->allocator->Deallocate(p);
	
	EXPECT_EQ(this->destroyCount, 1);
}

TEST_F(PoolAllocatorDestructingTest, TestDestructingMutipleObjects)
{
	Spy* p1 = this->allocator->Allocate(&this->destroyCount);
	Spy* p2 = this->allocator->Allocate(&this->destroyCount);
	
	this->allocator->Deallocate(p1);
	this->allocator->Deallocate(p2);
	
	EXPECT_EQ(this->destroyCount, 2);
}

TEST_F(PoolAllocatorDestructingTest, TestPoolDestructor)
{
	Spy* p1 = this->allocator->Allocate(&this->destroyCount);
	Spy* p2 = this->allocator->Allocate(&this->destroyCount);
	
	this->allocator->~PoolAllocator();

	EXPECT_EQ(this->destroyCount, 2);

	new (std::addressof(*this->allocator)) PoolAllocator<Spy>(this->size);
}