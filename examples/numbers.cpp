#include <iomanip>
#include <iostream>
#include <limits>
#include <numbers>

#include <fxd/fxd.hpp>


template<fxd::fixed_point Fxd>
void
print(const char* name,
      Fxd val)
{
    std::cout << std::setw(10)
              << name
              << " = "
              << val << '\n';
}


template<fxd::fixed_point Fxd>
void
report_all()
{
    std::cout << "std::numbers::\n";
    std::cout.precision(std::numeric_limits<Fxd>::digits10);

    print("e",          std::numbers::e_v<Fxd>);
    print("egamma",     std::numbers::egamma_v<Fxd>);
    print("inv_pi",     std::numbers::inv_pi_v<Fxd>);
    print("inv_sqrt3",  std::numbers::inv_sqrt3_v<Fxd>);
    print("inv_sqrtpi", std::numbers::inv_sqrtpi_v<Fxd>);
    print("ln10",       std::numbers::ln10_v<Fxd>);
    print("ln2",        std::numbers::ln2_v<Fxd>);
    print("log10e",     std::numbers::log10e_v<Fxd>);
    print("log2e",      std::numbers::log2e_v<Fxd>);
    print("phi",        std::numbers::phi_v<Fxd>);
    print("pi",         std::numbers::pi_v<Fxd>);
    print("sqrt2",      std::numbers::sqrt2_v<Fxd>);
    print("sqrt3",      std::numbers::sqrt3_v<Fxd>);
}


int main()
{
    report_all<fxd::fixed<8, 24>>();
}
