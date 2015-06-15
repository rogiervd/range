/*
Copyright 2011-2015 Rogier van Dalen.

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

#ifndef RANGE_DETAIL_CORE_VIEW_HPP_INCLUDED
#define RANGE_DETAIL_CORE_VIEW_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/and.hpp>
#include <boost/mpl/placeholders.hpp>

#include "meta/all_of_c.hpp"

#include "rime/core.hpp"

#include "../direction.hpp"

namespace range {

namespace helper {

    /** \brief
    Produce a lightweight range, a "view", on a heavyweight range such as a
    container.

    The resulting type must be assignable without changing any underlying
    container.
    If possible, the resulting type should be homogeneous.
    make_view must not be defined for the resulting type.
    If the range is already a lightweight range, then make_view should not be
    defined for it.
    view (range, directions...) will then return range unchanged.

    If \a once is true, then view_once was called, and each element in the view
    can be assumed to be seen only once.
    Therefore, if a container is passed in that owns the elements, they can be
    viewed as rvalue references.

    Not that even when an rvalue is passed in, the view should not contain a
    copy of the heavyweight range.
    The view should still assume that the reference remains usable as long as
    the view is used.

    \tparam once A boolean constant.
        Iff \c true, assume that each element in the view will be seen
        only once, and it is possible to pilfer the underlying container.
    \tparam tag The range tag.
    \tparam range The range itself, qualified (as an rvalue reference if an
        rvalue).
    \tparam directions The directions that the range should be viewed in.
    */
    void implement_make_view (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_make_view;

        template <bool Once, bool Forward> struct view {
        private:
            struct dispatch {
                template <class Range, class ... Directions>
                    auto operator() (overload_order <1> *,
                        Range && range, Directions const & ... directions) const
                RETURNS (implement_make_view (typename tag_of <Range>::type(),
                    rime::bool_ <Once>(),
                    std::forward <Range> (range), directions ...));

                // If implement_make_view is not implemented, and empty() is
                // implemented for each of the directions, then return the
                // range as-is.
                template <class Range, class ... Directions, class Enable =
                    meta::vector <decltype (std::declval <empty>() (
                        std::declval <Range>(), std::declval <Directions>()))...
                    >>
                    typename std::conditional <Forward, Range &&, Range>::type
                    operator() (overload_order <16> *,
                        Range && range, Directions const & ... directions) const
                { return std::forward <Range> (range); }
            };

        public:
            // At least one direction.
            template <class Range, class FirstDirection,
                class ... OtherDirections, class Enable =
                    typename std::enable_if <meta::all_of_c <
                        is_range <Range>::value,
                        is_direction <FirstDirection>::value,
                        is_direction <OtherDirections>::value ...
                    >::value>::type>
            auto operator() (Range && range,
                FirstDirection const & first_direction,
                OtherDirections const & ... other_directions) const
            RETURNS (dispatch() (pick_overload(),
                std::forward <Range> (range),
                first_direction, other_directions ...));

            // Without any direction: use default direction.
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range && range) const
            RETURNS (dispatch() (pick_overload(),
                std::forward <Range> (range),
                range::default_direction (range)));
        };

    } // namespace implementation

    typedef implementation::view <false, false> view;
    typedef implementation::view <false, true> forward_view;
    typedef implementation::view <true, false> view_once;

} // namespace callable

/** \brief
Turn a range into a view.

A view is lightweight.
If the range is already a view, then the range itself is returned.
If an rvalue reference to a view is passed in, then the unqualified view is
returned.

\param range
    Range that the view should be over.
\param directions
    Directions that the view should allow traversal in.
    If no direction is given, then the default direction is used.
*/
static const auto view = callable::view();

/** \brief
Turn a range into a view.

A view is lightweight.
If the range is already a view, then the range itself is returned.
Unlike for view(), the resulting type can be an rvalue reference.
This means that you need to make sure that if a temporary can be passed in, the
temporary is not destructed until the result of this function goes out of scope.
Alternatively, use view(), which is safer.

\param range
    Range that the view should be over.
\param directions
    Directions that the view should allow traversal in.
    If no direction is given, then the default direction is used.
*/
static const auto forward_view = callable::forward_view();

/** \brief
Turn a range into a view that can be viewed once.

If the range is a temporary, the elements of the view can be rvalue references.
That is, the elements can be moved.
The caller must therefore only access each element only once (or have knowledge
particular to the view).

\param range
    Range that the view should be over.
\param directions
    Directions that the view should allow traversal in.
    If no direction is given, then the default direction is used.
*/
static const auto view_once = callable::view_once();

/* is_view */

namespace is_view_detail {

    template <class Range, class ... Directions> struct is_view
    : std::is_same <
        typename std::decay <Range>::type,
        typename std::decay <typename result_of <
            callable::view (Range, Directions ...)>::type>::type
    > {};

} // namespace is_view_detail

/** \brief
Metafunction that returns true iff Range is a view.

That is, <c>view (range, directions...)</c> returns range itself.

\tparam Range The range.
\tparam Directions Zero or more directions.
    If no directions are given, the range's default direction is used.
*/
template <class Range, class ... Directions> struct is_view
: boost::mpl::and_ <
    is_range <Range>,
    is_view_detail::is_view <Range, Directions ...>> {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_VIEW_HPP_INCLUDED
