// GBenchmark.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <iostream>

#pragma comment (lib , "Shlwapi.lib")

#include <benchmark/benchmark.h>

namespace bm = benchmark;

static void i32_addition(bm::State& state) {
	int32_t a = 110, b = 30, c;
	for (auto _ : state) {
		c = a + b;
	}
}

BENCHMARK(i32_addition);

BENCHMARK_MAIN();

//int main()
//{
//    std::cout << "Hello Benchmark!\n";
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
