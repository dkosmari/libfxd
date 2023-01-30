#include <iostream>

#include <fxd/fxd.hpp>

int main()
{
    using F = fxd::fixed<20, 12>;

    F a = 5;
    a = 6;
    F b = 2.5;
    b = 3.5;
    F c;

    std::cout << "Enter a value: ";
    std::cin >> c;

    F d = (a + b + c) / 3;

    std::cout << "Average: " << d << std::endl;
    std::cout << "which has raw value: " << d.raw_value << std::endl;
}
