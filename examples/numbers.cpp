#include <iomanip>
#include <iostream>
#include <limits>
#include <numbers>

#include <fxd/fxd.hpp>

using std::cout;
using std::endl;


using F = fxd::fixed<4, 20>;


void
print(const char* name,
      F val)
{
    cout << std::setw(10)
         << name
         << " = "
         << val << '\n';
}


int main()
{
    cout << "std::numbers::\n";

    cout.precision(std::numeric_limits<F>::digits10);
    cout << std::fixed;

    print("e",          std::numbers::e_v<F>);
    print("egamma",     std::numbers::egamma_v<F>);
    print("inv_pi",     std::numbers::inv_pi_v<F>);
    print("inv_sqrt3",  std::numbers::inv_sqrt3_v<F>);
    print("inv_sqrtpi", std::numbers::inv_sqrtpi_v<F>);
    print("ln10",       std::numbers::ln10_v<F>);
    print("ln2",        std::numbers::ln2_v<F>);
    print("log10e",     std::numbers::log10e_v<F>);
    print("log2e",      std::numbers::log2e_v<F>);
    print("phi",        std::numbers::phi_v<F>);
    print("pi",         std::numbers::pi_v<F>);
    print("sqrt2",      std::numbers::sqrt2_v<F>);
    print("sqrt3",      std::numbers::sqrt3_v<F>);

}
