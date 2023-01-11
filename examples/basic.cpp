/*
#include <iostream>
#include <iomanip>
#include <numeric>
*/

#include <fxd/fxd.hpp>
#include <fxd/limits.hpp>

#if 0

using std::cout;
using std::endl;


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
    auto flags = cout.flags();
    auto fill = cout.fill();

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

#endif



int main()
{
    std::uint16_t x = ~0;
    auto y = fxd::utils::shift::shr(x, 20);
    return y;
}
