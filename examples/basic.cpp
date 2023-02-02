#include <iostream>

#include <fxd/fxd.hpp> // include everything

int main()
{
    using F = fxd::fixed<20, 12>;

    F a = 5;   // construct from integer
    F b = 2.5; // construct from floating point
    F c;       // uninitialized

    std::cout << "Enter a value: ";
    std::cin >> c;

    F d = (a + b + c) / 3;

    std::cout << "Average: " << d << std::endl;
    std::cout << "which has raw value: " << d.raw_value << std::endl;
}
