#include "../include/LinearAllocator.h"
#include <exception>
#include <iostream>

int main()
{
	std::cout << "Starting allocator test...\n";

	try {
		FrameAllocator allocator(sizeof(int) * 1000000, 64);

		TightAllocator allocator2(sizeof(int) * 1000000, 16);

	} catch (const std::exception& e) {
		std::cout << "Exception caught: " << e.what() << "\n";
	}

	return 0;
}
