// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <benchmark/benchmark.h>

#include "compute.h"
#include "tutorial.h"

int main()
{
    std::cout << "Hello World!\n";

    std::cout << "add_two(): " << compute::add_two(10) << std::endl;

    tutorial::G g;
    g(13);
    tutorial::doSomeWork();

    tutorial::testTemplate();

    tutorial::testPrototype();

    tutorial::testSingleton();

    tutorial::testFunction();

    return 0;
}

