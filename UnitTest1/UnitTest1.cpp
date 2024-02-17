#include "pch.h"
#include "CppUnitTest.h"
#include "../ConsoleApplication1/compute.h"

#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(ConsoleApplication1)
	{
	public:
		TEST_METHOD(TESTAddTwo)
		{
			Assert::AreEqual(add_two(10), 12);
		}
		TEST_METHOD(TESTVector) {
			std::vector<int> v(10);

			Assert::AreEqual(10, (int)(v.size()));
			Assert::AreEqual(10, (int)v.capacity());
		}
	};
}
