#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>

#include <fxd/fxd.hpp>


#include "printer.hpp"


using std::cout;
using std::endl;




//using F = fxd::fixed<10, 54>;
using F = fxd::fixed<10, 22>;
using Sat = fxd::safe::saturate;

int main()
{
    F a = std::numeric_limits<F>::max();
    SHOW(a);

    F b = Sat::negate(a);
    SHOW(b);

    F c = std::numeric_limits<F>::epsilon();
    SHOW(c);

    F d = Sat::sub(a, b);
    SHOW(d);

    F e = Sat::negate(d);
    SHOW(e);

    cout << endl;

    F f = -1;
    SHOW(f);

    F g = 500;
    SHOW(g);

    F h = Sat::mul(f, g);
    SHOW(h);

}
