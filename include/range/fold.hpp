/*
Copyright 2013-2015 Rogier van Dalen.

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

/** \file
This file does two things: it defines the interface for users and it gives the
default implementation of the "fold" operation.
The default implementation is good enough for most cases, but ranges can
specialise operation::fold to provide a faster version.
Note that operation::fold is declared (but not defined) in
detail/core_fold_declaration.hpp.
To implement a specialisation of fold for a specific range, there is no need
to include this file.
*/

#ifndef RANGE_FOLD_HPP_INCLUDED
#define RANGE_FOLD_HPP_INCLUDED

#include <functional>
#include <type_traits>

#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include <boost/utility/enable_if.hpp>

#include "utility/is_assignable.hpp"
#include "utility/storage.hpp"
#include "utility/assignable.hpp"

#include "rime/core.hpp"
#include "rime/assert.hpp"
#include "rime/variant.hpp"

#include "core.hpp"

#include "detail/fold_result.hpp"

namespace range {

/*
Interface.
The structure of this is the same as some of detail/core_*.hpp.
*/

namespace operation {

    // Reminder.
    // The general implementation is given at the bottom of this file.
    template <class RangeTag, class Direction, class Function, class State,
        class Range, class Enable /* = void*/>
    struct fold;

} // namespace operation

namespace apply {

    namespace automatic_arguments {

        // fold.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct fold : operation::unimplemented {};

        template <class Direction, class Function, class State, class Range>
            struct fold <meta::vector <Direction>,
                meta::vector <Function, State>, meta::vector <Range>>
        : operation::fold <typename range::tag_of <Range>::type,
            Direction, Function, State, Range &&> {};

    } // namespace automatic_arguments

    /** fold */
    template <class ... Arguments> struct fold
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view_once <
            automatic_arguments::fold>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace callable {
    struct fold : generic <apply::fold> {};
} // namespace callable

/**
General iteration through a range.
"fold" is the equivalent of standard C++ "accumulate".
It is sometimes called "reduce".
If a range r contains elements a, b, and c,
    fold (f, s, r),
with f a function and s the "state", computes
    f (f (f (s, a), b), c).
This yields a general form of iteration through a range.

For example, if there is a functor "plus" that calls operator+, then
    fold (plus, 0, r)
will compute
    ((0 + a) + b) + c.

The state that is passed in and the return values of the function must be
copy-constructible.
(They are cached in the function.)
Iteration happens with drop(), which will be passed an rvalue Range if that is
passed in to fold().
This should be useful for ranges that hold on to resources.

The fold is homogeneous if the range is homogeneous and the function returns
the same type as its first parameter.
The current version requires that the fold either has finite length or becomes
homogeneous at some point, otherwise it will not be able to compute the return
type.
(The only case for which this could be mended is if there is a limited set of
return values that the function cycles between, but this does not seem a great
use case.)

With homogeneous range, an obvious implementation is a loop, assigning a new
value to the state each time.
However, the implementation only uses move-construction and destruction.

The return type is automatically computed.
Types are collapsed up to some point (using the "collapse" merge policy), and
if multiple types result, the return type becomes a rime::variant.
The type resulting from each step of the fold is known: the result type of the
function is used exactly.
However, for the initial state, no distinction can be made between an rvalue
reference and a temporary.
For safety, it will always be returned as a temporary, not a reference.
\todo Parametrise these aspects: result type / merge policy, and state type.
(These must be passed to operation::fold in a clever way.)

\todo If the fold is fixed-period homogeneous  (e.g. a function that alternately
returns an int and a string), then a recursive implementation is currently
chosen, which may cause a stack overflow.

\todo (relatedly) It would be neat if fold_tree were to work.
This is not useful for result type inference: the intermediate types must be
assembled in a meta::set anyway, which involves a linear number of
instantiations.

\param direction
    (optional) The direction in which the range is traversed.
    If it is not given, then the default direction of the range is used.
\param function
    The function to be called on each element.
\param state
    The initial state.
    This is the first argument to the first invocation of \a function.
\param range
    The range to get the elements from.
*/
static const auto fold = callable::fold();

/* Implementation */

namespace operation {

    namespace fold_detail {

        /**
        The result type is given because it might otherwise become different
        throughout the fold.
        This can only lead to a string of conversions, and nothing good.

        The state type is given here so that the exact result of the function
        (including "&&") can be passed in.
        This makes a difference if the range turns out to be empty.
        */
        template <class Result, class Direction, class Function, class State>
            struct fold
        {
            /**
            Evaluate to the result of the function applied to the state and the
            first element of the range.
            */
            template <class Range> struct result_of_function
            : std::result_of <Function (State, typename
                std::result_of <callable::first (Direction, Range)>::type)>
            {};

            /**
            Evaluate to \c true iff the result type of the first function call
            is exactly the same as the current state type.
            */
            template <class Range> struct is_stable_state
            : std::is_same <State, typename result_of_function <Range>::type>
            {};

            /**
            Evaluate to \c true iff the fold is homogeneous.
            This always compiles, because boost::mpl::and_ evaluates its
            arguments lazily.
            If the range is not homogeneous, then is_stable_state is never
            instantiated.
            */
            template <class Range> struct is_homogeneous_fold
            : boost::mpl::and_ <
                is_homogeneous <Direction, Range>, is_stable_state <Range>> {};

            template <class Range> struct has_first
            : has <callable::first (Direction, Range &)> {};

            /*
            Four cases.
            This uses utility::overload_order to go through them one by one.
            */

            /**
            The fold is homogeneous, and "first" and "drop" are available.
            */
            template <class Range,
                class Enable = typename boost::enable_if <
                    has_first <Range>>::type,
                class Enable2 = typename boost::enable_if <
                    is_homogeneous_fold <Range>>::type>
            Result operator() (Direction const & direction,
                Function && function, State && state_, Range && range_,
                utility::overload_order <1> *) const
            {
                utility::assignable <State> state (
                    std::forward <State> (state_));
                auto range = std::forward <Range> (range_);
                while (!range::empty (direction, range)) {
                    state = function (state.move_content(),
                        range::first (direction, range));
                    range = range::drop (direction, std::move (range));
                }
                return state.move_content();
            }

            /**
            The fold is homogeneous, but only "chop" is available.
            */
            template <class Range,
                class Enable = typename boost::disable_if <
                    has_first <Range>>::type,
                class Enable2 = typename boost::enable_if <
                    is_homogeneous_fold <Range>>::type>
            Result operator() (Direction const & direction,
                Function && function, State && state_, Range && range_,
                utility::overload_order <2> *) const
            {
                utility::assignable <State> state (
                    std::forward <State> (state_));
                auto range = std::forward <Range> (range_);
                while (!range::empty (direction, range)) {
                    auto chopped = range::chop (direction, std::move (range));
                    state = function (
                        state.move_content(), chopped.move_first());
                    range = chopped.move_rest();
                }
                return state.move_content();
            }

            /**
            The fold is heterogeneous.
            The range is always empty: return "state".
            */
            template <class Range> typename
                boost::enable_if <always_empty <Direction, Range>, Result>::type
                operator() (Direction const &, Function &&, State && state,
                    Range &&, utility::overload_order <3> *) const
            { return std::forward <State> (state); }

            // Heterogeneous: if the range is non-empty.
            // Using first() and drop().
            template <class Range>
            typename boost::enable_if <has_first <Range>, Result>::type
                apply_non_empty (Direction const & direction,
                    Function && function, State && state, Range && range) const
            {
                // rime::assert_ (!range::empty (direction, range));
                fold <Result, Direction, Function,
                    typename result_of_function <Range>::type> recursive;
                return recursive (direction, std::forward <Function> (function),
                    function (std::forward <State> (state),
                        range::first (direction, range)),
                    range::drop (direction, std::forward <Range> (range)),
                    utility::pick_overload());
            }

            // Using chop().
            template <class Range>
            typename boost::disable_if <has_first <Range>, Result>::type
                apply_non_empty (Direction const & direction,
                    Function && function, State && state, Range && range) const
            {
                // rime::assert_ (!range::empty (direction, range));
                fold <Result, Direction, Function,
                    typename result_of_function <Range>::type> recursive;
                auto chopped = range::chop (
                    direction, std::forward <Range> (range));
                return recursive (direction, std::forward <Function> (function),
                    function (std::forward <State> (state),
                        chopped.move_first()),
                    chopped.move_rest(), utility::pick_overload());
            }

            /**
            The fold is heterogeneous.
            The range is never empty: call the implementation recursively.
            */
            template <class Range> typename
                boost::enable_if <never_empty <Direction, Range>, Result>::type
                operator() (Direction const & direction,
                    Function && function, State && state, Range && range,
                    utility::overload_order <3> *) const
            {
                return apply_non_empty (direction,
                    std::forward <Function> (function),
                    std::forward <State> (state), std::forward <Range> (range));
            }

            /**
            The fold is heterogeneous, and the range may or may not be
            empty.
            Depending on which one it is, call one of the above.
            */
            template <class Range>
                Result operator() (Direction const & direction,
                    Function && function, State && state, Range && range,
                    utility::overload_order <4> *) const
            {
                if (range::empty (direction, range))
                    return std::forward <State> (state);
                return apply_non_empty (direction,
                    std::forward <Function> (function),
                    std::forward <State> (state), std::forward <Range> (range));
            }
        };

        template <class Direction, class Function, class State, class Range>
            class default_implementation
        {
            typedef typename range::fold_detail::all_result_types <
                Direction, Function, State, Range>::type result_types;
            typedef typename rime::make_variant_over <result_types,
                rime::merge_policy::collapse>::type result_type;

        public:
            result_type operator() (Direction const & direction,
                Function && function, State && state, Range && range) const
            {
                static_assert (range::is_view <Direction, Range>::value,
                    "Internal error: the range must be a view here.");

                fold <result_type, Direction, Function, State> implementation;
                return implementation (direction,
                    std::forward <Function> (function),
                    std::forward <State> (state), std::forward <Range> (range),
                    utility::pick_overload());
            }
        };

    } // namespace fold_detail

    template <class RangeTag, class Direction, class Function, class State,
        class Range, class Enable>
    struct fold
    // Implemented if "empty" is implemented.
    : boost::mpl::if_ <is_implemented <empty <RangeTag, Direction, Range>>,
        fold_detail::default_implementation <Direction, Function, State, Range>,
        unimplemented>::type {};

} // namespace operation

} // namespace range

#endif // RANGE_FOLD_HPP_INCLUDED
