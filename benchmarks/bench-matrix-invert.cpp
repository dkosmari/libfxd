#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <limits>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <fxd/fxd.hpp>

#include "matrix.hpp"


using std::size_t;
using std::abs;


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


template<fxd::fixed_point Fxd>
Matrix<Fxd>
make_random_mat(size_t rows,
                size_t cols)
{
    Matrix<Fxd> m{rows, cols};

    fxd::uniform_real_distribution<Fxd> dist{-2, 2};

    for (size_t r = 0; r < m.rows(); ++r)
        for (size_t c = 0; c < m.cols(); ++c)
            m(r, c) = dist(rnd_eng);

    return m;
}


template<typename T>
std::vector<Matrix<T>>
make_random_mat_vec(size_t size)
{
    std::vector<Matrix<T>> result;
    result.reserve(size);

    for (size_t i = 0; i < size; ++i)
        result.push_back(make_random_mat<T>(4, 4));

    return result;
}


template<typename T>
void
swap_rows(Matrix<T>& m,
          size_t a,
          size_t b)
{
    if (a == b)
        return;
    for (size_t i = 0; i < m.cols(); ++i)
        std::swap(m(a, i), m(b, i));
}


template<typename T>
size_t
find_row(const Matrix<T>& m,
         size_t col)
{
    assert(col < m.rows());

#if 1
    // use partial pivoting, locate the row that has maximum magnitude in the column
    size_t max_row = col;
    for (size_t row = col + 1; row < m.rows(); ++row)
        if (abs(m(row, col)) > abs(m(max_row, col)))
            max_row = row;
    return max_row;
#else
    return col;
#endif
}


template<typename T>
T
invert(const Matrix<T>& m)
{
    assert(m.rows() == m.cols());
    assert(m.rows() > 0);

    Matrix<T> em(m.rows(), 2 * m.cols());
    for (size_t r = 0; r < m.rows(); ++r)
        for (size_t c = 0; c < m.cols(); ++c)
            em(r, c) = m(r, c);
    for (size_t i = 0; i < em.rows(); ++i)
        em(i, em.rows() + i) = 1;

    const T eps = std::numeric_limits<T>::epsilon();

    // Gaussian elimination
    for (size_t r = 0; r < em.rows(); ++r) {

        if (size_t best = find_row(em, r); best != r)
            swap_rows(em, r, best);

        const T k = em(r, r);

        if (abs(k) <= eps)
            return static_cast<T>(-1);

        // cancel out all rows below
        for (size_t rr = r + 1; rr < em.rows(); ++rr) {
            const T kk = em(rr, r);
            em(rr, r) = 0;
            for (size_t c = r + 1; c < em.cols(); ++c)
                em(rr, c) -= em(r, c) * kk / k;
        }
    }

    // Reduced Row Echelon Form
    for (size_t r = em.rows() - 1; r + 1 > 0; --r) {
        // cancel out all rows above
        const T lead_r = em(r, r);
        for (size_t rr = r - 1; rr + 1 > 0; --rr) {
            const T lead_rr = em(rr, r);
            // em(rr) -= lead_rr / lead_r * em(r)
            em(rr, r) = 0;
            for (size_t c = r + 1; c < em.cols(); ++c)
                em(rr, c) -= lead_rr / lead_r * em(r, c);
        }

        // divide row r by lead_r
        em(r, r) = 1;
        for (size_t c = r + 1; c < em.cols(); ++c)
            em(r, c) /= lead_r;
    }

    Matrix<T> im{m.rows(), m.cols()};
    for (size_t r = 0; r < im.rows(); ++r)
        for (size_t c = 0; c < im.cols(); ++c)
            im(r, c) = em(r, im.cols() + c);

    auto identity = m * im;

    T error = 0;
    for (size_t r = 0; r < identity.rows(); ++r)
        for (size_t c = 0; c < identity.cols(); ++c) {
            T e = (r == c ? 1 : 0) - identity(r, c);
            error += abs(e);
        }
    return error;
}



template<typename To,
         typename From>
std::vector<Matrix<To>>
convert(const std::vector<Matrix<From>>& src)
{
    return std::vector<Matrix<To>>(src.begin(), src.end());
}


template<typename T>
std::vector<Matrix<T>> mats = convert<T>(mats<float>);

template<>
std::vector<Matrix<float>> mats<float> = make_random_mat_vec<float>(1000);


template<typename T>
void
test_inv(benchmark::State& state)
{
    for (auto _ : state) {
        T error = 0;
        for (const auto& m : mats<T>)
            error += invert(m);
        benchmark::DoNotOptimize(error);
    }
}


BENCHMARK(test_inv<float>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_inv<double>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_inv<fxd::fixed<12, 4>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_inv<fxd::fixed<12, 20>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_inv<fxd::fixed<16, 16>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_inv<fxd::fixed<20, 44>>)->Unit(benchmark::kMillisecond);
BENCHMARK(test_inv<fxd::fixed<32, 32>>)->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();
