#include "pch.h"
#include "CppUnitTest.h"
#include "../ConsoleApplication1/compute.h"

#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <typename T> 
class scoped_ptr {
public:
	explicit scoped_ptr(T* p) : p_(p) {}
	~scoped_ptr() {
		delete p_; 
	}
	T* operator->() {
		return p_;
	}
	const T* operator->() const {
		return p_;
	}
	T& operator*() {
		return *p_;
	}
	const T& operator*() const {
		return *p_;
	}
private:
	T* p_;
};

struct object_counter {
	static int count;
	static int all_count;
	object_counter() {
		++count; ++all_count;
	}
	~object_counter() {
		--count;
	}
};
int object_counter::count = 0;
int object_counter::all_count = 0;

namespace UnitTest1
{
	TEST_CLASS(ConsoleApplication1)
	{
	public:
		TEST_METHOD(TESTAddTwo)
		{
			Assert::AreEqual(compute::add_two(10), 12);
		}
		TEST_METHOD(TESTVector) {
			std::vector<int> v(10);

			Assert::AreEqual(10, (int)(v.size()));
			Assert::AreEqual(10, (int)v.capacity());
		}
		TEST_METHOD(TESTRaii) {
			object_counter::count = object_counter::all_count = 0;
			{
				scoped_ptr<object_counter>  p(new object_counter);
				Assert::AreEqual(1, object_counter::count);
				Assert::AreEqual(1, object_counter::all_count);
			}
			Assert::AreEqual(0, object_counter::count);
			Assert::AreEqual(1, object_counter::all_count);
		}
	};
}
