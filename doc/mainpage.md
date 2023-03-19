Introduction
============

libfxd is a C++20 headers-only library that implements fixed-point types as a templated
class. It's licensed under the <a href="https://spdx.org/licenses/Apache-2.0.html">Apache Public License 2.0</a>.


Quick example
-------------

@include basic.cpp


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



\example basic.cpp
\example closest-pair.cpp
\example matrix-mul.cpp
\example numbers.cpp
\example square-root.cpp
