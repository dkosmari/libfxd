#include <iostream>
#include <iomanip>
#include <cfenv>

#include <fxd/fxd.hpp>

#include "printer.hpp"

// Local Variables:
// rmsbolt-command: "clang++ -std=c++20 -O3 -march=native -I/home/danielko/src/libfxd/include"
// rmsbolt-disassemble: nil
// End:

using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::cin;


using Fxd = fxd::fixed<32, 32>;



namespace local {

    template<fxd::unsigned_fixed_point Fxd>
    requires (Fxd::int_bits > 0 && Fxd::frac_bits >= 0)
    Fxd
    sqrt_bin(Fxd x)
        noexcept
    {
        if (!x)
            return x;

        using Raw = typename Fxd::raw_type;
        using Lim = std::numeric_limits<Fxd>;

        Fxd r = 0;
        Fxd e = x;
        const Fxd one = 1;
        const Raw rone = one.raw_value;

        const int ib = ilogb(x) / 2;

#if 0
        for (int b = ib; b >= Lim::min_bit / 2; --b) {
            Fxd d = fxd::ldexp(r, b + 1) + fxd::ldexp(one, 2 * b);
            if (d <= e) {
                e -= d;
                r += ldexp(one, b);
                if (!e)
                    break;
            }
        }
#else
        for (int b = ib; b >= 0; --b) {
            Raw rd = (r.raw_value << (b + 1)) + (rone << (2 * b));
            Fxd d = Fxd::from_raw(rd);
            if (d <= e) {
                e -= d;
                r += Fxd::from_raw(rone << b);
                if (!e)
                    return r;
            }
        }
        for (int b = -1; b >= Lim::min_bit / 2; --b) {
            Raw rd = (r.raw_value >> -(b + 1)) + (rone >> -(2 * b));
            Fxd d = Fxd::from_raw(rd);
            if (d <= e) {
                e -= d;
                r += Fxd::from_raw(rone >> -b);
                if (!e)
                    break;
            }
        }
#endif

        // We might still be off due to loss of fractional bits.
        // So try setting each remaining 0 bit to 1, and check the square.
#if 1
        int zeros = std::countr_zero(r.raw_value);
        for (int b = zeros - 1; b >= 0; --b) {
            Fxd next_r = Fxd::from_raw(r.raw_value | (Raw{1} << b));
            if (fxd::up::mul(next_r, next_r) <= x)
                r = next_r;
        }
#endif

        return r;
    }


    template<fxd::signed_fixed_point Fxd>
    requires (Fxd::int_bits > 1 && Fxd::frac_bits >= 0)
    Fxd
    sqrt_bin(Fxd x)
        noexcept
    {
        if (x < 0) {
            errno = EDOM;
            return 0;
        }
        auto ux = fxd::fixed_cast<fxd::make_unsigned_t<Fxd>>(x);
        auto r = local::sqrt_bin(ux);
        return fxd::fixed_cast<Fxd>(r);
    }

}




template<fxd::fixed_point F>
void
test(F x)
{
    std::fesetround(FE_TOWARDZERO);

    cout << "testing " << x << ":\n";

    F a = std::sqrt(to_float(x));
    cout << "    std::sqrt =     ";
    print(a);
    cout << '\n';

    F b = fxd::sqrt(x);
    cout << "    fxd::sqrt =     ";
    print(b);
    if (b - a)
        cout << "  diff = " << (b - a);
    cout << '\n';

    F c = local::sqrt_bin(x);
    cout << "    fxd::sqrt_bin = ";
    print(c);
    if  (c - a)
        cout << "  diff = " << (c - a);
    cout << '\n';

    cout << endl;
}


int main()
{

    for (auto s : {
            0.0, 1.0, 0.5, 0.25, 0.0625,
            1.5, 2.0, 3.0,
            9.0, 16.0, 25.0, 100.0, 121.0
        }) {
        test<Fxd>(s);
    }

    Fxd val;
    while ((cout << "Input a number (between 0 and "
            << std::setprecision(std::numeric_limits<Fxd>::max_digits10)
            << std::numeric_limits<Fxd>::max() << "): ") && (cin >> val))
        test(val);
    cout << endl;
}
