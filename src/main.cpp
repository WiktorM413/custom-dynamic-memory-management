#include "../include/LinearAllocator.h"
#include <exception>
#include <iostream>

int main()
{
	std::cout << "Starting allocator test...\n";

	try {
		LinearAllocator allocator(sizeof(int) * 1000000020);

		int* inttt = allocator.Allocate<int>(3);

		int* inttt2 = allocator.Reallocate<int>(inttt, 1000000000);

		for(int i = 0; i < 1000000000; i++){
			inttt2[i] = i;
		}

		std::cout << "Successfully allocated and wrote to memory!\n";

	} catch (const std::exception& e) {
		std::cout << "Exception caught: " << e.what() << "\n";
	}

	return 0;
}
