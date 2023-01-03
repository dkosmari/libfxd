#ifndef LIBFXD_RANDOM_HPP
#define LIBFXD_RANDOM_HPP

#include <istream>
#include <ostream>
#include <random>

#include "fixed.hpp"

#include "concepts.hpp"
#include "limits.hpp"


namespace fxd {


    template<fixed_point Fxd>
    struct uniform_real_distribution :
        private std::uniform_int_distribution<typename Fxd::raw_type> {

        using result_type = Fxd;


        struct param_type {

            constexpr
            param_type() :
                param_type{std::numeric_limits<result_type>::lowest()}
            {}

            explicit
            constexpr
            param_type(result_type a_,
                       result_type b_ = std::numeric_limits<result_type>::max()) :
                a_{a_},
                b_{b_}
            {}

            [[nodiscard]]
            constexpr result_type a() const noexcept { return a_; }

            [[nodiscard]]
            constexpr result_type b() const noexcept { return b_; }

            bool operator ==(const param_type& other) const noexcept = default;

        private:

            result_type a_, b_;

        };


        uniform_real_distribution() :
            uniform_real_distribution{std::numeric_limits<result_type>::lowest()}
        {}


        explicit
        uniform_real_distribution(result_type a_,
                                  result_type b_ = std::numeric_limits<result_type>::max()) :
            parent{a_.raw_value, b_.raw_value}
        {}


        explicit
        uniform_real_distribution(const param_type& p) :
            parent{typename parent::param_type{p.a().raw_value, p.b().raw_value}}
        {}


        void reset() noexcept {}


        result_type
        a()
            const noexcept
        {
            return result_type::from_raw(parent::a());
        }


        result_type
        b()
            const noexcept
        {
            return result_type::from_raw(parent::b());
        }


        [[nodiscard]]
        param_type
        param()
            const noexcept
        {
            return {a(), b()};
        }


        void
        param(const param_type& p)
        {
            parent::param(p.a().raw_value, p.b().raw_value);
        }


        [[nodiscard]]
        result_type
        min()
            const noexcept
        {
            return a();
        }


        [[nodiscard]]
        result_type
        max()
            const noexcept
        {
            return b();
        }


        template<typename Gen>
        [[nodiscard]]
        result_type
        operator ()(Gen& g)
        {
            return result_type::from_raw(parent::operator()(g));
        }


        template<typename Gen>
        [[nodiscard]]
        result_type
        operator ()(Gen& g, const param_type& p)
        {
            typename parent::param_type pp{p.a().raw_value, p.b().raw_value};
            return result_type::from_raw(parent::operator()(g, pp));
        }


        bool operator ==(const uniform_real_distribution&) const = default;


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



    private:

        using parent = std::uniform_int_distribution<typename Fxd::raw_type>;


    };


}


#endif
