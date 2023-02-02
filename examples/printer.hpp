#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <algorithm>
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
std::string
bin(T val)
    noexcept
{
    constexpr int w = 8 * sizeof(T);
    std::string result;
    for (int i = 0; i < w; ++i)
        result += (val & (T{1} << (w - i - 1))) ? '1' : '0';
    return result;
}



template<fxd::fixed_point F>
void
print(F f)
{
    using namespace std::literals;
    using std::string;

    constexpr int p = 5 + std::numeric_limits<F>::max_digits10;
    const int w = std::ceil(std::log10(2) * F::int_bits)
                  + p + 3;

    auto old_flags = std::cout.flags();
    auto old_prec = std::cout.precision();

    auto rbin = bin(f.raw_value);
    auto sbin = rbin.substr(8 * sizeof(typename F::raw_type) - F::bits);
    auto bin_int = F::int_bits > 0
        ? sbin.substr(0, std::min(F::bits, F::int_bits))
        : ""s;
    auto bin_frac = F::frac_bits > 0
        ? sbin.substr(bin_int.size())
        : ""s;

    string bin_str;
    if (F::int_bits >= 0 && F::frac_bits >= 0) {
        bin_str = bin_int + "."s + bin_frac;
    } else if (F::int_bits >= 0 && F::frac_bits < 0) {
        bin_str = bin_int + string(-F::frac_bits, 'x');
    } else if (F::int_bits < 0 && F::frac_bits >= 0) {
        bin_str = "."s + string(-F::int_bits, 'x') + bin_frac;
    }


    std::cout << std::fixed
              << std::setprecision(p)
              << std::setw(w)
              << f
              << "  0x"
              << hex(f.raw_value)
              << "  "
              << "0b" << bin_str;

    std::cout.precision(old_prec);
    std::cout.flags(old_flags);
}


#define SHOW(x)                                         \
    do {                                                \
        std::cout << std::setw(4) << #x << " = ";       \
        print(x);                                       \
        std::cout << '\n';                              \
    } while (false)


#endif
