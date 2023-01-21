#ifndef LIBFXD_EXCEPT_HPP
#define LIBFXD_EXCEPT_HPP

#include <cstdlib>
#include <stdexcept>

#include "concepts.hpp"
#include "error.hpp"
#include "utils-safe-includes.hpp"


namespace fxd::except {


    template<fxd::fixed_point Fxd>
    [[noreturn]]
    Fxd
    handler(error e)
    {
        switch (e) {
            case error::underflow:
                throw std::underflow_error{"underflow"};
            case error::overflow:
                throw std::overflow_error{"overflow"};
            case error::not_a_number:
                throw std::invalid_argument{"not a number"};
            default:
                std::abort();
        }
    }


#define LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL
#include "utils-safe.hpp"
#undef LIBFXD_INCLUDING_UTILS_SAFE_HPP_IMPL


} // namespace fxd::except


#endif
