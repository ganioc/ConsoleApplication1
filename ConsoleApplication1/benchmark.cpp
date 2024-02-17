
#pragma comment (lib , "Shlwapi.lib")

#include <benchmark/benchmark.h>

namespace bm = benchmark;

static void i32_addition(bm::State& state) {
	int32_t a=110, b=30, c;
	for (auto _ : state) {
		c = a + b;
	}
}

BENCHMARK(i32_addition);

//BENCHMARK_MAIN();
