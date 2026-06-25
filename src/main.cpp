#include <iostream>
#include <cstring>
#include "../include/LinearAllocator.h"

void PrintSeparator(const char* testName)
{
	std::cout << "\n=== " << testName << " ===\n";
}

int main()
{
	// ---- basic int ----
	PrintSeparator("Basic int allocation");
	{
		LinearAllocator allocator(sizeof(int) * 4);

		int* a = allocator.Allocate<int>();
		int* b = allocator.Allocate<int>();
		int* c = allocator.Allocate<int>();

		*a = 10;
		*b = 20;
		*c = 30;

		std::cout << "a: " << *a << " at " << static_cast<void*>(a) << "\n";
		std::cout << "b: " << *b << " at " << static_cast<void*>(b) << "\n";
		std::cout << "c: " << *c << " at " << static_cast<void*>(c) << "\n";

		std::cout << "gap a->b: " << (reinterpret_cast<uint8_t*>(b) - reinterpret_cast<uint8_t*>(a)) << " bytes (expected " << sizeof(int) << ")\n";
		std::cout << "gap b->c: " << (reinterpret_cast<uint8_t*>(c) - reinterpret_cast<uint8_t*>(b)) << " bytes (expected " << sizeof(int) << ")\n";
	}

	// ---- float ----
	PrintSeparator("Float allocation");
	{
		LinearAllocator allocator(sizeof(float) * 4);

		float* a = allocator.Allocate<float>();
		float* b = allocator.Allocate<float>();

		*a = 3.14f;
		*b = 2.71f;

		std::cout << "a: " << *a << " at " << static_cast<void*>(a) << "\n";
		std::cout << "b: " << *b << " at " << static_cast<void*>(b) << "\n";
		std::cout << "float aligned: " << (reinterpret_cast<uintptr_t>(a) % alignof(float) == 0 ? "yes" : "NO") << "\n";
	}

	// ---- double ----
	PrintSeparator("Double allocation");
	{
		LinearAllocator allocator(sizeof(double) * 4);

		double* a = allocator.Allocate<double>();
		double* b = allocator.Allocate<double>();

		*a = 1.7976931348623157;
		*b = 2.2250738585072014;

		std::cout << "a: " << *a << " at " << static_cast<void*>(a) << "\n";
		std::cout << "b: " << *b << " at " << static_cast<void*>(b) << "\n";
		std::cout << "double aligned: " << (reinterpret_cast<uintptr_t>(a) % alignof(double) == 0 ? "yes" : "NO") << "\n";
	}

	// ---- char ----
	PrintSeparator("Char allocation");
	{
		LinearAllocator allocator(sizeof(char) * 8);

		char* a = allocator.Allocate<char>();
		char* b = allocator.Allocate<char>();
		char* c = allocator.Allocate<char>();

		*a = 'A';
		*b = 'B';
		*c = 'C';

		std::cout << "a: " << *a << " at " << static_cast<void*>(a) << "\n";
		std::cout << "b: " << *b << " at " << static_cast<void*>(b) << "\n";
		std::cout << "c: " << *c << " at " << static_cast<void*>(c) << "\n";

		// chars are 1 byte so no padding between them
		std::cout << "gap a->b: " << (reinterpret_cast<uint8_t*>(b) - reinterpret_cast<uint8_t*>(a)) << " bytes (expected 1)\n";
	}

	// ---- char* string ----
	PrintSeparator("char* string (manual)");
	{
		const char* message = "Hello Allocator";
		std::size_t len     = strlen(message) + 1; // +1 for null terminator

		LinearAllocator allocator(len);

		char* str = allocator.Allocate<char>();
		// allocate gives one char, reallocate to full length
		str = allocator.Reallocate<char>(str, len);

		if (str != nullptr)
		{
			memcpy(str, message, len);
			std::cout << "string: " << str << "\n";
			std::cout << "length: " << strlen(str) << "\n";
			std::cout << "at: " << static_cast<void*>(str) << "\n";
		}
		else
		{
			std::cout << "string allocation failed\n";
		}
	}

	// ---- bool ----
	PrintSeparator("Bool allocation");
	{
		LinearAllocator allocator(sizeof(bool) * 4);

		bool* a = allocator.Allocate<bool>();
		bool* b = allocator.Allocate<bool>();

		*a = true;
		*b = false;

		std::cout << "a: " << (*a ? "true" : "false") << " at " << static_cast<void*>(a) << "\n";
		std::cout << "b: " << (*b ? "true" : "false") << " at " << static_cast<void*>(b) << "\n";
	}

	// ---- int64 / uint64 ----
	PrintSeparator("int64 / uint64 allocation");
	{
		LinearAllocator allocator(sizeof(int64_t) * 4);

		int64_t*  a = allocator.Allocate<int64_t>();
		uint64_t* b = allocator.Allocate<uint64_t>();

		*a = -9223372036854775807LL;
		*b =  18446744073709551615ULL;

		std::cout << "int64:  " << *a << " at " << static_cast<void*>(a) << "\n";
		std::cout << "uint64: " << *b << " at " << static_cast<void*>(b) << "\n";
		std::cout << "int64  aligned: " << (reinterpret_cast<uintptr_t>(a) % alignof(int64_t)  == 0 ? "yes" : "NO") << "\n";
		std::cout << "uint64 aligned: " << (reinterpret_cast<uintptr_t>(b) % alignof(uint64_t) == 0 ? "yes" : "NO") << "\n";
	}

	// ---- raw int array via reallocate ----
	PrintSeparator("Raw int array via Reallocate");
	{
		LinearAllocator allocator(sizeof(int) * 20);

		int* arr = allocator.Allocate<int>();
		arr      = allocator.Reallocate<int>(arr, 20);

		if (arr != nullptr)
		{
			for (int i = 0; i < 20; i++) arr[i] = i * i;

			std::cout << "squares: ";
			for (int i = 0; i < 20; i++) std::cout << arr[i] << " ";
			std::cout << "\n";
		}
		else
		{
			std::cout << "array allocation failed\n";
		}
	}

	// ---- raw float array ----
	PrintSeparator("Raw float array via Reallocate");
	{
		LinearAllocator allocator(sizeof(float) * 8);

		float* arr = allocator.Allocate<float>();
		arr        = allocator.Reallocate<float>(arr, 8);

		if (arr != nullptr)
		{
			for (int i = 0; i < 8; i++) arr[i] = i * 0.5f;

			std::cout << "halves: ";
			for (int i = 0; i < 8; i++) std::cout << arr[i] << " ";
			std::cout << "\n";
		}
	}

	// ---- struct Vec2 ----
	PrintSeparator("Struct Vec2");
	{
		struct Vec2 { float x, y; };

		LinearAllocator allocator(sizeof(Vec2) * 4);

		Vec2* a = allocator.Allocate<Vec2>();
		Vec2* b = allocator.Allocate<Vec2>();

		a->x = 1.0f; a->y = 2.0f;
		b->x = 3.0f; b->y = 4.0f;

		std::cout << "a: (" << a->x << ", " << a->y << ") at " << static_cast<void*>(a) << "\n";
		std::cout << "b: (" << b->x << ", " << b->y << ") at " << static_cast<void*>(b) << "\n";
		std::cout << "Vec2 aligned: " << (reinterpret_cast<uintptr_t>(a) % alignof(Vec2) == 0 ? "yes" : "NO") << "\n";
	}

	// ---- struct Vec3 ----
	PrintSeparator("Struct Vec3");
	{
		struct Vec3 { float x, y, z; };

		LinearAllocator allocator(sizeof(Vec3) * 4);

		Vec3* v = allocator.Allocate<Vec3>();

		v->x = 1.0f; v->y = 2.0f; v->z = 3.0f;

		std::cout << "v: (" << v->x << ", " << v->y << ", " << v->z << ")\n";
		std::cout << "Vec3 sizeof: "  << sizeof(Vec3)  << "\n";
		std::cout << "Vec3 alignof: " << alignof(Vec3) << "\n";
		std::cout << "Vec3 aligned: " << (reinterpret_cast<uintptr_t>(v) % alignof(Vec3) == 0 ? "yes" : "NO") << "\n";
	}

	// ---- struct with mixed field sizes ----
	PrintSeparator("Struct with mixed field sizes");
	{
		struct Mixed
		{
			char   flag;    // 1 byte
			int    id;      // 4 bytes — compiler pads after flag
			double value;   // 8 bytes
		};

		LinearAllocator allocator(sizeof(Mixed) * 4);

		Mixed* m = allocator.Allocate<Mixed>();

		m->flag  = 'X';
		m->id    = 99;
		m->value = 1.23456789;

		std::cout << "flag:  " << m->flag  << "\n";
		std::cout << "id:    " << m->id    << "\n";
		std::cout << "value: " << m->value << "\n";
		std::cout << "Mixed sizeof:  " << sizeof(Mixed)  << " (not " << sizeof(char) + sizeof(int) + sizeof(double) << " due to padding)\n";
		std::cout << "Mixed aligned: " << (reinterpret_cast<uintptr_t>(m) % alignof(Mixed) == 0 ? "yes" : "NO") << "\n";
	}

	// ---- mixed types sequential, alignment gaps ----
	PrintSeparator("Mixed types sequential — watch alignment gaps");
	{
		LinearAllocator allocator(128);

		char*    c = allocator.Allocate<char>();
		int*     i = allocator.Allocate<int>();
		double*  d = allocator.Allocate<double>();
		float*   f = allocator.Allocate<float>();
		int64_t* l = allocator.Allocate<int64_t>();

		*c = 'Q'; *i = 42; *d = 3.14; *f = 2.71f; *l = 123456789LL;

		std::cout << "char    at " << static_cast<void*>(c) << " value: " << *c  << "\n";
		std::cout << "int     at " << static_cast<void*>(i) << " value: " << *i  << "\n";
		std::cout << "double  at " << static_cast<void*>(d) << " value: " << *d  << "\n";
		std::cout << "float   at " << static_cast<void*>(f) << " value: " << *f  << "\n";
		std::cout << "int64   at " << static_cast<void*>(l) << " value: " << *l  << "\n";

		// print gaps to see padding
		std::cout << "gap char->int:    " << (reinterpret_cast<uint8_t*>(i) - reinterpret_cast<uint8_t*>(c)) << " bytes\n";
		std::cout << "gap int->double:  " << (reinterpret_cast<uint8_t*>(d) - reinterpret_cast<uint8_t*>(i)) << " bytes\n";
		std::cout << "gap double->float:" << (reinterpret_cast<uint8_t*>(f) - reinterpret_cast<uint8_t*>(d)) << " bytes\n";
		std::cout << "gap float->int64: " << (reinterpret_cast<uint8_t*>(l) - reinterpret_cast<uint8_t*>(f)) << " bytes\n";
	}

	// ---- out of memory ----
	PrintSeparator("Out of memory");
	{
		LinearAllocator allocator(sizeof(int) * 2);

		int* a = allocator.Allocate<int>();
		int* b = allocator.Allocate<int>();
		int* c = allocator.Allocate<int>(); // should be nullptr

		std::cout << "a: " << (a ? "allocated" : "nullptr") << "\n";
		std::cout << "b: " << (b ? "allocated" : "nullptr") << "\n";
		std::cout << "c: " << (c ? "nullptr (correct — out of memory)" : "wrongly allocated") << "\n";
	}

	// ---- exact fit ----
	PrintSeparator("Exact fit — allocator full to the byte");
	{
		LinearAllocator allocator(sizeof(int) * 3);

		int* a = allocator.Allocate<int>();
		int* b = allocator.Allocate<int>();
		int* c = allocator.Allocate<int>(); // should fit exactly
		int* d = allocator.Allocate<int>(); // should be nullptr

		std::cout << "a: " << (a ? "allocated" : "nullptr") << "\n";
		std::cout << "b: " << (b ? "allocated" : "nullptr") << "\n";
		std::cout << "c: " << (c ? "allocated (correct — exact fit)" : "nullptr") << "\n";
		std::cout << "d: " << (d ? "wrongly allocated" : "nullptr (correct — breach blocked)") << "\n";
	}

	// ---- memory breach attempt via reallocate ----
	PrintSeparator("Memory breach via Reallocate (should fail)");
	{
		LinearAllocator allocator(sizeof(int) * 5);

		int* arr      = allocator.Allocate<int>();
		int* expanded = allocator.Reallocate<int>(arr, 100); // way too big

		std::cout << "oversize realloc: " << (expanded == nullptr ? "nullptr (correct — breach blocked)" : "BREACH — something is wrong") << "\n";
	}

	// ---- reallocate non-last (should fail) ----
	PrintSeparator("Reallocate non-last allocation (should fail)");
	{
		LinearAllocator allocator(sizeof(int) * 4);

		int* a = allocator.Allocate<int>();
		int* b = allocator.Allocate<int>();

		int* result = allocator.Reallocate<int>(a, 2); // a is not last

		std::cout << "realloc non-last: " << (result == nullptr ? "nullptr (correct)" : "wrongly succeeded") << "\n";
	}

	// ---- reallocate shrink to zero then reuse ----
	PrintSeparator("Reallocate shrink to zero then reuse");
	{
		LinearAllocator allocator(sizeof(int) * 10);

		int* arr      = allocator.Allocate<int>();
		arr           = allocator.Reallocate<int>(arr, 10);
		int* shrunk   = allocator.Reallocate<int>(arr, 0);
		int* fresh    = allocator.Allocate<int>();

		std::cout << "arr at:    " << static_cast<void*>(arr)    << "\n";
		std::cout << "shrunk at: " << static_cast<void*>(shrunk) << "\n";
		std::cout << "fresh at:  " << static_cast<void*>(fresh)  << " (should match arr)\n";
	}

	// ---- reset and reuse ----
	PrintSeparator("Reset and reuse");
	{
		LinearAllocator allocator(sizeof(double) * 4);

		double* a = allocator.Allocate<double>();
		double* b = allocator.Allocate<double>();
		*a = 1.1; *b = 2.2;

		std::cout << "before reset — a: " << *a << " b: " << *b << "\n";
		std::cout << "a at: " << static_cast<void*>(a) << "\n";

		allocator.Reset();

		double* c = allocator.Allocate<double>();
		double* d = allocator.Allocate<double>();
		*c = 9.9; *d = 8.8;

		std::cout << "after reset  — c: " << *c << " d: " << *d << "\n";
		std::cout << "c at: " << static_cast<void*>(c) << " (should match a)\n";
	}

	// ---- zero size allocator ----
	PrintSeparator("Zero size allocator");
	{
		LinearAllocator allocator(0);

		int* a = allocator.Allocate<int>(); // should be nullptr immediately

		std::cout << "alloc from empty: " << (a == nullptr ? "nullptr (correct)" : "wrongly allocated") << "\n";
	}

	// ---- single byte allocator ----
	PrintSeparator("Single byte allocator");
	{
		LinearAllocator allocator(1);

		char*  a = allocator.Allocate<char>(); // fits — char is 1 byte
		char*  b = allocator.Allocate<char>(); // should be nullptr

		std::cout << "char a: " << (a ? "allocated" : "nullptr") << "\n";
		std::cout << "char b: " << (b ? "wrongly allocated" : "nullptr (correct)") << "\n";
	}

	return 0;
}
