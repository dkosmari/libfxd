#include <iostream>
#include <iomanip>
#include <numeric>

#include <fxd/fxd.hpp>
#include <fxd/limits.hpp>


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





int main()
{
#if 0
    using F = fxd::fixed<16, 16>;

    cout << std::hex
         << std::setprecision(std::numeric_limits<F>::max_digits10)
         << std::fixed;

    F a = std::numeric_limits<F>::max();
    cout << "a: " << a << "  " << unsigned(a.raw_value) << endl;

    F b = std::numeric_limits<F>::epsilon();
    cout << "b: " << b << "  " << unsigned(b.raw_value) << endl;


    // auto e = std::numbers::e_v<fixed<16, 16>>;
    // cout << "e: " << e << endl;


    auto c = a * b;
    cout << "c: " << c << "  " << unsigned(c.raw_value) << endl;


    auto [d, e] = fxd::utils::full_mult(a.raw_value, b.raw_value);
    cout << "d = " << d << endl;
    cout << "e = " << e << endl;


#endif

#if 1
    using std::uint16_t;

    uint16_t x = 10;
    uint16_t y = 5;

    cout << " x = " << hex(x) << "  " << bin(x) << endl;
    cout << " y = " << hex(y) << "  " << bin(y) << endl;

    auto [z0, z1] = fxd::utils::full_div(uint16_t{0},
                                         x,
                                         y);
    cout << "z1 = " << hex(z1) << "  " << bin(z1) << endl;
    cout << "z0 = " << hex(z0) << "  " << bin(z0) << endl;

    uint32_t xx = x << 16;
    uint32_t yy = y;
    uint32_t zz = xx / yy;
    cout << "zz = " << hex(zz) << "  " << bin(zz) << endl;


#endif

}
