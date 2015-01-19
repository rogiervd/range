/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_ZIP_HPP_INCLUDED
#define RANGE_ZIP_HPP_INCLUDED

#include <type_traits>
#include <utility>

#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

#include "meta/all.hpp"
#include "meta/vector.hpp"

#include "utility/disable_if_same.hpp"
#include "utility/returns.hpp"

#include "rime/min.hpp"

#include "core.hpp"

#include "detail/underlying.hpp"
#include "detail/with_direction.hpp"

#include "tuple.hpp"
#include "element_types.hpp"
#include "curry.hpp"
#include "any.hpp"
#include "transform.hpp"
#include "fold.hpp"

namespace range {

template <class Direction, class ... Ranges> class zip_range;

namespace apply {

    namespace automatic_arguments {

        // zip.
        template <class Directions, class Other, class Ranges,
            class Enable = void>
        struct zip : operation::unimplemented {};

        template <class Direction, class ... Ranges>
            struct zip <meta::vector <Direction>, meta::vector<>,
                meta::vector <Ranges ...>>
        {
            /*
            This is implemented here, because zip should not really be
            specialisable.
            Otherwise what would we do with zip_from?
            */
            zip_range <Direction, typename std::decay <Ranges>::type ...>
                operator() (Direction const & direction, Ranges && ... ranges)
                    const
            {
                return zip_range <
                    Direction, typename std::decay <Ranges>::type ...> (
                        direction, std::forward <Ranges> (ranges) ...);
            }
        };

    } // namespace automatic_arguments

    /** zip */
    template <class ... Arguments> struct zip
    : automatic_arguments::categorise_arguments_default_direction <
        automatic_arguments::call_with_view <
            automatic_arguments::zip>::apply,
        meta::vector <Arguments ...>>::type {};

} // namespace apply

namespace callable {
    struct zip : generic <apply::zip> {};
} // namespace callable

/** \brief
View a number of ranges in parallel.

This returns a range whose first element is a tuple with all the first element
of the ranges passed in; the second element is a tuple of the second elements;
et cetera.
The types contained in the tuples are the exact return types of first() on the
underlying ranges.
Thus, they may well be references to the elements of the underlying ranges.
This means that mutable operations are possible.

The range that is return by \c zip is empty if any of the underlying ranges are
empty.
This means that the range has the size equal to the shortest of the underlying
ranges.

The range that is returned by \c zip is assignable.
It can therefore be used to iterate over a tuple of homogeneous ranges.

Example:
\code
std::vector <int> vi {1, 2, 3};
std::vector <double> vd {4.5, 5.5};
auto zipped = zip (vi, vd);
tuple <int, double> f = first (zipped);
assert (first (f) == 1);
assert (second (f) == 4.5);

// Copy values from vi to vd.
for (; !empty (zipped); zipped = drop (zipped)) {
    tuple <int &, double &> element = first (zipped);
    second (element) = first (element);
}
assert (first (vd) == 1.0);
assert (second (vd) == 2.0);
\endcode

\param direction
    (optional_
    The direction of traversal through each of the underlying ranges.
    If the direction is not given, the default direction of the first range is
    used.
\param ranges
    (variadic)
    The underlying ranges to traverse.
*/
static const auto zip = callable::zip();

namespace zip_from_detail {

    template <class Direction, class RangeTuple, class Types
        = typename meta::as_vector <element_types <RangeTuple>>::type>
    struct zip_range_from;

    template <class Direction, class RangeTuple, class ... Types>
        struct zip_range_from <Direction, RangeTuple, meta::vector <Types ...>>
    {
        typedef zip_range <Direction,
            typename std::decay <typename
                std::result_of <callable::view (Direction, Types)>::type
            >::type ...> type;
    };

} // namespace zip_from_detail

/** \bried
Return a zipped range over the ranges in \a range_tuple.

\a range_tuple must be a fixed-length range containing the ranges to traverse.
<c>zip_from (direction, make_tuple (range1, range2))</c> is equivalent to
<c>zip (direction, range1, range2)</c>.
\a range_tuple itself will be traversed in direction \ref front to extract the
ranges.

\param direction
    (optional)
    The direction in which the underlying ranges are to be traversed.
    If the direction is not given, the default direction of the first range is
    used.
\param range_tuple
    The tuple (or similar range) with underlying ranges to traverse.
*/
template <class Direction, class RangeTuple> inline
    auto zip_from (Direction const & direction, RangeTuple && range_tuple)
RETURNS (typename zip_from_detail::zip_range_from <Direction, RangeTuple>::type
    (direction, transform (front,
        curry::view (direction), std::forward <RangeTuple> (range_tuple))));

/// \cond DONT_DOCUMENT
template <class RangeTuple> inline
    auto zip_from (RangeTuple && range_tuple)
RETURNS (zip_from (default_direction (first (range_tuple)),
    std::forward <RangeTuple> (range_tuple)));
/// \endcond

/* zip_range. */

/** \brief
Range that presents a tuple of ranges as a range of tuples.

Direction is the direction in which the underlying ranges will be traversed.
The underlying ranges are kept in a tuple.
The type returned is also tuple.
*/
template <class Direction, class ... Ranges> class zip_range
: public detail::with_default_direction <Direction>
{
public:
    typedef meta::vector <Ranges ...> range_types;
    typedef range::tuple <Ranges ...> underlying_type;

    static_assert (meta::all <meta::vector <
            std::is_same <Ranges, typename std::decay <Ranges>::type> ...
        >>::value,
        "All range types must be unqualified.");

    static_assert (meta::all <meta::vector <is_range <Ranges> ...>>::value,
        "All ranges must actually be ranges.");

    static_assert (
        meta::all <meta::vector <is_view <Direction, Ranges> ...>>::value,
        "All ranges must be views in Direction.");

private:
    typedef detail::with_default_direction <Direction> with_direction_type;
    range::tuple <Ranges ...> underlying_;
    friend class detail::callable::get_underlying;

public:
    template <class ... Arguments, class Enable = typename
        utility::disable_if_variadic_same_or_derived <
            zip_range, Direction, Arguments ...>::type>
    zip_range (Direction const & direction, Arguments && ... arguments)
    : detail::with_default_direction <Direction> (direction),
        underlying_ (std::forward <Arguments> (arguments) ...) {}

    range::tuple <Ranges ...> const & underlying() const { return underlying_; }
    range::tuple <Ranges ...> & underlying() { return underlying_; }

private:
    friend class operation::member_access;

    // Makes decltype complain.
    auto empty (Direction const & direction) const
    -> decltype (range::any (range::transform (
        curry::empty (std::declval <Direction>()),
        std::declval <underlying_type>())))
    {
        return range::any (range::transform (
            curry::empty (this->direction_must_be_equal (direction)),
            underlying_));
    }

    // size.
    /*
    Summarise the range by applying a binary function to elements of the
    range recursively until there is only one left.
    Maybe there should be a general version of this.
    */
    struct reduce {
        template <class Function, class Range> auto
            operator() (Function && function, Range && range) const
        RETURNS (range::fold (std::forward <Function> (function),
            range::first (range),
            range::drop (std::forward <Range> (range))));
    };

    /*
    We need the size of the shortest of the ranges.
    Take the size of each (using range::transform), and reduce it with the
    "min" function.

    Only enable this when all ranges implement "size".
    If not, keep the compiler from figuring out the return type, by pretending
    there is a type Direction2 which is different from Direction.
    */
    template <class Direction2,
        class Enable1 = typename boost::enable_if <
            std::is_same <Direction2, Direction>>::type,
        class Enable2 = typename boost::enable_if <meta::all <meta::vector <
            has <callable::size (Direction2, Ranges)> ...>>>::type>
    auto size (Direction2 const & direction) const
    RETURNS (reduce() (rime::min,
        range::transform (curry::size (direction), underlying_)));
};

template <class Direction> struct zip_range_tag;

template <class Direction, class ... Ranges>
    struct tag_of_qualified <zip_range <Direction, Ranges ...>>
{ typedef zip_range_tag <Direction> type; };

namespace operation {

    // first.
    template <class Direction, class ZipRange>
        struct first <zip_range_tag <Direction>, Direction, ZipRange>
    {
        auto operator() (Direction const & direction, ZipRange && r) const
        RETURNS (range::copy_tuple_from (range::transform (
            curry::first (r.direction_must_be_equal (direction)),
            range::view_once (range::detail::get_underlying (
                std::forward <ZipRange> (r))))));
    };

    namespace zip_detail {

        /* drop_implemented_all. */
        template <class Direction, class Increment, class ZipRange, class Ranges
                = typename std::decay <ZipRange>::type::range_types>
            struct drop_implemented_all;

        // Lvalue.
        template <class Direction, class Increment, class ZipRange,
                class ... Ranges>
            struct drop_implemented_all <Direction, Increment, ZipRange &,
                meta::vector <Ranges ...>>
        : meta::all <meta::vector <has <
            callable::drop (Direction, Increment, Ranges const &)> ...>> {};

        // Rvalue.
        template <class Direction, class Increment, class ZipRange,
                class ... Ranges>
            struct drop_implemented_all <Direction, Increment, ZipRange &&,
                meta::vector <Ranges ...>>
        : meta::all <meta::vector <has <
            callable::drop (Direction, Increment, Ranges &&)> ...>> {};

        /* chop_implemented_all. */
        template <class Direction, class ZipRange, class Ranges
                = typename std::decay <ZipRange>::type::range_types>
            struct chop_implemented_all;

        // Lvalue.
        template <class Direction, class ZipRange, class ... Ranges>
            struct chop_implemented_all <Direction, ZipRange &,
                meta::vector <Ranges ...>>
        : meta::all <meta::vector <has <
            callable::chop (Direction, Ranges const &)> ...>> {};

        // Rvalue.
        template <class Direction, class ZipRange, class ... Ranges>
            struct chop_implemented_all <Direction, ZipRange &&,
                meta::vector <Ranges ...>>
        : meta::all <meta::vector <has <
            callable::chop (Direction, Ranges &&)> ...>> {};

    } // namespace zip_detail

    // drop.
    template <class Direction, class Increment, class ZipRange>
        struct drop <zip_range_tag <Direction>,
            Direction, Increment, ZipRange, typename boost::enable_if <
                zip_detail::drop_implemented_all <
                    Direction, Increment, ZipRange>>::type>
    {
        auto operator() (Direction const & direction,
            Increment const & increment, ZipRange && r) const
        RETURNS (range::zip_from (direction, range::transform (
            curry::drop (direction, increment),
            range::view_once (range::detail::get_underlying (
                std::forward <ZipRange> (r))))));
    };

    // chop.
    // Lvalue.
    template <class Direction, class ZipRange>
        struct chop <zip_range_tag <Direction>,
            Direction, ZipRange &, typename boost::enable_if <
                zip_detail::chop_implemented_all <Direction, ZipRange &>>::type>
    {
        // If chop is implemented for lvalues, so are first and drop.
        // Using first and drop may be faster, and it is definitely easier.
        typedef chopped <
            typename range::result_of <range::callable::first (
                Direction, ZipRange)>::type,
            typename range::result_of <range::callable::drop (
                Direction, ZipRange)>::type> result_type;

        result_type operator() (Direction const & direction, ZipRange r) const {
            return result_type (range::first (direction, r),
                range::drop (direction, r));
        }
    };

    // Rvalue.
    template <class Direction, class ZipRange>
        struct chop <zip_range_tag <Direction>, Direction, ZipRange &&,
            typename boost::enable_if <zip_detail::chop_implemented_all <
                Direction, ZipRange &&>>::type>
    {
        struct move_first {
            template <class First, class Rest>
                First && operator() (chopped <First, Rest> & c) const
            { return c.move_first(); }
        };

        struct move_rest {
            template <class First, class Rest>
                Rest && operator() (chopped <First, Rest> & c) const
            { return c.move_rest(); }
        };

        template <class ... Ranges,
            class NewFirst = tuple <typename std::result_of <
                range::callable::first (Direction, Ranges)>::type ...>,
            class NewRest = zip_range <Direction, typename
                std::decay <typename std::result_of <range::callable::drop (
                    Direction, Ranges)>::type>::type ...>,
            class Result = chopped <NewFirst, NewRest>
        >
        Result operator() (Direction const & direction,
            zip_range <Direction, Ranges ...> && r) const
        {
            // Apply "chop" to each of the ranges and store the result as
            // tuple <chopped <first1, rest1>, chopped <first2, rest2>, ...>
            auto chopped = make_tuple_from (
                range::transform (curry::chop (direction),
                    range::view_once (std::move (r.underlying()))));
            // Produce underlying with rvalue references to the "first"
            // elements first1 &&, first2 &&, ...
            auto first = range::transform (move_first(), chopped);
            // ... and the "rest" elements
            // rest1 &&, rest2 &&, ....
            auto rest = range::transform (move_rest(), chopped);

            return Result (std::move (first),
                NewRest (direction, std::move (rest)));
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_ZIP_HPP_INCLUDED
