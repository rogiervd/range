/*
Copyright 2013, 2015 Rogier van Dalen.

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

#ifndef RANGE_ANY_RANGE_CAPABILITY_HPP_INCLUDED
#define RANGE_ANY_RANGE_CAPABILITY_HPP_INCLUDED

#include <type_traits>
#include <utility>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/set.hpp"
#include "meta/map.hpp"
#include "meta/filter.hpp"
#include "meta/all_of.hpp"
#include "meta/all_of_c.hpp"

#include "rime/core.hpp"

#include "range/direction.hpp"
#include "range/core.hpp"

namespace range {

/**
Namespace that contains dummy types that indicate capabilities of ranges.
This is useful for any_range, which hides the implementation, but exposes
exactly the given capabilities.
*/
namespace capability {

    template <class Type> struct type {};

    /** \brief
    Capability key for copy construction.

    The value type to go with this is void.
    */
    struct copy_construct;

    /** \brief
    Capability key for the default_direction.

    The value type to go with this indicates the type of the direction.
    */
    struct default_direction;

    /** \brief
    Indicate support for empty on a const-reference range.
    */
    struct empty;

    /** \brief
    Indicate support for size on a const-reference range.
    */
    struct size;

    /** \brief
    Indicate support for first on a const-reference range.
    */
    struct first;

    /** \brief
    Indicate support for drop without an increment on a const-reference range.
    */
    struct drop_one;

    /** \brief
    Indicate support for drop with an increment on a const-reference range.
    */
    struct drop_n;

    /** \brief
    Indicate support for either chop_in_place, or of chop with an rvalue
    reference.
    */
    struct chop_destructive;

    /* Capabilities and capability keys. */
    /*
    For any_range to know what the underlying range can do, this must be
    indicated in a type.
    There are many choices for this; and it will often be useful to cast to an
    any_range with reduced capabilities.

    Capabilities are therefore kept in a meta::map.
    Keys can be copy_construct, default_direction, or a direction type.
    The values are, respectively, void, the type of the default direction, or a
    meta::set with the operations for the direction.

    To cast an underlying range to one with reduced capabilities, there must be
    a virtual method for each capability that can be removed, that returns a
    newly allocated object that has all capabilities except for the one.
    That object, of course, also needs to have the same virtual methods for
    losing capabilities as well.
    For n directions, therefore, O(n^2) instantiations are required.
    That quickly becomes untenable if each pair of direction and capability is
    treated as separate.
    (Indeed, an earlier version of this had this problem.)
    Therefore, each key in the meta::map is treated as one by the polymorphic
    interface.
    Your average any_range will have three or four of these, so that is fine.

    A set of these keys, without the default_direction key, is called
    "capability keys".
    */

    template <class ... Types> struct all_are_capability_keys
    : meta::all_of_c <
        (std::is_same <Types, copy_construct>::value
            || is_direction <Types>::value) ...> {};

    /// Evaluate to \c true iff \a Type is a meta::set of capability keys.
    template <class Type> struct is_capability_keys : boost::mpl::false_ {};

    template <class ... Keys>
        struct is_capability_keys <meta::set <Keys ...>>
    : all_are_capability_keys <Keys ...> {};

    template <class Direction> struct default_construct_direction_equal
    : rime::same_constant <typename std::decay <decltype (
        std::declval <Direction>() == std::declval <Direction>())>::type,
        rime::true_type> {};

    /**
    Specify which directions can be default-constructed without a virtual
    function call.
    This is the case iff comparing two objects of class Direction always returns
    true.
    */
    template <class Direction> struct default_construct_direction
    : boost::mpl::and_ <std::is_constructible <Direction>,
        default_construct_direction_equal <Direction>> {};

    // Default capabilities.
    typedef meta::map <
            meta::map_element <default_direction, direction::front>,
            meta::map_element <direction::front, meta::set <
                empty, chop_destructive>>>
        unique_capabilities;

    typedef meta::map <
            meta::map_element <copy_construct, void>,
            meta::map_element <default_direction, direction::front>,
            meta::map_element <direction::front, meta::set <
                empty, first, drop_one, chop_destructive>>>
        forward_capabilities;

    typedef meta::map <
            meta::map_element <copy_construct, void>,
            meta::map_element <default_direction, direction::front>,
            meta::map_element <direction::front, meta::set <
                empty, first, drop_one, chop_destructive>>,
            meta::map_element <direction::back, meta::set <
                empty, first, drop_one, chop_destructive>>>
        bidirectional_capabilities;

    typedef meta::map <
            meta::map_element <copy_construct, void>,
            meta::map_element <default_direction, direction::front>,
            meta::map_element <direction::front, meta::set <
                empty, size, first, drop_one, drop_n, chop_destructive>>,
            meta::map_element <direction::back, meta::set <
                empty, size, first, drop_one, drop_n, chop_destructive>>>
        random_access_capabilities;

    template <class Range> struct detect_default_direction
    : range::decayed_result_of <callable::default_direction (Range)> {};

    /// Evaluate to \a Capabilities, or if it is void, to forward_capabilities.
    template <class Capabilities> struct normalise_capabilities
    : Capabilities {};

    template <> struct normalise_capabilities <void>
    : forward_capabilities {};

    /* Extracting capability keys. */
    template <class Capabilities> struct extract_capability_keys_implementation;

    template <class ... Keys, class ... Values>
        struct extract_capability_keys_implementation <
            meta::map <meta::map_element <Keys, Values> ...>>
    : meta::set <Keys ...> {};

    /**
    Extract the capability keys from \a Capabilities.
    These keys give the directions, and whether copy construction is possible.
    They do not indicate the default direction.
    */
    template <class Capabilities> struct extract_capability_keys
    : extract_capability_keys_implementation <typename
        meta::remove <default_direction, Capabilities>::type> {};

    /* Detect capabilities from a range. */

    /**
    For an always-empty range, mark all operations as available.
    (They should never be called, because \c empty() returns true.)
    */
    template <class Range, class Direction>
        struct detect_capabilities_for_key_always_empty
    { typedef meta::set <first, drop_one, drop_n, chop_destructive> type; };

    template <class Range, class Direction>
        class detect_capabilities_for_key_not_always_empty
    {
        // Add these in reverse order because they get added to the front.
        // (It does not really matter except for the number of template
        // instantiations and user sanity.)
        typedef typename boost::mpl::if_ <
                boost::mpl::or_ <
                    range::has <callable::chop (Direction, Range &&)>,
                    range::has <callable::chop_in_place (Direction, Range &)>
                >,
                meta::set <chop_destructive>, meta::set<>
            >::type capabilities1;

        typedef typename boost::mpl::eval_if <
                range::has <callable::drop (
                    Direction, std::size_t, Range const &)>,
                meta::push <drop_n, capabilities1>, capabilities1
            >::type capabilities2;

        typedef typename boost::mpl::eval_if <
            range::has <callable::drop (Direction, Range const &)>,
                meta::push <drop_one, capabilities2>, capabilities2
            >::type capabilities3;

    public:
        typedef typename boost::mpl::eval_if <
            range::has <callable::first (Direction, Range const &)>,
                meta::push <first, capabilities3>, capabilities3
            >::type type;
    };

    /**
    Return a meta::set with the capabilities of \a Range in \a Direction.
    If the range is always empty, then first, drop_one, drop_n, and
    chop_destructive are switched on.
    They should never be called, because the range is empty, but with an
    any_range that only turns out to be the case at compile time.
    */
    template <class Range, class Direction>
        class detect_capabilities_for_key
    {
        static_assert (
            std::is_same <Range, typename std::decay <Range>::type>::value,
            "Internal: Range must be unqualified.");

        typedef typename boost::mpl::eval_if <always_empty <Direction, Range>,
            detect_capabilities_for_key_always_empty <Range, Direction>,
            detect_capabilities_for_key_not_always_empty <
                Range, Direction>>::type capabilities1;

        typedef typename boost::mpl::eval_if <
            range::has <callable::size (Direction, Range const &)>,
                meta::insert <size, capabilities1>, capabilities1
            >::type capabilities2;

        typedef typename boost::mpl::eval_if <
            range::has <callable::empty (Direction, Range const &)>,
                meta::insert <empty, capabilities2>, capabilities2
            >::type capabilities3;

    public:
        typedef capabilities3 type;
    };

    template <class Range>
        class detect_capabilities_for_key <Range, copy_construct>
    {
        // Is this error better generated elsewhere?
        static_assert (std::is_constructible <Range, Range const &>::value,
            "The copy construction capability was requested but this range "
            "does not have a copy-constructor.");

    public:
        typedef void type;
    };

    /** \brief
    Return the set \a Directions, with \c copy_consturct prepended if the range
    is copy-constructible.
    */
    template <class Range, class Directions>
        struct detect_copy_construct_key
    : boost::mpl::eval_if <
        std::is_constructible <Range, Range const &>,
        meta::push <copy_construct, Directions>,
        Directions> {};

    /** \brief
    Detect directions for a range.

    This merely takes the Range's default direction and its reverse direction,
    and checks whether they make sense.
    */
    template <class Range> class detect_capability_keys {
        typedef typename detect_default_direction <Range>::type
            default_direction;
        typedef typename result_of_or <
                ::direction::callable::reverse (default_direction), void>::type
            reverse;

        typedef typename boost::mpl::if_ <
                std::is_same <reverse, void>,
                meta::vector <default_direction>,
                meta::vector <default_direction, reverse>>::type
            potential_directions;

        template <class Direction> struct uses_direction
        : boost::mpl::or_ <
            always_empty <Direction, Range>,
            has <callable::first (Direction, Range &&)>> {};

        typedef typename meta::as_set <meta::filter <
                uses_direction <boost::mpl::_1>, potential_directions>>::type
            directions;

    public:
        /*typedef typename boost::mpl::eval_if <
            std::is_constructible <Range, Range const &>,
            meta::push <copy_construct, directions>, directions>::type type;*/
        typedef typename detect_copy_construct_key <Range, directions>::type
            type;
    };

    /** \brief
    Detect capabilities for an underlying range.

    \tparam Range The range of interest.
    \tparam CapabilityKeys
        (optional)
        The capability keys to include, as a meta::set.
        If this is not given, it is detected as the default direction and
        potentially its reverse.
    */
    template <class Range, class CapabilityKeys =
            typename detect_capability_keys <Range>::type>
        struct detect_capabilities;

    template <class Range, class ... CapabilityKeys>
        struct detect_capabilities <Range, meta::set <CapabilityKeys ...>>
    : meta::map <
        meta::map_element <default_direction, typename
            detect_default_direction <Range>::type>,
        meta::map_element <CapabilityKeys, typename
            detect_capabilities_for_key <Range, CapabilityKeys>::type> ...> {};

    /** \brief
    Evaluate to \c true iff all capabilities in \a Subset are present in
    \a Superset.

    In practical terms: an any_range with \a Superset can be used where
    \a Subset is required.

    This is true if all keys in the subset exist in the superset, and their
    values are the same, or the values are sets with the left value a subset of
    the right one.
    */
    template <class Subset, class Superset> struct is_subset;

    template <class Value1, class LazyValue2> struct is_subset_capability
    : std::is_same <Value1, typename LazyValue2::type> {};

    template <class ... Elements1, class LazyValue2>
        struct is_subset_capability <
            meta::set <Elements1 ...>, LazyValue2>
    : meta::is_subset <meta::set <Elements1 ...>, typename LazyValue2::type> {};

    template <class ... Keys1, class ... Values1, class Superset>
        struct is_subset <
            meta::map <meta::map_element <Keys1, Values1> ...>, Superset>
    // Use all_of <vector < ...>> to ensure lazy evaluation.
    : meta::all_of <meta::vector <
        meta::has_key <Keys1, Superset> ...,
        is_subset_capability <Values1, meta::at <Keys1, Superset>> ...>> {};

}} // namespace range::capability

#endif // RANGE_ANY_RANGE_CAPABILITY_HPP_INCLUDED

