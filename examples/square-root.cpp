#include <iostream>
#include <iomanip>

#include <fxd/fxd.hpp>

#include "printer.hpp"


using std::cout;
using std::cerr;
using std::clog;
using std::endl;


using F = fxd::fixed<32, 32>;


// Since integers don't have a square root function, fixed<> types don't have it either.


// Cheating method

template<fxd::fixed_point Fxd>
Fxd
sqrt_std(Fxd s)
    noexcept
{
    return std::sqrt(to_float(s));
}


// Babylonian/Heron/Newton-Raphson method

template<fxd::fixed_point Fxd>
Fxd
sqrt_nwt(Fxd x,
         int max_iterations = Fxd::bits)
    noexcept
{
    if (!x)
        return x;

    int i = 0;
    Fxd a = x;
    Fxd old_a;
    do {
        if (++i > max_iterations)
            return a;

        Fxd b = x / a;

        if (a > b)
            std::swap(a, b); // a is always the lower bound

        old_a = a;

        a = (a + b) / 2;

        if (!a)
            return a;

    } while (old_a != a);
    return a;
}


// Bakhshali method

template<fxd::fixed_point Fxd>
Fxd
sqrt_bak(Fxd s,
         int max_iterations = Fxd::bits)
{
    int i = 0;
    Fxd x = s;
    Fxd old_x;
    do {
        if (!x)
            return x;
        old_x = x;
        Fxd a = (s - x * x) / (2 * x);
        Fxd b = x + a;
        if (!b)
            break;
        x = b - (a * a / (2 * b));
            //(x + a) - ((a * a) / (2 * (x + a)));
    } while (old_x != x && ++i < 20);
    if (i >= max_iterations)
        cerr << "max iterations reached" << endl;
    return x;
}


// Newton-Raphson's reciprocal

template<fxd::fixed_point Fxd>
Fxd
sqrt_rec(Fxd s,
         int max_iterations = Fxd::bits)
{
    if (!s)
        return s;
    int i = 0;
    Fxd x = 1 / sqrt_nwt(s, 2);
    Fxd old_x;
    do {
        old_x = x;
        Fxd x2 = x * x;
        x = x * (3 - s * x2) / 2;
        if (x <= 0)
            return s * old_x;
    } while (old_x != x && ++i < max_iterations);
    if (i >= max_iterations)
        cerr << "max iterations reached" << endl;
    return s * x;
}



template<fxd::fixed_point Fxd>
void
test(Fxd x)
{
    cout << "testing " << x << ":\n";

    auto a = sqrt_std(x);
    cout << "    sqrt_std = ";
    print(a);
    cout << '\n';

    auto b = sqrt_nwt(x);
    cout << "    sqrt_nwt = ";
    print(b);
    cout << "\n        diff = " << static_cast<long double>(b - a);
    cout << '\n';

    auto c = sqrt_bak(x);
    cout << "    sqrt_bak = ";
    print(c);
    cout << "\n        diff = " << static_cast<long double>(c - a);
    cout << '\n';

    auto d = sqrt_rec(x);
    cout << "    sqrt_rec = ";
    print(d);
    cout << "\n        diff = " << static_cast<long double>(d - a);
    cout << '\n';

    cout << endl;
}


int main()
{
    for (auto s : {0.0, 1.0, 0.5, 0.25, 0.0625,
                   9.0, 2.0, 1.5, 16.0, 25.0, 100.0, 3.0}) {
        test<F>(s);
    }

    // auto r = sqrt_rec(F{0.25});
    // cout << r << endl;


}
