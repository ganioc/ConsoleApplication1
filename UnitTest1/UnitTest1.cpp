#include "pch.h"
#include "CppUnitTest.h"
#include "../ConsoleApplication1/compute.h"

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
	};
}
