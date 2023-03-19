#include <iostream>
#include <iomanip>
#include <cfenv>

#include <fxd/fxd.hpp>

#include "printer.hpp"

using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::cin;


using Fxd = fxd::fixed<32, 32>;


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

    F c = fxd::sqrt_bin(x);
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
