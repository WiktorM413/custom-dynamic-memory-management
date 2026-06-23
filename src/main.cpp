#include <iostream>
#include "../include/LinearAllocator.h"

int main()
{
	LinearAllocator allocator(sizeof(int));

	int* testInt = allocator.Allocate<int>();

	if( testInt != nullptr){
		*testInt = 14;
		std::cout << *testInt;
	}
	else {
		std::cout<< "allocation error";
	}

	return 0;
}
