/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_RANDOM_HPP
#define LIBFXD_RANDOM_HPP

#include <istream>
#include <ostream>
#include <random>

#include "concepts.hpp"
#include "limits.hpp"


namespace fxd {

    /// Analogous to `std::uniform_real_distribution`.
    template<fixed_point Fxd>
    class uniform_real_distribution :
        private std::uniform_int_distribution<typename Fxd::raw_type> {

        using parent = std::uniform_int_distribution<typename Fxd::raw_type>;

    public:

        /// The type that will be generated.
        using result_type = Fxd;

        /// Analogous to `std::uniform_real_distribution::param_type`
        class param_type {

            result_type a_, b_;

        public:

            constexpr
            param_type() :
                param_type{std::numeric_limits<result_type>::lowest()}
            {}

            /**
             * @brief Construct from min and max range.
             * @param a_ minimum value.
             * @param b_ maximum value (inclusive).
             */
            explicit
            constexpr
            param_type(result_type a_,
                       result_type b_ = std::numeric_limits<result_type>::max()) :
                a_{a_},
                b_{b_}
            {}

            /// Minimum value.
            [[nodiscard]]
            constexpr result_type a() const noexcept { return a_; }

            /// Maximum value.
            [[nodiscard]]
            constexpr result_type b() const noexcept { return b_; }

            /// Defaulted `==` operator.
            bool operator ==(const param_type& other) const noexcept = default;

        };


        /// Default constructor.
        uniform_real_distribution() :
            uniform_real_distribution{std::numeric_limits<result_type>::lowest()}
        {}


        /**
         * @brief Constructor with min and max values.
         * @param a_ Minimum value.
         * @param b_ Maximum value (inclusive).
         */
        explicit
        uniform_real_distribution(result_type a_,
                                  result_type b_ = std::numeric_limits<result_type>::max()) :
            parent{a_.raw_value, b_.raw_value}
        {}


        /// Constructor from a `param_type` object.
        explicit
        uniform_real_distribution(const param_type& p) :
            parent{typename parent::param_type{p.a().raw_value, p.b().raw_value}}
        {}


        /// Reset distribution state.
        void reset() noexcept {}


        /// Same as `min()`.
        result_type
        a()
            const noexcept
        {
            return result_type::from_raw(parent::a());
        }


        /// Same as `max()`.
        result_type
        b()
            const noexcept
        {
            return result_type::from_raw(parent::b());
        }


        /// Obtain distribution parameters.
        [[nodiscard]]
        param_type
        param()
            const noexcept
        {
            return {a(), b()};
        }


        /// Set distribution parameters.
        void
        param(const param_type& p)
        {
            parent::param(typename parent::param_type{p.a().raw_value,
                                                      p.b().raw_value});
        }


        /// Return lower bound of distribution.
        [[nodiscard]]
        result_type
        min()
            const noexcept
        {
            return a();
        }


        /// Return upper bound of distribution.
        [[nodiscard]]
        result_type
        max()
            const noexcept
        {
            return b();
        }


        /// Generate a random value.
        template<typename Gen>
        [[nodiscard]]
        result_type
        operator ()(Gen& g)
        {
            return result_type::from_raw(parent::operator()(g));
        }


        /// Generate a random value from a specific parameters object.
        template<typename Gen>
        [[nodiscard]]
        result_type
        operator ()(Gen& g, const param_type& p)
        {
            typename parent::param_type pp{p.a().raw_value, p.b().raw_value};
            return result_type::from_raw(parent::operator()(g, pp));
        }


        /// Defaulted `==` operator.
        bool operator ==(const uniform_real_distribution&) const = default;


        /// Output operator.
        template<typename CharT,
                 typename Traits>
        friend
        std::basic_ostream<CharT, Traits>&
        operator <<(std::basic_ostream<CharT, Traits>& out,
                    const uniform_real_distribution& dist)
        {
            const auto flags = out.flags();
            const auto fill = out.fill();
            const auto prec = out.precision();
            const auto space = out.widen(' ');

            out << std::scientific << std::left;
            out.fill(space);
            out.precision(std::numeric_limits<result_type>::max_digits10);

            try {
                out << dist.a() << space << dist.b();
                out.flags(flags);
                out.fill(fill);
                out.precision(prec);
                return out;
            }
            catch (...) {
                out.flags(flags);
                out.fill(fill);
                out.precision(prec);
                throw;
            }
        }


        /// Input operator.
        template<typename CharT,
                 typename Traits>
        friend
        std::basic_istream<CharT, Traits>&
        operator >>(std::basic_istream<CharT, Traits>& in,
                    const uniform_real_distribution& dist)
        {
            const auto flags = in.flags();
            in >> std::skipws;

            result_type a, b;
            try {
                if (in >> a >> b)
                    dist.params({a, b});
                in.flags(flags);
                return in;
            }
            catch (...) {
                in.flags(flags);
                throw;
            }
        }


    };


}


#endif
