#include "tutorial.h"

namespace tutorial {
	void G::operator()(int i)
	{
		std::cout << "G-> " << i << std::endl;
	}
	void doSomeWork() {
		auto compare = [](int i, int j) {
			return i < j;
		};
		std::cout << compare(3, 2) << std::endl;
		std::cout << "some work" << std::endl;
	}
}