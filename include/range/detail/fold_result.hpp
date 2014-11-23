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

#ifndef RANGE_DETAIL_FOLD_RESULT_HPP_INCLUDED
#define RANGE_DETAIL_FOLD_RESULT_HPP_INCLUDED

#include <utility>
#include <type_traits>

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/or.hpp"
#include "boost/mpl/placeholders.hpp"
#include "boost/mpl/identity.hpp"
#include "boost/mpl/bool.hpp"

#include "meta/set.hpp"
#include "meta/range.hpp"
#include "meta/transform.hpp"
#include "meta/filter.hpp"

#include "rime/core.hpp"
#include "rime/variant.hpp"

#include "range/core.hpp"

namespace range { namespace fold_detail {

    /**
    Intermediate value for folds: the state, and the range.
    This is a pair of parameters that can change between steps of the fold.
    */
    template <class State, class Range> struct step {
        typedef State state_type;
        typedef Range range_type;
    };

    /// Return the \c state_type type from \a Steps.
    template <class Step> struct get_state
    { typedef typename Step::state_type type; };

    /// Return whether the step is not known to be non-empty.
    template <class Direction, class Step> struct step_may_be_empty
    : boost::mpl::not_ <rime::equal_constant <rime::false_type,
        typename std::result_of <range::callable::empty (Direction,
            typename Step::range_type)>::type>> {};

    /**
    Hold a direction and a function, and call it to produce the next step of
    a fold.
    */
    template <class Direction, class Function> class next_step {
    private:
        Direction direction;
        Function function;
    public:
        next_step (Direction const & direction, Function && function)
        : direction (direction), function (function) {}

        template <class Step> struct result;

        template <class State, class Range>
            struct result <step <State, Range>>
        {
            // Use "chop" and not "first" and "drop", because "chop" is always
            // defined.
            // This defined first_type and next_range_type if "chop" is not
            // used:
            /*
            typedef typename std::result_of <
                range::callable::first (Direction, Range &)>::type first_type;
            typedef typename std::result_of <
                    range::callable::drop (Direction, Range &&)>::type
                next_range_type;
            */

            static_assert (
                range::has <range::callable::chop (Direction, Range)>::value,
                "Chop should always be implemented.");

            typedef typename std::result_of <
                range::callable::chop (Direction, Range)>::type chopped_type;
            typedef typename chopped_type::first_type first_type;
            typedef typename chopped_type::rest_type next_range_type;

            typedef typename std::result_of <
                Function (State &&, first_type)>::type next_state_type;

            typedef step <next_state_type, next_range_type> type;
        };
    };

    /**
    Compute a meta::set containing all steps of a fold.
    This finishes when the step is already in the set, because no more new
    types could be added.
    If the range is empty, then it inserts the current step into the set and
    returns.
    */
    template <class Direction, class Function, class Step,
            class PreviousSteps = meta::set<>>
        struct all_steps;
    template <class Direction, class Function, class Step, class PreviousSteps>
        struct all_steps_next;
    template <class Direction, class Function, class Step, class PreviousSteps>
        struct all_steps_next_not_empty;

    template <class Direction, class Function, class Step, class PreviousSteps>
        struct all_steps
    : boost::mpl::eval_if <meta::contains <Step, PreviousSteps>,
        PreviousSteps,
        all_steps_next <Direction, Function, Step, PreviousSteps>
    > {};

    template <class Direction, class Function, class Step, class PreviousSteps>
        struct all_steps_next
    : boost::mpl::eval_if <
        always_empty <Direction, typename Step::range_type>,
        meta::push <meta::front, Step, PreviousSteps>,
        all_steps_next_not_empty <Direction, Function, Step, PreviousSteps>
    > {};

    template <class Direction, class Function, class Step, class PreviousSteps>
        struct all_steps_next_not_empty
    {
        typedef typename next_step <Direction, Function>
            ::template result <Step>::type next_step;
        typedef typename all_steps <Direction, Function, next_step, typename
                meta::push <meta::front, Step, PreviousSteps>::type>::type
            type;
    };

    /**
    Compute all possible return types that could result from a fold operation.
    This works by, for all possible steps, determining whether the range could
    be finished (in which case the fold finishes) and returning the set of
    state types that may result from this.
    */
    template <class Direction, class Function, class State, class Range>
        struct all_result_types
    {
        typedef typename all_steps <Direction, Function, step <State, Range>
        >::type steps;

        typedef typename meta::as_vector <meta::filter <
                step_may_be_empty <Direction, boost::mpl::_1>, steps>
            >::type returnable_steps;

        /*typedef typename meta::as_set <
                meta::transform <get_state <boost::mpl::_1>, returnable_steps>
            >::type type;*/
        // Reverse the list while taking out the duplicates.
        typedef typename meta::fold <
                meta::insert <boost::mpl::_2, boost::mpl::_1>,
                meta::set<>,
                meta::transform <get_state <boost::mpl::_1>, returnable_steps>
            >::type type;
    };

}} // namespace range::fold_detail

#endif // RANGE_DETAIL_FOLD_RESULT_HPP_INCLUDED
