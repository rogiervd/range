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

/* Implementation */

namespace fold_detail {

    /**
    The result type is given because it might otherwise become different
    throughout the fold.
    This can only lead to a string of conversions, and nothing good.

    The state type is given here so that the exact result of the function
    (including "&&") can be passed in.
    This makes a difference if the range turns out to be empty.
    */
    template <class Result, class State, class Direction, class Function>
        struct fold
    {
        /**
        Evaluate to the result of the function applied to the state and the
        first element of the range.
        */
        template <class Range> struct result_of_function
        : result_of <Function (State, typename
            result_of <callable::first (Range, Direction)>::type)>
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
            is_homogeneous <Range, Direction>, is_stable_state <Range>> {};

        template <class Range> struct has_first
        : has <callable::first (Range &, Direction)> {};

        /*
        Four cases.
        This uses overload_order to go through them one by one.
        */

        /**
        The fold is homogeneous, and "first" and "drop" are available.
        */
        template <class Range,
            class Enable = typename boost::enable_if <
                has_first <Range>>::type,
            class Enable2 = typename boost::enable_if <
                is_homogeneous_fold <Range>>::type>
        Result operator() (State && state_, Range && range_,
            Direction const & direction, Function && function,
            overload_order <1> *) const
        {
            utility::assignable <State> state (
                std::forward <State> (state_));
            auto range = std::forward <Range> (range_);
            while (!range::empty (range, direction)) {
                state = function (state.move_content(),
                    range::first (range, direction));
                range = range::drop (std::move (range), direction);
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
        Result operator() (State && state_, Range && range_,
            Direction const & direction, Function && function,
            overload_order <2> *) const
        {
            utility::assignable <State> state (
                std::forward <State> (state_));
            auto range = std::forward <Range> (range_);
            while (!range::empty (range, direction)) {
                auto chopped = range::chop (std::move (range), direction);
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
        template <class Range,
            class Enable = typename boost::enable_if <
                always_empty <Range, Direction>>::type>
            Result operator() (State && state, Range &&, Direction const &,
                Function &&, overload_order <3> *) const
        { return std::forward <State> (state); }

        // Heterogeneous: if the range is non-empty.
        // Using first() and drop().
        template <class Range>
        typename boost::enable_if <has_first <Range>, Result>::type
            apply_non_empty (State && state, Range && range,
                Direction const & direction, Function && function) const
        {
            // rime::assert_ (!range::empty (direction, range));
            fold <Result, typename result_of_function <Range>::type,
                Direction, Function> recursive;
            return recursive (
                function (std::forward <State> (state),
                    range::first (range, direction)),
                range::drop (std::forward <Range> (range), direction),
                direction, std::forward <Function> (function), pick_overload());
        }

        // Using chop().
        template <class Range>
        typename boost::disable_if <has_first <Range>, Result>::type
            apply_non_empty (State && state, Range && range,
                Direction const & direction, Function && function) const
        {
            // rime::assert_ (!range::empty (direction, range));
            fold <Result, typename result_of_function <Range>::type,
                Direction, Function> recursive;
            auto chopped = range::chop (
                std::forward <Range> (range), direction);
            return recursive (
                function (std::forward <State> (state), chopped.move_first()),
                chopped.move_rest(),
                direction, std::forward <Function> (function), pick_overload());
        }

        /**
        The fold is heterogeneous.
        The range is never empty: call the implementation recursively.
        */
        template <class Range> typename
            boost::enable_if <never_empty <Range, Direction>, Result>::type
            operator() (State && state, Range && range,
                Direction const & direction, Function && function,
                overload_order <3> *) const
        {
            return apply_non_empty (std::forward <State> (state),
                std::forward <Range> (range), direction,
                std::forward <Function> (function));
        }

        /**
        The fold is heterogeneous, and the range may or may not be
        empty.
        Depending on which one it is, call one of the above.
        */
        template <class Range>
            Result operator() (State && state, Range && range,
                Direction const & direction, Function && function,
                overload_order <4> *) const
        {
            if (range::empty (range, direction))
                return std::forward <State> (state);
            return apply_non_empty (std::forward <State> (state),
                std::forward <Range> (range), direction,
                std::forward <Function> (function));
        }
    };

    template <class State, class Range, class Direction, class Function>
        class default_implementation
    {
        typedef typename range::fold_detail::all_result_types <
            State, Range, Direction, Function>::type result_types;
        typedef typename rime::make_variant_over <result_types,
            rime::merge_policy::collapse>::type result_type;

    public:
        result_type operator() (State && state, Range && range,
            Direction const & direction, Function && function) const
        {
            static_assert (range::is_view <Range, Direction>::value,
                "Internal error: the range must be a view here.");

            fold <result_type, State, Direction, Function> implementation;
            return implementation (
                std::forward <State> (state), std::forward <Range> (range),
                direction, std::forward <Function> (function), pick_overload());
        }
    };

} // namespace fold_detail

namespace helper {

    /** \brief
    Hook for implementing fold() for a type of range.

    This does normally not have to be implemented, unless the default
    implementation does not suffice.

    To provide an implementation of fold() specific to a range, implement either
    member function fold() on the range, or free function implement_fold().
    If both of these are defined, then the free function will be preferred.

    \param tag The range tag.
    \param state The initial state.
    \param range The range to get the elements from.
    \param direction The direction in which the range is traversed.
    \param function The function to be called on each element.
    */
    void implement_fold (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_fold;

        struct fold {
        private:
            struct dispatch {
                // Use implement_fold, if it is implemented.
                template <class State, class Range, class Direction,
                    class Function>
                auto operator() (State && state, Range && range,
                    Direction const & direction, Function && function,
                    overload_order <1> *) const
                RETURNS (implement_fold (typename tag_of <Range>::type(),
                    std::forward <State> (state),
                    std::forward <Range> (range), direction,
                    std::forward <Function> (function)));

                // Use member function .fold, if it is implemented.
                template <class State, class Range, class Direction,
                    class Function>
                auto operator() (State && state, Range && range,
                    Direction const & direction, Function && function,
                    overload_order <2> *) const
                RETURNS (helper::member_access::fold (
                    std::forward <State> (state),
                    std::forward <Range> (range), direction,
                    std::forward <Function> (function)));

                // Use default implementation.
                template <class State, class Range, class Direction,
                    class Function>
                auto operator() (State && state, Range && range,
                    Direction const & direction, Function && function,
                    overload_order <3> *) const
                RETURNS (fold_detail::default_implementation <
                        State, Range, Direction, Function>() (
                    std::forward <State> (state),
                    std::forward <Range> (range), direction,
                    std::forward <Function> (function)));
            };

        public:
            template <class State, class Range, class Direction, class Function,
                // Implemented if "empty" is implemented.
                class Enable = decltype (range::empty (
                    std::declval <Range>(), std::declval <Direction>()))>
            auto operator() (State && state, Range && range,
                Direction const & direction, Function && function) const
            RETURNS (dispatch() (std::forward <State> (state),
                range::view_once (std::forward <Range> (range), direction),
                direction,
                std::forward <Function> (function), pick_overload()));

            // Without direction: use default_direction.
            template <class State, class Range, class Function,
                // Implemented if "empty" is implemented.
                class Enable = decltype (range::empty (std::declval <Range>()))>
            auto operator() (State && state, Range && range,
                Function && function) const
            RETURNS (dispatch() (std::forward <State> (state),
                range::view_once (std::forward <Range> (range)),
                range::default_direction (range),
                std::forward <Function> (function), pick_overload()));
        };

    } // namespace implementation

    using implementation::fold;

} // namespace callable

/** \brief
Traverse a range and accumulate a value.

fold() is the equivalent of standard C++ \c accumulate.
It is sometimes called \c reduce.
If a range \c r contains elements \c a, \c b, and \c c,
<c>fold (f, s, r)</c>,
with \c f a function and \c s the "state", computes
    <c>f (f (f (s, a), b), c)</c>.
This yields a general form of iteration through a range.

For example, if there is a functor \c plus that calls \c operator+, then
<c>fold (plus, 0, r)</c>
will compute
<c>((0 + a) + b) + c</c>.

The state that is passed in and the return values of the function must be
copy-constructible.
(They are cached in the function.)
The range can be noncopyable.
In the default implementation, iteration is implemented with drop(), which will
be passed an rvalue Range if that is passed in to fold().

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
Types are collapsed to some degree (using the "collapse" merge policy), and
if multiple types result, the return type becomes a rime::variant.
The type resulting from each step of the fold is known: the result type of the
function is used exactly.
However, for the initial state, no distinction can be made between an rvalue
reference and a temporary.
For safety, it will always be returned as a temporary, not a reference.

\todo Parametrise these aspects: result type / merge policy, and state type.
(These must be passed to implement_fold in a clever way.)

\todo If the fold is fixed-period homogeneous  (e.g. a function that alternately
returns an int and a string), then a recursive implementation is currently
chosen, which may cause a stack overflow.

\todo (relatedly) It would be neat if fold_tree were to work.
This is not useful for result type inference: the intermediate types must be
assembled in a meta::set anyway, which involves a linear number of
instantiations.

\param state
    The initial state.
    This is the first argument to the first invocation of \a function.
\param range
    The range to get the elements from.
\param direction
    (optional) The direction in which the range is traversed.
    If it is not given, then the default direction of the range is used.
\param function
    The function to be called on each element.
*/
static const auto fold = callable::fold();

} // namespace range

#endif // RANGE_FOLD_HPP_INCLUDED
