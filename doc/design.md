Design
======

This page explains the rationale behind design choices in this library.


Parameters
----------

A fixed-point, implemented in the `fxd::fixed` templated class, is specified as:

~~~{.cpp}
fxd::fixed<Int, Frac, Raw>
~~~

Where:

  - `Int` indicates how many integral bits to use. It can be negative.
  - `Frac` indicates how many fractional bits to use. It can be negative.
  - `Raw` indicates the underlying representation. When not explicitly specified, it will
    be the smallest \b signed integer with `Int+Frac` bits.

For convenience, there's also the templated alias:

~~~{.cpp}
fxd::ufixed<Int, Frac, Raw>
~~~

The only difference is that `Raw` defaults to the smallest \b unsigned integer with
`Int+Frac` bits.

Some examples:

  - `fxd::fixed<32, 0>` behaves like `std::int32_t`.
  - `fxd::fixed<10, 0>` a 10-bit integer.
  - `fxd::fixed<33, -1>` only stores even integers.
  - `fxd::ufixed<1, 31>` only stores values in the interval \f$ [0, 2) \f$.
  - `fxd::ufixed<0, 32>` only stores values in the interval \f$ [0, 1) \f$.
  - `fxd::ufixed<-1, 33>` only stores values in the interval \f$ [0, 0.5) \f$.
  - `fxd::fixed<1, 31>` only stores values in the interval \f$ [-1, 1) \f$.
  - `fxd::fixed<0, 32>` only stores values in the interval \f$ [-0.5, 0.5) \f$.


Note how the intervals are all half-open.


Underlying Representation
-------------------------

The "raw value" of a fixed-point is always a bit-field of a native integer. For instance,
the fixed-point `fxd::fixed<4, 10>` will have its raw value defined as:

~~~{.cpp}
std::int16_t raw_value : 14;
~~~

This guarantees there will be no gratuitous precision bits nor extra range.

Whenever a bit-field matches the full size of the selected integer, compilers remove all
bit-field manipulation overhead, and the bit-field is treated like any regular integer.

But there's a limitation: the C++ language does not allow taking addresses or references
from bit-fields. So this code does not compile:

~~~{.cpp}
using F = fxd::fixed<32, 0>; // behaves like a regular std::int32_t
F x; 
std::cin >> x.raw_value; // error: can't bind bit-field to int&
~~~

But this does:

~~~{.cpp}
using F = fxd::fixed<32, 0>; // behaves like a regular std::int32_t
F::raw_type raw_x;
std::cin >> raw_x;
F x = F::from_raw(raw_x);
~~~

While bit operations (`<<`, `>>`, `&`, `|`, `^`) are not available for fixed-point types,
they can still be used directly on the `.raw_value` member.


Extended Integers
-----------------

The compiler must use the "raw type" as the underlying type for a bit-field. That excludes
user-defined types.

The library also makes heavy usage of standard concepts, type traits and
`std::numeric_limits`. To convince the standard library to accept types like `__int128` as
integral types, we often need special compiler options, like `-std=gnu++20`; otherwise,
the `std::integral` concept will not match such extended types, and `std::numeric_limits`
might not have a specialization for such a type.

Internally, the library still tries to use 128-bit arithmetic when it's available, even if
the standard library does not recognize `__int128` as an integral type. When no 128-bit
integers are not available, portable routines are used instead, with lower
performance.


Rounding Modes
--------------

The rounding mode is not part of the type. Instead, it's specified on a per-operation
basis, by specifying a rounding namespace. Available rounding namespaces are `fxd::down`,
`fxd::up` and `fxd::zero`.

For instance: to evaluate the expression \f$ x^2 - y^2 \f$ rounded down, we can write:

~~~{.cpp}
fxd::down::mul(x, x) - fxd::up::mul(y, y);
~~~

Had the rounding been part of the type, this would require obnoxious typecasts.

TODO: show runtime rounding mode

The default rounding mode is round-to-zero, the same defined by current C and C++
standards for integers. The `fxd::zero` namespace is defined inline, so these 3
expressions are all equivalent:

~~~{.cpp}
auto a = fxd::zero::mul(x, x);
auto b = fxd::mul(x, x);
auto c = x * x;
~~~


Boundary / Overflow Checking
----------------------------

Overflow checking is not part of the type. Since there are multiple ways to respond to an
overflow, it would also need rules for mixing up variables that have different "overflow
types."

Overflow checking is performed on a per-operation basis, in the namespaces:

  - `fxd::except`: all functions throw exceptions on overflow.

  - `fxd::expect`: all functions return a `std::expected` (if available; otherwise a
    built-in simplified implementation is used) with a `fxd::error` value on overflow.

  - `fxd::saturate`: all functions return either the minimum or maximum value on
    overflows.

In general, overflow checking is avoided whenever possible, due to the extra runtime cost
incurred. Here's an example for calculating an average:

~~~{.cpp}
using F = fxd::fixed<16, 16>;

F
calc_average(const std::vector<F>& values)
{
    if (values.empty())
        return 0;
    F total = 0;
    for (auto v : values)
        total = fxd::saturate::add(total, v);
    return total / values.size();
}
~~~

The division operation here does not need overflow checking, since it can't make the
result larger.

The limits of a fixed-point are directly associated with the limits of the underlying
representation. This allows for a fast overflow detection through compiler built-in
functions, such as `__builtin_add_overflow()`. Custom arbitrary boundaries are not supported.


Conversions and Mixed Operations
--------------------------------

Integers and floating-point values can be implicitly converted to `std::fixed`. The
reverse direction requires explicit conversion.

Fixed-point types can be implicitly converted to another if the destination type is not
smaller (in both integral and fractional bits); this guarantees no bits can be lost during
implicit conversions. Converting from unsigned to signed requires one extra integral bit,
since the highest bit is the sign bit.

Converting to an integer type `I` can be done with `static_cast<I>(...)`. There's also a
convenience function, `fxd::to_int<I>(...)` that performs the same cast. This function
also exists in rounding namespaces (`fxd::down::to_int<I>(...)`,
`fxd::up::to_int<I>(...)`, etc). When `I` is omitted, it defaults to the smallest integer
that can hold all of the integral bits; in other words, no overflow can occur.

Converting to a floating-point type `F` can be done with `static_cast<F>(...)`. Similarly,
there's also a `fxd::to_float<F>(...)` (also present in rounding namespaces.) When `F` is
omitted, it defaults to the smallest floating-point type that can represent all of the
bits of the `fxd::fixed` type; in other words, there's no loss of data in this conversion.

Explicit conversion to a fixed-point type `Fxd` is done with
`static_cast<Fxd>(...)`. There's also a named function, `fxd::fixed_cast<Fxd>(...)` (also
present in rounding namespaces.)

There are two exceptions to this rule:

  - Multiplication and division can be mixed with integers and floating-point, without
    an implicit conversion to `fxd::fixed`.

  - Comparisons (`==`, `!=`, `<=>`) can be mixed with integers, floating-point and
    different fixed-point.

The comparison operators (`==`, `<=>`) are always safe:

  - It's safe to compare signed and unsigned fixed-point types.

  - It's safe to compare fixed-point with any integer type.

  - It's safe to compare fixed-point with any floating-point type. The fixed-point value
    is converted to its natural floating-point type, and a floating-point comparison is
    used. The return type of `<=>` in this case is `std::partial_ordering`.

This decision was made to not require "safe" comparison functions like `std::cmp_*`.


Intermediate Precision
----------------------

No attempt is made to preserve extra bits, even though the internal computation might
require more bits. This makes operations more predictable. When appropriate, we can
manually change the fixed-point type to a wider one:

~~~{.cpp}
using F = fxd::fixed<16, 16>;

F
calc_average_ex(const std::vector<F>& values)
{
    if (values.empty())
        return 0;
    using FF = fxd::fixed<48, 16>;
    FF total = 0;
    for (auto v : values)
        total += v;
    // to go back to a smaller type, an explicit cast is needed.
    return static_cast<F>(total / values.size());
}
~~~


Standard Library Integration
----------------------------

Specializations in the `std` namespace are provided for:

  - `std::numeric_limits<>`
  - `std::hash<>`
  - `std::numbers::e_v<>`, `std::numbers::pi_v<>`, `std::numbers::sqrt2_v<>`, etc.
  - `std::common_type<>` (every arithmetic type is converted to a fixed-point)

Other standard-like elements are provided in the namespace `fxd`, because the standard
disallows specializations/overloads in `std`:

  - `fxd::uniform_real_distribution<>`
  - `fxd::abs()`, `fxd::sqrt()`
  
