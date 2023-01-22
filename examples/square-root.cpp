#include <iostream>
#include <iomanip>

#include <fxd/fxd.hpp>

#include "printer.hpp"


using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::cin;


using F = fxd::fixed<20, 44>;


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


template<fxd::fixed_point Fxd>
void
test(Fxd x)
{
    cout << "testing " << x << ":\n";

    Fxd a = std::sqrt(to_float(x));
    cout << "    std::sqrt = ";
    print(a);
    cout << '\n';

    Fxd b = fxd::sqrt(x);
    cout << "    fxd::sqrt = ";
    print(b);
    cout << "\n        diff = " << (b - a);
    cout << '\n';

    Fxd c = sqrt_bak(x);
    cout << "     sqrt_bak = ";
    print(c);
    cout << "\n        diff = " << (c - a);
    cout << '\n';

    cout << endl;
}


int main()
{

    for (auto s : {0.0, 1.0, 0.5, 0.25, 0.0625,
                   1.5, 2.0, 3.0,
                   9.0, 16.0, 25.0, 100.0, 121.0}) {
        test<F>(s);
    }

    F val;
    while ((cout << "Input a number: ") && (cin >> val))
        test(val);
    cout << endl;

}
