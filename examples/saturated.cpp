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

int main()
{
    using namespace fxd::saturate;

    F a = std::numeric_limits<F>::max();
    SHOW(a);

    F b = negate(a);
    SHOW(b);

    F c = std::numeric_limits<F>::epsilon();
    SHOW(c);

    F d = sub(a, b);
    SHOW(d);

    F e = negate(d);
    SHOW(e);

    cout << endl;

    F f = -1;
    SHOW(f);

    F g = 500;
    SHOW(g);

    F h = mul(f, g);
    SHOW(h);

}
