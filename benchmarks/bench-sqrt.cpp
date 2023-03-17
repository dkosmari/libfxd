#include <cmath>

#include <benchmark/benchmark.h>

#include <fxd/fxd.hpp>

#include "make-random-vec.hpp"


using fxd::ufixed;

const std::vector<float> numbers_flt = make_random_vec<float>(10'000, 0, 10);


template<typename T>
const std::vector<T> numbers(numbers_flt.begin(),
                             numbers_flt.end());



template<typename T>
void
test_sqrt(benchmark::State& state)
{
    using std::sqrt;
    using fxd::sqrt;

    for (auto _ : state) {
        T total = 0;
        for (const auto x : numbers<T>)
            total += sqrt(x);
        benchmark::DoNotOptimize(total);
    }
}

template<typename T>
void
test_sqrt_bin(benchmark::State& state)
{
    for (auto _ : state) {
        T total = 0;
        for (const auto x : numbers<T>)
            total += fxd::sqrt_bin(x);
        benchmark::DoNotOptimize(total);
    }
}


// BENCHMARK(test_sqrt<float>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<double>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<long double>)->Unit(benchmark::kMillisecond);

// BENCHMARK(test_sqrt<ufixed<8, 8>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<4, 12>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<4, 28>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<16, 16>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<32, 0>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<16, 48>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<32, 32>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<48, 16>>)->Unit(benchmark::kMillisecond);
// BENCHMARK(test_sqrt<ufixed<64, 0>>)->Unit(benchmark::kMillisecond);

BENCHMARK(test_sqrt_bin<ufixed<8, 8>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<4, 12>>)->Unit(benchmark::kMillisecond);

BENCHMARK(test_sqrt_bin<ufixed<4, 28>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<6, 26>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<8, 24>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<10, 22>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<12, 20>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<14, 18>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<16, 16>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<18, 14>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<20, 12>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<22, 10>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<24, 8>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<26, 4>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<28, 2>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<32, 0>>)->Unit(benchmark::kMillisecond);

BENCHMARK(test_sqrt_bin<ufixed<16, 48>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<32, 32>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<48, 16>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_sqrt_bin<ufixed<64, 0>>)->Unit(benchmark::kMillisecond);
