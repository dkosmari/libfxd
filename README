libfxd - a fixed-point library for C++
======================================

libfxd is a C++20 headers-only library that implements fixed-point types as a templated
class. It's licensed under the Apache Public License 2.0 (see the file `COPYING.APL`).


Installation
------------

Since it's a headers-only library, no compilation is necessary; the compiler just needs to
locate the headers using an option like `-I /path/to/libfxd/include`.

To install the headers, use the standard Automake procedure:

    ./configure --prefix=/usr
    make
    sudo make install


See the `INSTALL` file and/or run `./configure --help` for more details.

If the source code was checked out from the repository, you may need to first run the
`bootstrap` script to create the `configure` script.


Usage
-----

Sample program:

~~~.cpp
#include <iostream>

#include <fxd/fxd.hpp>

int main()
{
    using F = fxd::fixed<20, 12>;

    F a = 5;
    F b = 2.5;
    F c;

    std::cout << "Enter a value: ";
    std::cin >> c;

    F d = (a + b + c) / 3;

    std::cout << "Average: " << d << std::endl;
    std::cout << "which has raw value: " << d.raw_value << std::endl;
}
~~~

Documentation is available in the `doc` directory in .md text files (using Markdown
syntax.) If Doxygen is available, it will be automatically built inside `doc/html`.

Examples can be found in the `examples` directory.
