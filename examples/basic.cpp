#include <iostream>
#include <iomanip>
#include <numeric>


#include <fxd/fxd.hpp>
#include <fxd/limits.hpp>


int main()
{
    using F = fxd::fixed<24, 40>;

    F a = 5;
    std::cout << "a = " << a << std::endl;

    std::cout << "the raw value of a is " << a.raw_value << std::endl;


}
