#include <iostream>
#include <iterator>
#include "../include/LinearAllocator.h"

int main()
{
	LinearAllocator allocator(sizeof(int) * 20);

	int* testArray = reinterpret_cast<int*>(allocator.Allocate<int[10]>());


	if( testArray != nullptr){
		testArray[0] = 14;
		std::cout << testArray[0] << "\n";
		int* reallocatedArray = allocator.Reallocate<int>(testArray, 20);
		reallocatedArray[19] = 1;
		std::cout << reallocatedArray[19] << "\n";

		allocator.Reset();
		float* testFloat = reinterpret_cast<float*>(allocator.Allocate<float>());
		*testFloat = 1.21;
		std::cout << *testFloat << "\n";

		//garbage value
		std::cout << reallocatedArray[0] << "\n";
	}
	else {
		std::cout<< "allocation error";
	}

	return 0;
}
