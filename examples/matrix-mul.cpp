#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <cfenv>

#include <fxd/fxd.hpp>


template<typename T>
struct Matrix {

    unsigned rows, columns;
    std::vector<T> data;

    Matrix(unsigned r, unsigned c) :
        rows{r},
        columns{c},
        data(rows * columns)
    {}


    template<typename U>
    Matrix(const Matrix<U>& other) :
        rows{other.rows},
        columns{other.columns},
        data(rows * columns)
    {
        for (std::size_t i = 0; i < data.size(); ++i)
            data[i] = static_cast<T>(other.data[i]);
    }


    T&
    at(unsigned row,
       unsigned col)
        noexcept
    {
        return data[row * columns + col];
    }


    T
    at(unsigned row,
       unsigned col)
        const noexcept
    {
        return data[row * columns + col];
    }


    template<typename U>
    Matrix&
    operator =(const Matrix<U>& other)
    {
        if (rows != other.rows || columns != other.columns)
            throw std::domain_error{"invalid dimmensions in assignment"};

        for (std::size_t i = 0; i < data.size(); ++i)
            data[i] = static_cast<T>(other.data[i]);

        return *this;
    }

};


template<typename T>
Matrix<T>
operator *(const Matrix<T>& __restrict__ a,
           const Matrix<T>& __restrict__ b)
{
    Matrix<T> c(a.rows, b.columns);
    if (a.columns != b.rows)
        throw std::domain_error{"matrix sizes mismatch in multiplication"};

    for (unsigned row = 0; row < a.rows; ++row)
        for (unsigned k = 0; k < a.columns; ++k)
            for (unsigned col = 0; col < b.columns; ++col)
                c.at(row, col) += a.at(row, k) * b.at(k, col);

    return c;
}


int main()
{
    // make floating-point rounding match fixed-point rounding
    std::fesetround(FE_TOWARDZERO);

    using Fxd = fxd::fixed<8, 24>;

    std::random_device dev;
    std::mt19937 eng{dev()};
    fxd::uniform_real_distribution<Fxd> dist{-1, 1};


    using mat_fxd = Matrix<Fxd>;
    using mat_flt = Matrix<double>;

    mat_fxd fxa(10, 20);
    mat_fxd fxb(20, 30);

    for (auto& elem : fxa.data)
        elem = dist(eng);

    for (auto& elem : fxb.data)
        elem = dist(eng);

    mat_flt fla = fxa;
    mat_flt flb = fxb;


    auto fxc = fxa * fxb;
    auto flc = fla * flb;

    double error = 0;
    for (unsigned i = 0; i < fxc.rows; ++i)
        for (unsigned j = 0; j < fxc.columns; ++j)
            error += std::abs(to_float(fxc.at(i, j)) - flc.at(i, j));

    std::cout << "Error: " << error << std::endl;

}
