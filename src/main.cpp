#include "../include/LinearAllocator.h"
#include <exception>
#include <iostream>

int main()
{
	std::cout << "Starting allocator test...\n";

	try {
		LinearAllocator allocator(sizeof(int) * 1000000);

		std::string* s = allocator.Allocate<std::string>("hello");
		std::cout << *s << "\n";

		int* arrInt = allocator.AllocateArray<int>(10, {1,2,3,4,5,6,7,323,114});

		for(int i = 0; i<10; i++){
			std::cout<< arrInt[i]<< std::endl;
		}
		std::cout << "Array i 4: " << arrInt[4] << "\n";

		std::cout << "Successfully allocated and wrote to memory!\n";

	} catch (const std::exception& e) {
		std::cout << "Exception caught: " << e.what() << "\n";
	}

	return 0;
}
