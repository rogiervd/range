/*
Copyright 2014 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RANGE_CURRY_HPP_INCLUDED
#define RANGE_CURRY_HPP_INCLUDED

#include "meta/vector.hpp"
#include "meta/count.hpp"
#include "meta/all_of_c.hpp"
#include "meta/any_of_c.hpp"

#include "utility/disable_if_same.hpp"
#include "utility/returns.hpp"

#include "core.hpp"
#include "tuple.hpp"

namespace range {

namespace callable {

    /**
    \brief Curry callable objects.

    Class that can be called and called, and will store the arguments until at
    least one of the arguments is a range, at which point it will call the
    \a Callable.

    For example, <c>curried \<callable::drop>() (front) (4) (range)</c> is
    equivalent to <c>drop  (front, 4, range)</c>.
    <c>curried \<callable::drop>() (front, 4) (range)</c> is too, as is
    <c>curried \<callable::drop>() (front, 4, range)</c>.
    The good thing about currying is that curried functions can be passed in as
    functors.

    The arguments are stored by value and passed to the function by const
    reference.

    \tparam Callable
        The callable that will be default-constructed and called.
    \tparam StoredArguments
        The arguments stored so far.
    */
    template <class Callable, class ... StoredArguments> class curried {
        static_assert (meta::all_of_c <
                std::is_same <StoredArguments,
                    typename std::decay <StoredArguments>::type>::value ...
            >::value,
            "Only unqualified types can be stored.");

        range::tuple <StoredArguments ...> stored_arguments_;

        /**
        \internal
        To extract the elements of stored_arguments_ and call anything with
        them, they must be extracted by
        at (zero, stored_arguments_), at (one, stored_arguments_) ... .
        stored_indices_type provides a meta::vector <zero_type, one_type ...>
        for this exact purpose.
        */
        typedef typename meta::count <sizeof ... (StoredArguments)>::type
            stored_indices_type;

        /**
        The type this but then with extra arguments.
        */
        template <class ... NewArguments> struct next_type {
            typedef curried <Callable, StoredArguments ...,
                typename std::decay <NewArguments>::type ...> type;
        };

        template <class ... StoredIndices, class ... NewArguments>
            typename next_type <NewArguments ...>::type
            add_arguments (meta::vector <StoredIndices ...>,
                NewArguments && ... new_arguments) const
        {
            return typename next_type <NewArguments ...>::type (
                ::range::at (StoredIndices(), stored_arguments_) ...,
                std::forward <NewArguments> (new_arguments) ...);
        }

        /**
        Result type of calling this, assuming the call will go through to the
        Callable.
        */
        template <class ... NewArguments> struct call_result
        : std::result_of <Callable (
            StoredArguments const & ..., NewArguments ...)> {};

        template <class StoredArgumentsTuple, class ... StoredIndices,
            class ... NewArguments>
        static typename call_result <NewArguments ...>::type
        call_with (StoredArgumentsTuple const & stored_arguments,
            meta::vector <StoredIndices ...>,
            NewArguments && ... new_arguments)
        {
            return (Callable() (
                ::range::at (StoredIndices(), stored_arguments) ...,
                std::forward <NewArguments> (new_arguments) ...));
        }

    public:
        template <class ... QStoredArguments, class Enable =
            typename utility::disable_if_variadic_same_or_derived <
                curried, QStoredArguments ...>::type>
        curried (QStoredArguments && ... stored_arguments)
        : stored_arguments_ (
            std::forward <QStoredArguments> (stored_arguments) ...) {}

        /**
        Call a newly constructed <c>Callable()</c> with the additional
        arguments.
        This overload is activated when at least one of the new arguments is a
        range.
        \param new_arguments
            The arguments to be used in the call behind the stored arguments.
        */
        template <class ... NewArguments>
            typename boost::lazy_enable_if <
                meta::any_of_c <is_range <NewArguments>::value ...>,
            call_result <NewArguments ...>>::type
        operator() (NewArguments && ... new_arguments) const
        {
            return (call_with (stored_arguments_, stored_indices_type(),
                std::forward <NewArguments> (new_arguments) ...));
        }

        /**
        Return a new <c>curried\<></c> object with the additional arguments.
        This overload is activated when none of the new arguments is a range.
        \param new_arguments
            The arguments to be appended to the currently stored arguments.
        */
        template <class ... NewArguments>
            typename boost::lazy_disable_if <meta::any_of_c <
                is_range <NewArguments>::value ...>,
            next_type <NewArguments ...>>::type
        operator() (NewArguments && ... new_arguments) const {
            return add_arguments (stored_indices_type(),
                std::forward <NewArguments> (new_arguments) ...);
        }
    };

} // namespace callable

namespace curry {

    static auto const default_direction
        = callable::curried <callable::default_direction>();

    static auto const empty = callable::curried <callable::empty>();
    static auto const size = callable::curried <callable::size>();
    static auto const first = callable::curried <callable::first>();
    static auto const at = callable::curried <callable::at>();
    static auto const drop = callable::curried <callable::drop>();
    static auto const chop = callable::curried <callable::chop>();
    static auto const chop_in_place
        = callable::curried <callable::chop_in_place>();

    static auto const view = callable::curried <callable::view>();
    static auto const forward_view
        = callable::curried <callable::forward_view>();
    static auto const view_once = callable::curried <callable::view_once>();

} // namespace curry

} // namespace range

#endif // RANGE_CURRY_HPP_INCLUDED
