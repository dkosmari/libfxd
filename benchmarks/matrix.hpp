#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>
#include <concepts>
#include <ostream>
#include <iomanip>


template<typename T>
class Matrix {

    std::size_t num_rows = 0;
    std::size_t num_cols = 0;
    std::vector<T> data = std::vector<T>(num_rows * num_cols);

public:

    constexpr Matrix() noexcept = default;

    template<std::convertible_to<T> U>
    Matrix(const Matrix<U>& other) :
        Matrix{other.rows(), other.cols()}
    {
        for (std::size_t r = 0; r < num_rows; ++r)
            for (std::size_t c = 0; c < num_cols; ++c)
                (*this)(r, c) = other(r, c);
    }

    Matrix(std::size_t r,
           std::size_t c) :
        num_rows{r},
        num_cols{c}
    {}

    template<std::convertible_to<T> U>
    Matrix&
    operator =(const Matrix<U>& other)
    {
        if constexpr (std::same_as<T, U>)
            if (this == &other)
                return *this;

        resize(other.rows(), other.cols());
        for (std::size_t r = 0; r < num_rows; ++r)
            for (std::size_t c = 0; c < num_cols; ++c)
                (*this)(r, c) = other(r, c);
        return *this;
    }

    void
    resize(std::size_t r,
           std::size_t c)
    {
        num_rows = r;
        num_cols = c;
        data.resize(num_rows * num_cols);
    }

    constexpr
    T
    operator ()(std::size_t r,
                std::size_t c)
        const noexcept
    {
        return data[c + r * num_cols];
    }

    constexpr
    T&
    operator ()(std::size_t r,
                std::size_t c)
        noexcept
    {
        return data[c + r * num_cols];
    }

    constexpr
    std::size_t
    rows()
        const noexcept
    {
        return num_rows;
    }

    constexpr
    std::size_t
    cols()
        const noexcept
    {
        return num_cols;
    }

};


template<typename T>
Matrix<T>
operator *(const Matrix<T>& a,
           const Matrix<T>& b)
{
    Matrix<T> c{a.rows(), b.cols()};

    for (std::size_t row = 0; row < c.rows(); ++row)
        for (std::size_t i = 0; i < a.cols(); ++i)
            for (std::size_t col = 0; col < c.cols(); ++col)
                c(row, col) += a(row, i) * b(i, col);

    return c;
}


template<typename T>
std::ostream&
operator <<(std::ostream& out,
            const Matrix<T>& m)
{
    auto flags = out.flags();
    auto prec = out.precision();

    out << std::setprecision(4) << std::fixed;
    for (std::size_t r = 0; r < m.rows(); ++r) {
        for (std::size_t c = 0; c < m.cols(); ++c) {
            if (c)
                out << "  ";
            out << std::setw(8) << m(r, c);
        }
        out << '\n';
    }

    out.precision(prec);
    out.flags(flags);
    return out;
}


#endif
