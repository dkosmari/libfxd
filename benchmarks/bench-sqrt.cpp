#include <cmath>

#include <benchmark/benchmark.h>

#include <fxd/fxd.hpp>

#include "make-random-vec.hpp"


using Fxd = fxd::ufixed<10, 22>;
using Vec = std::vector<Fxd>;


const auto numbers = make_random_vec<float>(100'000, 1, 2);


template<typename T>
void
test_sqrt(benchmark::State& state)
{
    using std::sqrt;
    using fxd::sqrt;

    for (auto _ : state) {
        T total = 0;
        for (const auto x : numbers)
            total += sqrt(static_cast<T>(Fxd{x}));
        benchmark::DoNotOptimize(total);
    }
}


BENCHMARK(test_sqrt<float>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt<double>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt<Fxd>)->Unit(benchmark::kMillisecond);
