// GBenchmark.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <iostream>

#pragma comment (lib , "Shlwapi.lib")

#include <benchmark/benchmark.h>
#include <stdlib.h>

namespace bm = benchmark;

static void i32_addition(bm::State& state) {
	int32_t a = 110, b = 30, c;
	for (auto _ : state) {
		c = a + b;
	}
}

static void BM_malloc(benchmark::State& state) {
	constexpr size_t size = 1024;
	for (auto _ : state) {
		void* p = malloc(size);
		benchmark::DoNotOptimize(p);
		free(p);
	}
	state.SetItemsProcessed(state.iterations());
}

static void BM_increment(benchmark::State& state) {
	size_t i = 0;
	for (auto _ : state) {
		++i;
		benchmark::DoNotOptimize(i);
	}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(i32_addition);
BENCHMARK(BM_malloc);
BENCHMARK(BM_increment);

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)

#define REPEAT(x)  REPEAT32(x)

static void BM_increment32(benchmark::State& state) {
	size_t i = 0;
	for (auto _ : state) {
		REPEAT(
			++i;
		benchmark::DoNotOptimize(i);
		);
	}
	state.SetItemsProcessed(32 * state.iterations());
}
BENCHMARK(BM_increment32);


BENCHMARK_MAIN();

//int main()
//{
//    std::cout << "Hello Benchmark!\n";
//}

