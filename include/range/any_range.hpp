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

#ifndef RANGE_ANY_RANGE_HPP_INCLUDED
#define RANGE_ANY_RANGE_HPP_INCLUDED

#include <memory>
#include <type_traits>

#include "meta/contains.hpp"
#include "meta/all_of_c.hpp"

#include "utility/returns.hpp"
#include "utility/disable_if_same.hpp"

#include "core.hpp"
#include "any_range/capability.hpp"
#include "any_range/interface.hpp"
#include "any_range/implementation.hpp"

namespace range {

/** \brief
A homogeneous range that hides the type of the underlying range.

It wraps any range that has elements of a specific type, and the capabilities
given.
any_range is homogeneous.
The underlying range's elements must be convertible to the any_range's type.
(If they are not, using range::transform with a function that performs the
conversion can help.)

Most operations require a virtual method call, so performance is decreased
compared to using the underlying range directly.
Additionally, some operations require a heap allocation.
chop_in_place can be quite efficient if it is implemented on the underlying
range.
drop(), and chop() with an lvalue, however, need to allocate the new underlying
range, and can be slow.

Implicit conversion between different any_range types is possible as long as
they have the same Element type and the list of target capabilities is a subset
of the source capabilities.
This conversion can involve a number of memory allocations.
If the Element types are different, then conversion is also possible but
explicit, since it works as with any other range type.
It causes an additional layer of virtual calls and allocations, so this is
something to be careful with.

\tparam Element
    The type that \c first(r) returns.
\tparam Capabilities (optional)
    A meta::map with meta::map_element types indicating the capabilities of
    the underlying range.
    One of these must have key range::capability::default_direction, with the
    default direction as the value.
    Another will often have key range::capability::copy_construct, with a void
    value, to indicate that the underlying range (and therefore the any_range)
    supports copy construction.

    Each of the other map_element should have a direction as a key, and a
    meta::set of available capabilities for that as the value.
    These classes are defined in namespace range::capabilities:
    \li range::capability::empty,
    \li range::capability::size,
    \li range::capability::first,
    \li range::capability::drop_one,
    \li range::capability::drop_n,
    \li range::capability::chop_destructive.

    There are also predefined meta::map types for some types of ranges, all with
    direction::front as the default direction:
    \li range::capability::unique_capabilities,
    \li range::capability::forward_capabilities,
    \li range::capability::bidirectional_capabilities,
    \li range::capability::random_access_capabilities.

    If this parameter is not given, range::capability::forward_capabilities
    is used.
*/
template <class Element, class Capabilities = void> class any_range;

template <class Capabilities> struct any_range_tag;

template <class Element, class Capabilities>
    struct tag_of_qualified <any_range <Element, Capabilities>>
{
    typedef any_range_tag <typename
        capability::normalise_capabilities <Capabilities>::type> type;
};

template <class Element, class Capabilities> class any_range {
public:
    typedef typename capability::normalise_capabilities <Capabilities>::type
        capabilities;
    typedef typename meta::at <capability::default_direction, capabilities
        >::type default_direction_type;
    typedef typename capability::extract_capability_keys <capabilities>::type
        capability_keys;
private:
    typedef any_range_interface::interface <
        Element, capability_keys, default_direction_type> interface_type;
    typedef std::unique_ptr <interface_type> interface_ptr;

    /**
    Pointer to implementation.
    It is always non-null, except if the object has been moved.
    */
    interface_ptr implementation_;

    template <class Element2, class Capabilities2> friend class any_range;

    template <class Underlying>
        interface_ptr wrap_view (Underlying && underlying) const
    {
        typedef any_range_implementation::implementation <Element,
                capability_keys, typename std::decay <Underlying>::type>
            implementation;
        // implementation::capabilities now contains the actual capabilities.
        // The required capabilities, in "capabilities", must be a subset of
        // those.
        static_assert (capability::is_subset <
            capabilities, typename implementation::capabilities>::value,
            "Required capabilities must be subset of available ones.");

        return utility::make_unique <implementation> (
            std::forward <Underlying> (underlying));
    }

    template <class OtherCapabilities>
        struct any_range_is_convertible
    : capability::is_subset <capabilities, typename
        capability::normalise_capabilities <OtherCapabilities>::type> {};

    static_assert (any_range_is_convertible <Capabilities>::value,
        "Sanity: interface must be convertible to itself.");

    template <class OtherCapabilities> struct convert_from {
        typedef typename any_range <Element, OtherCapabilities>::capability_keys
            other_capability_keys;

        typedef any_range_interface::convert_interface <
            interface_ptr, capability_keys, other_capability_keys> convert;

        interface_ptr operator() (
            any_range <Element, OtherCapabilities> const & other) const
        { return convert() (other.implementation_); }

        interface_ptr operator() (
            any_range <Element, OtherCapabilities> && other) const
        { return convert() (std::move (other.implementation_)); }
    };

    template <class Type> struct is_any_range_with_same_element_type_impl
    : boost::mpl::false_ {};

    template <class Capabilities2>
        struct is_any_range_with_same_element_type_impl <
            any_range <Element, Capabilities2>>
    : boost::mpl::true_ {};

    template <class Type> struct is_any_range_with_same_element_type
    : is_any_range_with_same_element_type_impl <
        typename std::decay <Type>::type> {};

    struct not_constructible;
    typedef typename boost::mpl::if_ <
        meta::has_key <capability::copy_construct, capability_keys>,
        any_range, not_constructible>::type any_range_if_copy_constructible;

    /**
    Internal constructor.
    */
    any_range (interface_ptr && implementation)
    : implementation_ (std::move (implementation)) {}

public:
    /** \brief
    Construct with a range.

    \param directions (optional; any number of arguments is allowed)
        The directions that should be passed to \ref range::view to view the
        range.
        If left out, the default direction is used.
    \param range
        The actual range to view with this any_range.
    */
    template <class ... Arguments,
        class Range = typename
            meta::first <meta::back, meta::vector <Arguments ...>>::type,
        class Enable1 = typename boost::enable_if <is_range <Range>>::type,
        class Enable2 = typename boost::disable_if <
            is_any_range_with_same_element_type <Range>>::type>
    explicit any_range (Arguments && ... arguments)
    : implementation_ (wrap_view (range::view (
        std::forward <Arguments> (arguments) ...))) {}

    /** \brief
    Copy constructor.

    This is only available if the underlying range allows copying, that is, if
    \c copy_construct is in the list of capabilities.
    */
    any_range (any_range_if_copy_constructible const & other)
    : implementation_ (other.implementation_->copy()) {}

    /** \brief
    Move constructor.
    */
    any_range (any_range && other)
    : implementation_ (std::move (other.implementation_)) {}

    /** \brief
    Constructor that takes an any_range with the same Element type, but
    different capabilities.

    This is only available if the underlying range allows copying, that is, if
    \c copy_construct is in the list of capabilities, and if the capabilities
    of the target any_range are a subset of the capabilities of the source
    any_range.

    \todo Make move-construction from a range with other capabilities work.
    This is useful if the underlying range can't be copied.
    This would require "lose_direction_move" to the interface.
    */
    template <class OtherCapabilities, class CapabilityKeys = capability_keys,
        class Enable1 = typename utility::disable_if_same_or_derived <
            any_range <Element, OtherCapabilities>, any_range>::type,
        class Enable2 = typename boost::enable_if <any_range_is_convertible <
            OtherCapabilities>>::type,
        class Enable3 = typename boost::enable_if <meta::has_key <
            capability::copy_construct, CapabilityKeys>>::type>
    any_range (any_range <Element, OtherCapabilities> const & other)
    : implementation_ (convert_from <OtherCapabilities>() (other)) {}

    any_range & operator = (any_range const & other) {
        this->implementation_ = other.implementation_->copy();
        return *this;
    }

    any_range & operator = (any_range && other) {
        this->implementation_ = std::move (other.implementation_);
        return *this;
    }

private:
    // Implementation of operations.

    friend class operation::member_access;

    default_direction_type default_direction() const
    { return implementation_->default_direction(); }

    template <class Capability, class Direction> struct is_implemented_helper
    : meta::contains <Capability,
        typename meta::at <Direction, capabilities>::type> {};

    template <class Capability, class Direction> struct is_implemented
    : boost::mpl::and_ <meta::has_key <Direction, capabilities>,
        is_implemented_helper <Capability, Direction>> {};

    template <class Direction, class Enable = typename boost::enable_if <
        is_implemented <capability::empty, Direction>>::type>
    bool empty (Direction const & direction) const
    { return implementation_->empty (direction); }

    template <class Direction, class Enable = typename boost::enable_if <
        is_implemented <capability::size, Direction>>::type>
    std::size_t size (Direction const & direction) const
    { return implementation_->size (direction); }

    template <class Direction, class Enable = typename boost::enable_if <
        is_implemented <capability::first, Direction>>::type>
    Element first (Direction const & direction) const
    { return implementation_->first (direction); }

    template <class Direction, class Enable = typename boost::enable_if <
        is_implemented <capability::drop_one, Direction>>::type>
    any_range drop_one (Direction const & direction) const
    { return any_range (implementation_->drop_one (direction)); }

    template <class Direction, class Enable = typename boost::enable_if <
        is_implemented <capability::drop_n, Direction>>::type>
    any_range drop (Direction const & direction, size_t increment) const
    { return any_range (implementation_->drop_n (direction, increment)); }

    template <class Direction, class Enable = typename boost::enable_if <
        is_implemented <capability::chop_destructive, Direction>>::type>
    Element chop_in_place (Direction const & direction)
    {
        auto chopped = implementation_->chop_destructive (
            direction, implementation_);
        return chopped.move_first();
    }
};

namespace operation {

    // chop cannot be implemented in the class for older compilers, because
    // member functions cannot be qualified with &&.
    template <class Capabilities, class Direction, class AnyRange>
        struct chop <any_range_tag <Capabilities>, Direction, AnyRange &&>
    : chop_by_chop_in_place <
        any_range_tag <Capabilities>, Direction, AnyRange &&> {};

} // namespace operation

namespace callable {

    class make_any_range {
        /// Compute the capabilities, using the directions, or not, if they are
        /// not given.
        template <class Directions, class View> struct compute_capabilities;

        template <class ... Directions, class View>
            struct compute_capabilities <meta::set <Directions ...>, View>
        : capability::detect_capabilities <View,
            typename capability::detect_copy_construct_key <View,
                meta::set <Directions ...>>::type> {};

        template <class View> struct compute_capabilities <meta::set<>, View>
        : capability::detect_capabilities <View> {};

        /**
        The actual implementation, having split up the argument types into
        Directions (everything except for the last argument) and Range (the last
        argument).
        */
        template <class Directions, class Range, class Enable = void>
            struct implementation : operation::unimplemented {};

        template <class ... Directions, class Range>
            class implementation <meta::vector <Directions ...>, Range,
                typename boost::enable_if <meta::all_of_c <
                    is_direction <Directions>::value ...,
                    is_range <Range>::value>>::type>
        {
            typedef typename range::decayed_result_of <callable::view (
                Directions ..., Range)>::type view_type;
            typedef typename compute_capabilities <
                meta::set <typename std::decay <Directions>::type ...>,
                view_type>::type capabilities;
            typedef typename range::result_of <callable::first (view_type &&)
                >::type element_type;
        public:
            typedef any_range <element_type, capabilities> result_type;

            // Clang 3.0 does not like being passed Directions ..., Range.
            template <class ... Arguments>
                result_type operator() (Arguments && ... arguments) const
            { return result_type (std::forward <Arguments> (arguments) ...); }
        };

    public:
        template <class ...> struct apply;

        template <class ... Arguments> struct apply
        : implementation <
            typename meta::drop <meta::back,
                meta::vector <Arguments ...>>::type,
            typename meta::first <meta::back,
                meta::vector <Arguments ...>>::type> {};

        template <class ... Arguments>
            auto operator() (Arguments && ... arguments) const
        RETURNS (apply <Arguments ...>() (
            std::forward <Arguments> (arguments) ...));
    };

} // namespace callable

/** \brief
Create an any_range object from a range, with best-guess type parameters.

If directions are given, they specify the directions that the range should be
traversed in.
Otherwise, the default direction will be used, and its reverse, if applicable.

The any_range always has the correct type for default_direction.

The element type is set to the result type of <c>first (range)</c>, in the
default direction.
make_any_range can only be used if this is implemented.

\param directions (optional; any number of arguments is allowed)
    The directions that should be passed to \ref range::view to view the range.
    If left out, the default direction is used to call view, and the any_range
    will allow traversal in that direction and its reverse, if applicable.
\param range
    The actual range the any_range will give a view on.
*/
static auto constexpr make_any_range = callable::make_any_range();

} // namespace range

#endif // RANGE_ANY_RANGE_HPP_INCLUDED
