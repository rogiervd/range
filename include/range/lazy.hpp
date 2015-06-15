/*
Copyright 2014, 2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef RANGE_LAZY_HPP_INCLUDED
#define RANGE_LAZY_HPP_INCLUDED

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
    \brief Store arguments for a callable object.

    Class that can be called and called, and will store the arguments until it
    is called with a range, at which point it will call the \a Callable with
    the range as the first argument.

    For example, <c>lazy \<callable::drop>() (4) (front) (range)</c> is
    equivalent to <c>drop  (range, front, 4)</c>.
    <c>lazy \<callable::drop>() (4, front) (range)</c> is too.
    The good thing about lazy functions is that they can be passed in as
    functors.

    The arguments are stored by value and passed to the function by const
    reference.

    \tparam Callable
        The callable that will be default-constructed and called.
    \tparam StoredArguments
        The arguments stored so far.
    */
    template <class Callable, class ... StoredArguments> class lazy {
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
            typedef lazy <Callable, StoredArguments ...,
                typename std::decay <NewArguments>::type ...> type;
        };

        template <class ... StoredIndices, class ... NewArguments>
            typename next_type <NewArguments ...>::type
            add_arguments (meta::vector <StoredIndices ...>,
                NewArguments && ... new_arguments) const
        {
            return typename next_type <NewArguments ...>::type (
                ::range::at (stored_arguments_, StoredIndices()) ...,
                std::forward <NewArguments> (new_arguments) ...);
        }

        /**
        Result type of calling this, assuming the call will go through to the
        Callable.
        */
        template <class Range> struct call_result
        : result_of <Callable (Range, StoredArguments const & ...)> {};

        template <class Range,
            class StoredArgumentsTuple, class ... StoredIndices>
        static typename call_result <Range>::type
        call_with (Range && range,
            StoredArgumentsTuple const & stored_arguments,
            meta::vector <StoredIndices ...>)
        {
            return Callable() (std::forward <Range> (range),
                ::range::at (stored_arguments, StoredIndices()) ...);
        }

    public:
        template <class ... QStoredArguments, class Enable =
            typename utility::disable_if_variadic_same_or_derived <
                lazy, QStoredArguments ...>::type>
        lazy (QStoredArguments && ... stored_arguments)
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
        template <class Range> typename
            boost::lazy_enable_if <is_range <Range>, call_result <Range>>::type
        operator() (Range && range) const {
            return (call_with (std::forward <Range> (range),
                stored_arguments_, stored_indices_type()));
        }

        /**
        Return a new <c>lazy\<></c> object with the additional arguments.
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

namespace lazy {

    static auto const default_direction
        = callable::lazy <callable::default_direction>();

    static auto const empty = callable::lazy <callable::empty>();
    static auto const size = callable::lazy <callable::size>();
    static auto const first = callable::lazy <callable::first>();
    static auto const at = callable::lazy <callable::at>();
    static auto const drop = callable::lazy <callable::drop>();
    static auto const chop = callable::lazy <callable::chop>();
    static auto const chop_in_place
        = callable::lazy <callable::chop_in_place>();

    static auto const view = callable::lazy <callable::view>();
    static auto const forward_view
        = callable::lazy <callable::forward_view>();
    static auto const view_once = callable::lazy <callable::view_once>();

} // namespace lazy

} // namespace range

#endif // RANGE_LAZY_HPP_INCLUDED
