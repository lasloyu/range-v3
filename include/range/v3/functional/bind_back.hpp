/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_FUNCTIONAL_BIND_BACK_HPP
#define RANGE_V3_FUNCTIONAL_BIND_BACK_HPP

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/invoke.hpp>

namespace ranges
{
    // bind_back like std::bind_front has no special treatment for nested
    // bind-expressions. So there is no need to wrap Callables with ranges::protect.

    template<typename Fn, typename... Args>
    class bind_back_t
    {
        using Self = bind_back_t<Fn, Args...>;

        // clang-format off
        template<typename Self, std::size_t... I, typename... CallArgs>
        constexpr auto static CPP_auto_fun(apply)
            (Self & self, std::index_sequence<I...>, CallArgs &&... call_args)
        (
            return invoke(self.fn,
                          std::forward<CallArgs>(call_args)...,
                          std::get<I>(self.args_tuple)...)
        )
        template<typename Self, typename... CallArgs>
        constexpr auto static CPP_auto_fun(run)(Self & self, CallArgs &&... call_args)
        (
            return apply(self,
                         std::index_sequence_for<Args...>{},
                         std::forward<CallArgs>(call_args)...)
        )

        Fn fn;
        std::tuple<Args...> args_tuple;
        // clang-format on

    public:
        constexpr bind_back_t(Fn && fn, Args &&... args)
          : fn(std::move(fn))
          , args_tuple(std::move(args)...)
        {}

        template<typename... CallArgs>
        constexpr decltype(auto) operator()(CallArgs &&... call_args) noexcept(
            noexcept(run(std::declval<Self &>(), std::forward<CallArgs>(call_args)...)))
        {
            return run(*this, std::forward<CallArgs>(call_args)...);
        }
        template<typename... CallArgs>
        constexpr decltype(auto) operator()(CallArgs &&... call_args) const
            noexcept(noexcept(run(std::declval<const Self &>(),
                                  std::forward<CallArgs>(call_args)...)))
        {
            return run(*this, std::forward<CallArgs>(call_args)...);
        }
    };

    // T/T&& => T&&, T& => T
    template<typename T,
             typename U = meta::if_<             //
                 std::is_lvalue_reference<T>,    //
                 uncvref_t<T>,                   //
                 std::remove_reference_t<T> &&>> //
    constexpr U bind_back_forward(std::remove_reference_t<T> & t) noexcept
    {
        return static_cast<U>(t);
    }

    // clang-format off
    template<typename Fn, typename... Args>
    constexpr auto CPP_auto_fun(bind_back)(Fn && fn, Args &&... args)
    (
        return bind_back_t<std::decay_t<Fn>, std::decay_t<Args>...>(
            bind_back_forward<Fn>(fn), bind_back_forward<Args>(args)...)
    )
    // clang-format on

} // namespace ranges

#endif // RANGE_V3_FUNCTIONAL_BIND_BACK_HPP