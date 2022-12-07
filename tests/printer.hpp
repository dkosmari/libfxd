#ifndef PRINTER_HPP
#define PRINTER_HPP


#include <iostream>
#include <iomanip>
#include <string>

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
    auto fill = out.fill();

    out << std::hex
        << std::setfill('0')
        << std::setw(sizeof(T)*2)
        << x.value;

    out.flags(flags);
    out.fill(fill);
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
    using U = std::make_unsigned_t<T>;

    constexpr int bits = 8 * sizeof(U);

    for (U mask = U{1} << (bits - 1); mask; mask >>= 1)
        out << (x.value & mask ? '1' : '0');

    return out;
}


template<fxd::fixed_point F>
void
print(const std::string& name,
      const F& f)
{
    std::cout << name
              << " = "
              << std::fixed
              << std::setprecision(std::numeric_limits<F>::digits10)
              << std::setw(std::ceil(std::log10(2) * F::int_bits)
                           + std::numeric_limits<F>::digits10
                           + 1)
              << f
              << "  "
              << hex(f.raw_value)
              << "  "
              << bin(f.raw_value)
              << std::endl;
}


#define SHOW(x) \
    print(#x, x)


#endif
