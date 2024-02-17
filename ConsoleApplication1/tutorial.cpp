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
	void testTemplate()
	{
		int i = 5;
		double d = 7.3;

		auto load = overload(
			[](int* i) {
				std::cout << "i=" << *i << std::endl;
			},
			[](double* d) {
				std::cout << "d=" << *d << std::endl;
			}
			);
		load(&i);
		load(&d);
	}
	FactoryP::FactoryP()
	{
	}

	FactoryP::~FactoryP()
	{
	}
	A::A()
	{
	}

	A::~A()
	{
	}
	B::B()
	{
	}

	B::~B()
	{
	}
	BaseP* FactoryP::s_prototypes[] = {NULL, new A, new B};
	BaseP* FactoryP::makeBase(int choice) {
		return s_prototypes[choice]->clone();
	}
	void testPrototype() {
		BaseP* b = FactoryP::makeBase(1);
		b->doSomething();
		delete b;

		BaseP* c = FactoryP::makeBase(2);
		c->doSomething();
		delete c;
	}

	Single* Single::instance_; 
	void testSingleton() {
		std::cout << "testSingleton()\n";
		Single* s = Single::getInstance();
		delete s;
	}
}