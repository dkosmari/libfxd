#ifndef PRINTER_HPP
#define PRINTER_HPP


#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

#include <fxd/fxd.hpp>


template<typename T>
struct Hex {
    const T value;
};


template<typename T>
Hex<T>
hex(T val)
    noexcept
{
    return {val};
}



template<typename T>
std::ostream&
operator <<(std::ostream& out,
           const Hex<T>& x)
{
    auto flags = out.flags();
    out << std::hex;

    T mask = 0xf;
    for (int i = sizeof(T) * 2 - 1; i >= 0; --i) {
        unsigned y = (x.value >> (i*4)) & mask;
        out << y;
    }

    out.flags(flags);
    return out;
}



template<typename T>
struct Bin {
    const T value;
};


template<typename T>
Bin<T>
bin(T val)
    noexcept
{
    return {val};
}


template<typename T>
std::ostream&
operator <<(std::ostream& out,
           const Bin<T>& x)
{
    constexpr int bits = 8 * sizeof(T);

    for (int i = bits - 1; i >= 0; --i)
        out << ((x.value & (T{1} << i)) ? '1' : '0');

    return out;
}


template<fxd::fixed_point F>
void
print(F f)
{
    constexpr int p = 5 + std::numeric_limits<F>::max_digits10;
    constexpr int w = std::ceil(std::log10(2) * F::int_bits)
                  + p + 3;

    auto old_flags = std::cout.flags();
    auto old_prec = std::cout.precision();

    std::cout << std::fixed
              << std::setprecision(p)
              << std::setw(w)
              << f
              << "  "
              << hex(f.raw_value)
              << "  "
              << bin(f.raw_value);

    std::cout.precision(old_prec);
    std::cout.flags(old_flags);
}


#define SHOW(x)                                 \
    do {                                        \
        std::cout << setw(4) << #x << " = ";    \
        print(#x);                              \
        std::cout << '\n';                      \
    } while (false)


#endif
