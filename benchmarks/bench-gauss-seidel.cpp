#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <limits>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <fxd/fxd.hpp>

#include "matrix.hpp"


std::random_device rnd_dev;
std::seed_seq rnd_seq{rnd_dev(), rnd_dev(), rnd_dev(), rnd_dev()};
std::mt19937 rnd_eng{rnd_seq};


template<typename T>
Matrix<T>
make_random_mat(size_t rows,
                size_t cols)
{
    Matrix<T> m{rows, cols};

    std::uniform_real_distribution<T> dist{-2, 2};

    for (size_t r = 0; r < m.rows(); ++r)
        for (size_t c = 0; c < m.cols(); ++c)
            m(r, c) = dist(rnd_eng);

    return m;
}


template<typename T>
std::vector<Matrix<T>>
make_random_mat_vec(size_t size,
                    size_t rows,
                    size_t cols)
{
    std::vector<Matrix<T>> result;
    result.reserve(size);

    for (size_t i = 0; i < size; ++i)
        result.push_back(make_random_mat<T>(rows, cols));

    return result;
}


// solve ax = b, returns error
template<typename T>
T
solve_gs(const Matrix<T>& a,
         const Matrix<T>& b)
{
    assert(a.rows() == b.rows());
    assert(a.rows() == a.cols());

    Matrix<T> x{a.cols(), 1};

    const std::size_t max_iterations = 100;
    for (std::size_t it = 0; it < max_iterations; ++it) {

        for (std::size_t r = 0; r < a.rows(); ++r) {

            const T rx = (a.row(r) * x)(0, 0);
            const T dx = b(r, 0) - rx;
            x(r, 0) += dx / a(r, r);

        }

    }

    auto diff = a * x - b;
    T error = 0;
    for (std::size_t r = 0; r < diff.rows(); ++r)
        error += diff(r, 0) * diff(r, 0);

    return error;
}


template<typename To,
         typename From>
std::vector<Matrix<To>>
convert(const std::vector<Matrix<From>>& src)
{
    return std::vector<Matrix<To>>(src.begin(), src.end());
}


const std::size_t mat_size = 8;

template<typename T>
std::vector<Matrix<T>> mats_A = convert<T>(mats_A<float>);

template<>
std::vector<Matrix<float>> mats_A<float> = make_random_mat_vec<float>(100, mat_size, mat_size);


template<typename T>
std::vector<Matrix<T>> mats_b = convert<T>(mats_b<float>);

template<>
std::vector<Matrix<float>> mats_b<float> = make_random_mat_vec<float>(100, mat_size, 1);


template<typename T>
void
test_solve_gs(benchmark::State& state)
{
    for (auto _ : state) {
        T error = 0;
        for (std::size_t i = 0; i < mats_A<T>.size(); ++i)
            error = solve_gs(mats_A<T>[i], mats_b<T>[i]);
        benchmark::DoNotOptimize(error);
    }
}


BENCHMARK(test_solve_gs<float>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_solve_gs<double>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_solve_gs<__float80>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_solve_gs<fxd::fixed<16, 16>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_solve_gs<fxd::fixed<32, 32>>)->Unit(benchmark::kMillisecond);
