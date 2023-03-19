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

  - `fxd::fixed<32, 0>` behaves like `std::int32_t`
  - `fxd::fixed<33, -1>` only stores even integers.
  - `fxd::ufixed<1, 31>` only stores values in the interval \f$ [0, 2) \f$
  - `fxd::ufixed<0, 32>` only stores values in the interval \f$ [0, 1) \f$
  - `fxd::ufixed<-1, 33>` only stores values in the interval \f$ [0, 0.5) \f$
  - `fxd::fixed<1, 31>` only stores values in the interval \f$ [-1, 1) \f$
  - `fxd::fixed<0, 32>` only stores values in the interval \f$ [-0.5, 0.5) \f$

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

The compiler must accept the "raw type" for a bit-field. That excludes user-defined
types. This restriction might be lifted in the future, by allowing a custom type trait to
wrap the bit-field.

Furthermore, the library makes heavy usage of standard concepts, type traits and
`std::numeric_limits`. To convince the standard library to accept types like `__int128` as
integral types, we often need special compiler options, like `-std=gnu++20`; otherwise,
the `std::integral` concept will not match such extended types, and `std::numeric_limits`
might not have a specialization for such a type.


Rounding Modes
--------------

The rounding mode is not part of the type. Instead, it's specified on a per-operation
basis.

For instance: to evaluate the expression \f$ x^2 - y^2 \f$ rounded down, we can write:

~~~{.cpp}
fxd::down::mul(x, x) - fxd::up::mul(y, y);
~~~

Had the rounding been part of the type, this would require obnoxious typecasts.

TODO: show runtime rounding mode

The default rounding mode is round-to-zero, the same defined by current C and C++
standards for integers.


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

Integers and floating-point values can be implicitly converted to `std::fixed`. But once
we have a fixed-point value, it will not be automatically converted to anything else, not
even to a different fixed-point type.

This is done because a fixed-point type imposes invariants. We can control the rounding,
check for overflows. There's expectations about where the point is located, how many
precision bits are used in the operations. These would be harder to manage if the
fixed-point type was allowed to be implicitly "promoted" to a floating-point, for
instance.

Converting to an integer can be done with the `fxd::to_int<I>(...)` function, or a
`static_cast<I>(...)`, where `I` is the destination integer type. When `I` is not
specified to `fxd::to_int(...)`, it will default to the smallest integer with
`Int` bits.

Converting to a floating-point type can be done with the `fxd::to_float<F>(...)` function,
or a `static_cast<F>(...)`, where `F` is the destination floating-point type. When `F` is
not specified to `fxd::to_float(...)`, it will default to the smallest floating-point type
that fully represents all of the fixed-point bits. This default type is available at
`std::numeric_limits<fxd::fixed<...>>::float_type`.

Converting to a different fixed-point type is done through the `fxd::fixed_cast<...>(...)`
function.

There are two exceptions to this rule:

  - Multiplication and division can be mixed with integers and floating-point.
  - Comparisons (`==`, `!=`, `<=>`) can be mixed with integers, floating-point and
    different fixed-point. This avoids the need for helper `std::cmp_*()` just to ensure
    correctness.


Intermediate Precision
----------------------

No attempt is made to use more bits during computations. This makes operations more
predictable. When appropriate, we can manually change the fixed-point type to a wider one:

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
        total += fixed_cast<FF>(v);
    return fixed_cast<F>(total / values.size());
}
~~~


Standard Library Integration
----------------------------

Specializations in the `std` namespace are provided for:

  - `numeric_limits<>`
  - `hash<>`
  - `numbers::e_v<>`, `numbers::pi_v<>`, `numbers::sqrt2_v<>`, etc.
  - `common_type<>` (every arithmetic type is converted to a fixed-point)

Other standard-like elements are provided in the namespace `fxd`, because the standard
disallows specializations/overloads in `std`:

  - `fxd::uniform_real_distribution<>`
  - `fxd::abs()`, `fxd::sqrt()`
  
