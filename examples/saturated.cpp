#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>

#include <fxd/fxd.hpp>
#include <fxd/safe-operators.hpp>


using std::cout;
using std::endl;


template<fxd::fixed_point Fxd>
void
show(const std::string& name,
     const Fxd& f)
{
    auto flags = cout.flags();
    auto fill = cout.fill();

    cout << name << " = ";

    cout << std::fixed
         << std::setprecision(std::numeric_limits<Fxd>::max_digits10)
         << std::showpos
         << std::setw(std::numeric_limits<Fxd>::max_digits10 + 5)
         << f;

    cout.flags(flags);

    cout << "\t[ "
         << std::hex
         << std::setfill('0')
         << std::setw(fxd::utils::type_width<typename Fxd::raw_type> / 4)
         << f.raw_value
         << " ]";

    cout.flags(flags);
    cout.fill(fill);

    cout << endl;
}



//using F = fxd::fixed<10, 54>;
using F = fxd::fixed<10, 22>;

int main()
{
    F a = std::numeric_limits<F>::max();
    show("    a", a);

    a = fxd::safe::sat::negate(a);
    show("neg a", a);

    F b = std::numeric_limits<F>::epsilon();
    show("    b", b);

    F c = fxd::safe::sat::minus(a, b);
    show("a - b", c);

    c = fxd::safe::sat::negate(c);
    show("-(a-b)", c);

    cout << endl;

    F d = -1;
    show("    d", d);
    F e = 500;
    show("    e", e);
    F f = fxd::safe::sat::multiplies(d, e);
    show("d * e", f);

}
