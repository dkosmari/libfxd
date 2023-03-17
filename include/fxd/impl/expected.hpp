/*
 * libfxd - a fixed-point library for C++
 *
 * Copyright 2023 Daniel K. O.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBFXD_IMPL_EXPECTED_HPP
#define LIBFXD_IMPL_EXPECTED_HPP

#include <exception>
#include <type_traits>
#include <utility> // in_place_t, forward()
#include <variant>

/*
 * A simplified implementation of std::expected from C++23
 */


namespace fxd::impl {


    template<typename E>
    struct bad_expected_access : std::exception {

        explicit bad_expected_access(E e):
            data{e}
        {}

        const E&
        error() const & noexcept
        { return data; }

        E&
        error() & noexcept
        { return data; }

        const E&&
        error() const && noexcept
        { return data; }

        E&&
        error() && noexcept
        { return data; }


        virtual
        const char*
        what()
            const noexcept override
        {
            return "bad_expected_access";
        }

    private:

        E data;

    };



    template<typename E>
    class unexpected {

        E value;

    public:

        constexpr
        unexpected(const unexpected&) = default;

        constexpr
        unexpected(unexpected&&) = default;

        template<typename Err = E>
        requires (
                 !std::is_same_v<std::remove_cvref_t<Err>, unexpected>
                 &&
                 !std::is_same_v<std::remove_cvref_t<Err>, std::in_place_t>
                 &&
                 std::is_constructible_v<E, Err>
                 )
        constexpr
        explicit unexpected(Err&& e) :
            value(std::forward<Err>(e))
        {}


        template<typename... Args>
        requires (std::is_constructible_v<E, Args...>)
        constexpr
        explicit unexpected(std::in_place_t, Args&&... args) :
            value(std::forward<Args>(args)...)
        {}


        constexpr
        bool
        operator ==(const unexpected&) const noexcept = default;


        constexpr
        const E&
        error()
            const & noexcept
        {
            return value;
        }


        constexpr
        E&
        error()
            & noexcept
        {
            return value;
        }



    };


    template<typename E>
    unexpected(E) -> unexpected<E>;



    struct unexpect_t {
        explicit unexpect_t() = default;
    };


    inline constexpr
    unexpect_t unexpect{};



    template<typename T,
             typename E>
    class expected {

        using value_idx = std::in_place_index_t<0>;
        using error_idx = std::in_place_index_t<1>;

        using var_type = std::variant<T, E>;
        var_type data;

    public:

        using value_type = T;
        using error_type = E;
        using unexpected_type = unexpected<E>;

        template<typename U>
        using rebind = expected<U, error_type>;

        // constructors


        constexpr
        expected()
            noexcept
            requires (std::is_default_constructible_v<var_type>)
            = default;


        constexpr
        expected(const expected&)
            requires (std::is_copy_constructible_v<var_type>)
            = default;


        constexpr
        expected(expected&& other)
            requires (std::is_move_constructible_v<var_type>)
            = default;


        template<typename U = T>
        constexpr
        explicit (!std::is_convertible_v<U, T>)
        expected(U&& v) :
            data(value_idx{}, std::forward<U>(v))
        {}


        template<typename G>
        constexpr
        explicit (!std::is_convertible_v<const G&, E>)
        expected(const unexpected<G>& e) :
            data(error_idx{}, e.error())
        {}


        template<typename G>
        constexpr
        explicit (!std::is_convertible_v<G, E>)
        expected(unexpected<G>&& e) :
            data(error_idx{}, std::move(e.error()))
        {}


        constexpr
        explicit
        expected(std::in_place_t) noexcept :
            data(value_idx{})
        {}


        template<typename... Args>
        constexpr
        explicit expected(std::in_place_t,
                          Args&&... args) :
            data(value_idx{}, std::forward<Args>(args)...)
        {}


        template<typename... Args>
        constexpr
        explicit expected(unexpect_t,
                          Args&&... args) :
            data(error_idx{}, std::forward<Args>(args)...)
        {}



        // observers


        constexpr
        const value_type*
        operator ->()
            const noexcept
        {
            return &get<0>(data);
        }


        constexpr
        value_type*
        operator ->()
            noexcept
        {
            return &get<0>(data);
        }


        constexpr
        const value_type&
        operator *()
            const& noexcept
        {
            return get<0>(data);
        }


        constexpr
        value_type&
        operator *()
            & noexcept
        {
            return get<0>(data);
        }


        constexpr
        const value_type&&
        operator *()
            const&& noexcept
        {
            return get<0>(data);
        }

        constexpr
        value_type&&
        operator *()
            && noexcept
        {
            return get<0>(data);
        }


        constexpr
        explicit operator bool()
            const noexcept
        {
            return has_value();
        }


        constexpr
        bool
        has_value()
            const noexcept
        {
            return data.index() == 0;
        }


        constexpr value_type&
        value()
            &
        {
            if (!has_value())
                throw bad_expected_access{error()};
            return get<0>(data);
        }


        constexpr
        const value_type&
        value()
            const &
        {
            if (!has_value())
                throw bad_expected_access{error()};
            return get<0>(data);
        }


        constexpr
        value_type&&
        value()
            &&
        {
            if (!has_value())
                throw bad_expected_access{std::move(error())};
            return get<0>(data);
        }


        constexpr
        const value_type&&
        value()
            const &&
        {
            if (!has_value())
                throw bad_expected_access{std::move(error())};
            return get<0>(data);
        }


        constexpr
        const error_type&
        error() const & noexcept
        { return get<1>(data); }

        constexpr
        error_type&
        error() & noexcept
        { return get<1>(data); }

        constexpr
        const error_type&&
        error() const && noexcept
        { return get<1>(data); }

        constexpr
        error_type&&
        error() && noexcept
        { return get<1>(data); }


        template<typename U>
        constexpr
        value_type
        value_or(U&& v)
            const &
        {
            return has_value()
                ? **this
                : static_cast<value_type>(std::forward<U>(v));
        }


        template<typename U>
        constexpr
        value_type
        value_or(U&& v)
            &&
        {
            return has_value()
                ? std::move(**this)
                : static_cast<value_type>(std::forward<U>(v));
        }





    };

} // namespace fxd::impl


#endif
