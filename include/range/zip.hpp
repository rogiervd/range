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

#ifndef RANGE_ZIP_HPP_INCLUDED
#define RANGE_ZIP_HPP_INCLUDED

#include <type_traits>
#include <utility>

#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

#include "meta/all_of_c.hpp"
#include "meta/vector.hpp"

#include "utility/disable_if_same.hpp"
#include "utility/returns.hpp"

#include "rime/min.hpp"

#include "core.hpp"

#include "helper/underlying.hpp"
#include "helper/with_direction.hpp"

#include "tuple.hpp"
#include "element_types.hpp"
#include "lazy.hpp"
#include "any_of.hpp"
#include "transform.hpp"
#include "fold.hpp"

namespace range {

template <class Direction, class ... Ranges> class zip_range;

namespace callable {

    class zip {
    public:
        template <class FirstRange, class ... RestRanges,
            class Enable = typename std::enable_if <meta::all_of_c <
                is_range <FirstRange>::value,
                is_range <RestRanges>::value ...>::value>::type,
            class DefaultDirection = typename decayed_result_of <
                default_direction (FirstRange)>::type,
            class Result = zip_range <DefaultDirection,
                typename decayed_result_of <
                    view (FirstRange, DefaultDirection)>::type,
                typename decayed_result_of <
                    view (RestRanges, DefaultDirection)>::type ...>>
        Result operator() (
            FirstRange && first_range, RestRanges && ... rest_ranges) const
        {
            auto direction = range::default_direction (first_range);
            return Result (direction,
                range::view (std::forward <FirstRange> (first_range),
                    direction),
                range::view (std::forward <RestRanges> (rest_ranges),
                    direction) ...);
        }
    };

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

The range that is returned by \c zip is empty if any of the underlying
are empty.
This means that the range has the size equal to the shortest of the underlying
ranges.

The range that is returned by \c zip is assignable.
It can therefore be used to iterate over a tuple of homogeneous ranges.

The underlying ranges are traversed over the default direction of the first
range.

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

\param ranges
    (variadic)
    The underlying ranges to traverse.
*/
static const auto zip = callable::zip();

namespace zip_from_detail {

    template <class RangeTuple, class Direction, class Types
        = typename meta::as_vector <element_types <RangeTuple>>::type>
    struct zip_range_from;

    template <class RangeTuple, class Direction, class ... Types>
        struct zip_range_from <RangeTuple, Direction, meta::vector <Types ...>>
    {
        typedef zip_range <Direction,
            typename std::decay <typename
                result_of <callable::view (Types, Direction)>::type
            >::type ...> type;
    };

} // namespace zip_from_detail

/** \brief
Return a zipped range over the ranges in \a range_tuple.

\a range_tuple must be a fixed-length range containing the ranges to traverse.
<c>zip_from (make_tuple (range1, range2))</c> is equivalent to
<c>zip (range1, range2)</c>.
\a range_tuple itself will be traversed in direction \ref front to extract the
ranges.

\param range_tuple
    The tuple (or similar range) with underlying ranges to traverse.
\param direction
    (optional)
    The direction in which the underlying ranges are to be traversed.
    If the direction is not given, the default direction of the first range is
    used.
*/
template <class RangeTuple, class Direction,
    class Enable1 = typename
        std::enable_if <is_range <RangeTuple>::value>::type,
    class Enable2 = typename
        std::enable_if <is_direction <Direction>::value>::type,
    class Result = typename zip_from_detail::zip_range_from <
        RangeTuple, Direction>::type>
inline Result zip_from (RangeTuple && range_tuple, Direction const & direction)
{
    return Result (direction,
        transform (std::forward <RangeTuple> (range_tuple),
            lazy::view (direction), front));
}

/// \cond DONT_DOCUMENT
template <class RangeTuple,
    class Enable = typename std::enable_if <is_range <RangeTuple>::value>::type>
inline auto zip_from (RangeTuple && range_tuple)
RETURNS (zip_from (std::forward <RangeTuple> (range_tuple),
    default_direction (first (range_tuple))));
/// \endcond

/* zip_range. */

/** \brief
Range that presents a tuple of ranges as a range of tuples.

Direction is the direction in which the underlying ranges will be traversed.
The underlying ranges are kept in a tuple.
The type returned is also tuple.
*/
template <class Direction, class ... Ranges> class zip_range
: public helper::with_default_direction <Direction>
{
public:
    typedef meta::vector <Ranges ...> range_types;
    typedef range::tuple <Ranges ...> underlying_type;

    static_assert (meta::all_of_c <
            std::is_same <Ranges, typename std::decay <Ranges>::type>::value ...
        >::value,
        "All range types must be unqualified.");

    static_assert (meta::all_of_c <is_range <Ranges>::value ...>::value,
        "All ranges must actually be ranges.");

    static_assert (
        meta::all_of_c <is_view <Ranges, Direction>::value ...>::value,
        "All ranges must be views in Direction.");

private:
    typedef helper::with_default_direction <Direction> with_direction_type;
    range::tuple <Ranges ...> underlying_;
    template <class Wrapper> friend class helper::callable::get_underlying;

public:
    template <class ... Arguments, class Enable = typename
        utility::disable_if_variadic_same_or_derived <
            zip_range, Direction, Arguments ...>::type>
    zip_range (Direction const & direction, Arguments && ... arguments)
    : helper::with_default_direction <Direction> (direction),
        underlying_ (std::forward <Arguments> (arguments) ...) {}

    range::tuple <Ranges ...> const & underlying() const { return underlying_; }
    range::tuple <Ranges ...> & underlying() { return underlying_; }

private:
    friend class helper::member_access;

    // Makes decltype complain.
    auto empty (Direction const & direction) const
    -> decltype (range::any_of (range::transform (
        std::declval <underlying_type>(),
        lazy::empty (std::declval <Direction>()))))
    {
        return range::any_of (range::transform (underlying_,
            lazy::empty (this->direction_must_be_equal (direction))));
    }

    // size.
    /*
    Summarise the range by applying a binary function to elements of the
    range recursively until there is only one left.
    Maybe there should be a general version of this.
    */
    struct reduce {
        template <class Range, class Function> auto
            operator() (Range && range, Function && function) const
        RETURNS (range::fold (
            range::first (range),
            range::drop (std::forward <Range> (range)),
            front, std::forward <Function> (function)));
    };

    /*
    We need the size of the shortest of the ranges.
    Take the size of each (using range::transform), and reduce it with the
    "min" function.

    Only enable this when all ranges implement "size".
    "transform" does not enable this automatically.
    If not, keep the compiler from figuring out the return type, by pretending
    there is a type Direction2 which is different from Direction.
    */
    template <class Direction2,
        class Enable1 = typename boost::enable_if <
            std::is_same <Direction2, Direction>>::type,
        class Enable2 = typename boost::enable_if <meta::all_of_c <
            has <callable::size (Ranges, Direction2)>::value ...>>::type>
    auto size (Direction2 const & direction) const
    RETURNS (reduce() (
        range::transform (underlying_, lazy::size (direction)), rime::min));
};

namespace zip_operation {
    template <class Direction> struct zip_range_tag {};
} // namespace zip_operation

template <class Direction, class ... Ranges>
    struct tag_of_qualified <zip_range <Direction, Ranges ...>>
{ typedef zip_operation::zip_range_tag <Direction> type; };

/* Operations. */

namespace zip_operation {

    namespace zip_detail {

        /*
        first and drop, and therefore chop, essentially apply the same operation
        on each of the components of the zip_range.
        This uses transform(), which (by design) is defined even if not all the
        elements of the range can be transformed with a given function.
        Therefore, this must be checked explicitly.
        */

        /* implemented_all. */
        template <class Callable, class ZipRange, class Direction, class Ranges
                = typename std::decay <ZipRange>::type::range_types>
            struct implemented_all;

        // Lvalue.
        template <class Callable, class ZipRange, class Direction,
                class ... Ranges>
            struct implemented_all <Callable, ZipRange, Direction,
                meta::vector <Ranges ...>>
        : meta::all_of_c <has <Callable (Ranges const &, Direction)>::value ...>
        {};

        // Rvalue.
        template <class Callable, class ZipRange, class Direction,
                class ... Ranges>
            struct implemented_all <Callable, ZipRange &&, Direction,
                meta::vector <Ranges ...>>
        : meta::all_of_c <has <Callable (Ranges &&, Direction)>::value ...> {};

        // first_implemented_all.
        template <class ZipRange, class Direction>
            struct first_implemented_all
        : implemented_all <callable::first, ZipRange, Direction> {};

        // chop_implemented_all.
        template <class ZipRange, class Direction>
            struct chop_implemented_all
        : implemented_all <callable::chop, ZipRange, Direction> {};


        /* drop_implemented_all. */
        // This has an additional parameter Increment, so it must be defined
        // separately.
        template <class ZipRange, class Increment, class Direction, class Ranges
                = typename std::decay <ZipRange>::type::range_types>
            struct drop_implemented_all;

        // Lvalue.
        template <class ZipRange, class Increment, class Direction,
                class ... Ranges>
            struct drop_implemented_all <ZipRange &, Increment, Direction,
                meta::vector <Ranges ...>>
        : meta::all_of_c <has <callable::drop (
            Ranges const &, Increment, Direction)>::value ...> {};

        // Rvalue.
        template <class ZipRange, class Increment, class Direction,
                class ... Ranges>
            struct drop_implemented_all <ZipRange &&, Increment, Direction,
                meta::vector <Ranges ...>>
        : meta::all_of_c <has <
            callable::drop (Ranges &&, Increment, Direction)>::value ...> {};


        // Callables to use on "chopped".
        // These return rvalue references.
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

    } // namespace zip_detail

    // first.
    template <class Direction, class ZipRange,
        class Enable = typename boost::enable_if <
            zip_detail::first_implemented_all <ZipRange &&, Direction>>::type>
        inline auto implement_first (zip_range_tag <Direction> const &,
            ZipRange && r, Direction const & direction)
    RETURNS (copy_tuple_from (transform (
        view_once (helper::get_underlying <ZipRange> (r)),
        lazy::first (r.direction_must_be_equal (direction)))));

    // drop.
    template <class Direction, class ZipRange, class Increment,
        class Enable = typename boost::enable_if <
            zip_detail::drop_implemented_all <
                ZipRange &&, Increment, Direction>>::type>
        inline auto implement_drop (zip_range_tag <Direction> const &,
            ZipRange && r, Increment const & increment,
            Direction const & direction)
    RETURNS (zip_from (transform (
            view_once (helper::get_underlying <ZipRange> (r)),
            lazy::drop (increment, direction)),
            direction));

    // chop.
    // Lvalue.
    template <class Direction, class ZipRange,
        class Enable = typename boost::enable_if <
            zip_detail::chop_implemented_all <ZipRange &, Direction>>::type,
        // If chop is implemented for lvalues, so are first and drop.
        // Using first and drop may be faster, and it is definitely easier.
        class Result = chopped <
            decltype (range::callable::first_direct() (
                std::declval <ZipRange const &>(),
                std::declval <Direction>())),
            decltype (range::callable::drop_direct() (
                std::declval <ZipRange const &>(),
                std::declval <Direction>()))>>
    inline Result implement_chop (zip_range_tag <Direction> const &,
        ZipRange const & r, Direction const & direction)
    {
        r.direction_must_be_equal (direction);
        return Result (range::first (r, direction), range::drop (r, direction));
    }

    // Rvalue.
    template <class Direction, class ... Ranges,
        class Enable = typename boost::enable_if <
            zip_detail::chop_implemented_all <
                zip_range <Direction, Ranges ...> &&, Direction>>::type,
        class NewFirst = tuple <typename result_of <
            range::callable::first (Ranges, Direction)>::type ...>,
        class NewRest = zip_range <Direction, typename decayed_result_of <
            range::callable::drop (Ranges, Direction)>::type ...>,
        class Result = chopped <NewFirst, NewRest>>
    inline Result implement_chop (zip_range_tag <Direction> const &,
        zip_range <Direction, Ranges ...> && r, Direction const & direction)
    {
        r.direction_must_be_equal (direction);
        // Apply "chop" to each of the ranges and store the result as
        // tuple <chopped <first1, rest1>, chopped <first2, rest2>, ...>
        auto chopped = make_tuple_from (
            transform (view_once (std::move (r.underlying())),
                lazy::chop (direction)));
        // Produce underlying with rvalue references to the "first"
        // elements first1 &&, first2 &&, ...
        auto first = transform (chopped, zip_detail::move_first());
        // ... and the "rest" elements
        // rest1 &&, rest2 &&, ....
        auto rest = transform (chopped, zip_detail::move_rest());

        return Result (std::move (first),
            NewRest (direction, std::move (rest)));
    }

} // namespace zip_operation

} // namespace range

#endif // RANGE_ZIP_HPP_INCLUDED
