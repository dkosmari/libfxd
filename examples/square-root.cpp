#include <iostream>
#include <iomanip>

#include <fxd/fxd.hpp>

#include "printer.hpp"


using std::cout;
using std::clog;
using std::endl;


const unsigned max_iterations = 30;


// Since integers don't have a square root function, fixed<> types don't have it either.


// Cheating method

template<fxd::fixed_point Fxd>
Fxd
sqrt_std(Fxd s)
{
    return std::sqrt(to_float(s));
}


// Babylonian/Heron/Newton-Raphson method

template<fxd::fixed_point Fxd>
Fxd
sqrt_nwt(Fxd s)
{
    unsigned i = 0;
    Fxd next_x = s;
    Fxd x;
    do {
        x = next_x;
        if (!x)
            return x;
        next_x = (x + (s / x)) / 2;
    } while (next_x != x && ++i < max_iterations);
    if (i >= max_iterations)
        clog << "max iterations reached" << endl;
    return x;
}


// Bakhshali method

template<fxd::fixed_point Fxd>
Fxd
sqrt_bak(Fxd s)
{
    unsigned i = 0;
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
        clog << "max iterations reached" << endl;
    return x;
}


// Newton-Raphson's reciprocal

template<fxd::fixed_point Fxd>
Fxd
sqrt_rec(Fxd s)
{
    if (!s)
        return s;
    unsigned i = 0;
    Fxd x = 1 / s;
    Fxd old_x;
    do {
        old_x = x;
        x = x * (3 - s * x * x) / 2;
    } while (old_x != x && ++i < max_iterations);
    if (i >= max_iterations)
        clog << "max iterations reached" << endl;
    return 1 / x;
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
    cout << "\n        diff = ";
    print(fdim(a, b));
    cout << '\n';

    auto c = sqrt_bak(x);
    cout << "    sqrt_bak = ";
    print(c);
    cout << "\n        diff = ";
    print(fdim(a, c));
    cout << '\n';

    auto d = sqrt_rec(x);
    cout << "    sqrt_rec = ";
    print(d);
    cout << "\n        diff = ";
    print(fdim(a, d));
    cout << '\n';

}


int main()
{
    using F = fxd::fixed<32, 32>;

    for (auto s : {0.0, 1.0, 9.0, 2.0, 1.5, 16.0, 20.0, 0.0625}){
        test<F>(s);
    }


}
