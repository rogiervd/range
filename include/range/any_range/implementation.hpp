/*
Copyright 2013, 2015 Rogier van Dalen.

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
Implement the polymorphic interface for actual ranges underlying any_range.

This file follows the same structure as interface.hpp.
Some of the class names are even the same, but then in a different namespace.
Before reading this file, it is probably easiest to read and understand
interface.hpp.

The same idea of linear inheritance is used in this file, but here each
combination of direction and operation gets its own class, and the
implementation linearly inherits from all of them.
*/

#ifndef RANGE_ANY_RANGE_IMPLEMENTATION_HPP_INCLUDED
#define RANGE_ANY_RANGE_IMPLEMENTATION_HPP_INCLUDED

#include <utility>
#include <stdexcept>

#include <boost/mpl/placeholders.hpp>

#include "utility/unique_ptr.hpp"

#include "meta/range.hpp"
#include "meta/fold.hpp"

#include "range/core.hpp"
#include "capability.hpp"
#include "interface.hpp"

namespace range { namespace any_range_implementation {

    /** \brief
    Implement the interface in ./interface.hpp for \a Capabilities, for all
    operations that \a Underlying supports.

    Operations that are not supported by \a Underlying lead to undefined
    behaviour when called.
    */
    template <class Element, class CapabilityKeys, class Underlying>
        class implementation;

    template <class Element, class CapabilityKeys, class Underlying>
        class base
    : public any_range_interface::interface <Element, CapabilityKeys,
        typename capability::detect_default_direction <Underlying>::type>
    {
        static_assert (capability::is_capability_keys <CapabilityKeys>::value,
            "");

        Underlying underlying_;

    public:
        typedef Underlying underlying_type;
        typedef implementation <Element, CapabilityKeys, Underlying>
            implementation_type;

        template <class OtherUnderlying> struct implementation_for {
            typedef implementation <Element, CapabilityKeys, OtherUnderlying>
                type;
        };

        template <class Argument> base (Argument && argument)
        : underlying_ (std::forward <Argument> (argument)) {}

        base (base const & other) : underlying_ (other.underlying_) {}
        base (base && other) : underlying_ (std::move (other.underlying_)) {}

        Underlying & underlying() { return underlying_; }
        Underlying const & underlying() const { return underlying_; }
    };

    /** Implement the losing of a capability. */
    template <class Element, class CapabilityKeys, class KeyToRemove,
            class Base, class Enable = void>
        struct implement_lose_capability;

    // Without copy construction: can not be implemented.
    template <class Element, class CapabilityKeys, class KeyToRemove,
            class Base>
        struct implement_lose_capability <Element, CapabilityKeys, KeyToRemove,
            Base, typename boost::disable_if <
                meta::has_key <capability::copy_construct, CapabilityKeys>
            >::type>
    : Base
    {
        static_assert (capability::is_capability_keys <CapabilityKeys>::value,
            "");

        template <class Argument> implement_lose_capability (
            Argument && argument)
        : Base (std::forward <Argument> (argument)) {}
    };

    // With copy construction: can be implemented.
    template <class Element, class CapabilityKeys, class KeyToRemove,
            class Base>
        struct implement_lose_capability <Element, CapabilityKeys, KeyToRemove,
            Base, typename boost::enable_if <
                meta::has_key <capability::copy_construct, CapabilityKeys>
            >::type>
    : Base
    {
        static_assert (capability::is_capability_keys <CapabilityKeys>::value,
            "");

        template <class Argument> implement_lose_capability (
            Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::lose_capability;

        virtual typename Base::template
            interface_ptr_without <KeyToRemove>::type
                lose_capability (capability::type <KeyToRemove>) const
        {
            typedef typename meta::remove <KeyToRemove, CapabilityKeys>::type
                new_capability_keys;
            typedef implementation <Element, new_capability_keys,
                typename Base::underlying_type> new_implementation_type;

            return utility::make_unique <new_implementation_type> (
                this->underlying());
        }
    };

    /* Implement capabilities. */
    template <class Element, class Capability, class Base, class Enable = void>
        struct implement;

    template <class Element, class Base>
        struct implement <Element,
            meta::map_element <capability::copy_construct, void>,
            Base>
    : Base
    {
        template <class Argument> implement (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        virtual typename Base::interface_ptr copy() const {
            return utility::make_unique <typename Base::implementation_type> (
                this->underlying());
        }
    };

    // default_direction
    template <class Element, class Direction, class Base>
        struct implement <Element,
            meta::map_element <capability::default_direction, Direction>,
            Base>
    : Base
    {
        template <class Argument> implement (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        // Sometimes this overrides something, but if the default direction
        // is default-constructible, the method is not virtual in the interface.
        virtual Direction default_direction() const
        { throw std::logic_error ("Bug in any_range."); }
    };

    template <class Element, class Capability, class Direction, class Base>
        struct implement_capability;

    // Farm out to implement_capability for each capability.
    template <class Element, class Direction, class Capabilities, class Base>
        struct implement <Element,
            meta::map_element <Direction, Capabilities>, Base>
    : meta::fold <implement_capability <
            Element, boost::mpl::_2, Direction, boost::mpl::_1>,
        Base, Capabilities>::type
    {
        typedef typename meta::fold <implement_capability <
                Element, boost::mpl::_2, Direction, boost::mpl::_1>,
            Base, Capabilities>::type base_type;

        template <class Argument> implement (Argument && argument)
        : base_type (std::forward <Argument> (argument)) {}
    };

    // empty.
    template <class Element, class Direction, class Base>
        struct implement_capability <Element,
            capability::empty, Direction, Base>
    : Base
    {
        template <class Argument> implement_capability (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::empty;

        virtual bool empty (Direction const & direction) const
        { return range::empty (direction, this->underlying()); }
    };

    // size.
    template <class Element, class Direction, class Base>
        struct implement_capability <Element,
            capability::size, Direction, Base>
    : Base
    {
        template <class Argument> implement_capability (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::size;

        virtual std::size_t size (Direction const & direction) const
        { return range::size (direction, this->underlying()); }
    };

    // first.
    template <class Element, class Direction, class Base>
        struct implement_capability <Element,
            capability::first, Direction, Base>
    : Base
    {
        template <class Argument> implement_capability (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::first;

    private:
        Element implementation (
            Direction const & direction, rime::false_type) const
        { return range::first (direction, this->underlying()); }

        Element implementation (Direction const &, rime::true_type) const
        { throw std::logic_error ("Bug in any_range."); }

    public:
        virtual Element first (Direction const & direction) const {
            return implementation (direction,
                always_empty <Direction, typename Base::underlying_type>());
        }
    };

    // drop_one.
    template <class Element, class Direction, class Base>
        struct implement_capability <Element,
            capability::drop_one, Direction, Base>
    : Base
    {
        template <class Argument> implement_capability (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::drop_one;

        typedef typename Base::interface_ptr interface_ptr;

    private:
        interface_ptr implementation (
            Direction const & direction, rime::false_type) const
        {
            typedef typename Base::underlying_type underlying_type;
            // If the range is heterogeneous, the new underlying type will be
            // different.
            // It is assumed it has the same capabilities.
            typedef typename result_of <callable::drop (
                Direction, underlying_type)>::type new_underlying_type;

            typedef typename Base::template implementation_for <
                new_underlying_type>::type new_implementation_type;
            return utility::make_unique <new_implementation_type> (
                range::drop (direction, this->underlying()));
        }

        interface_ptr implementation (Direction const &, rime::true_type) const
        { throw std::logic_error ("Bug in any_range."); }

    public:
        virtual interface_ptr drop_one (
            Direction const & direction) const
        {
            return implementation (direction,
                always_empty <Direction, typename Base::underlying_type>());
        }
    };

    // drop_n.
    template <class Element, class Direction, class Base>
        struct implement_capability <Element,
            capability::drop_n, Direction, Base>
    : Base
    {
        template <class Argument> implement_capability (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::drop_n;

        typedef typename Base::interface_ptr interface_ptr;

    private:
        interface_ptr implementation (Direction const & direction,
            std::size_t increment, rime::false_type) const
        {
            typedef typename Base::underlying_type underlying_type;
            typedef typename result_of <callable::drop (
                    Direction, std::size_t, underlying_type)>::type
                new_underlying_type;
            typedef typename Base::template implementation_for <
                new_underlying_type>::type new_implementation_type;
            return utility::make_unique <new_implementation_type> (
                range::drop (direction, increment, this->underlying()));
        }

        interface_ptr implementation (
            Direction const &, std::size_t, rime::true_type) const
        { throw std::logic_error ("Bug in any_range."); }

    public:
        virtual interface_ptr drop_n (
            Direction const & direction, std::size_t increment) const
        {
            return implementation (direction, increment,
                always_empty <Direction, typename Base::underlying_type>());
        }
    };

    template <class Element, class Direction, class Base>
        struct implement_capability <Element,
            capability::chop_destructive, Direction, Base>
    : Base
    {
        template <class Argument> implement_capability (Argument && argument)
        : Base (std::forward <Argument> (argument)) {}

        using Base::chop_destructive;

        typedef typename Base::interface_ptr interface_ptr;

    public:
        // Don't hide this type from subclasses.
        typedef typename Base::underlying_type underlying_type;

    private:
        typedef any_range_interface::chopped <Element, interface_ptr>
            chop_destructive_result;

        // Dispatch based on: has <chop_in_place>, always_empty.
        chop_destructive_result implementation (
            Direction const & direction, interface_ptr & this_,
            rime::true_type, rime::false_type)
        {
            return chop_destructive_result (
                range::chop_in_place (direction, this->underlying()),
                interface_ptr());
        }

        chop_destructive_result implementation (
            Direction const & direction, interface_ptr & this_,
            rime::false_type, rime::false_type)
        {
            auto c = range::chop (direction, std::move (this->underlying()));
            typedef typename std::decay <decltype (c.forward_rest())>::type
                new_underlying_type;
            typedef typename Base::template implementation_for <
                new_underlying_type>::type new_implementation_type;
            interface_ptr discardable = std::move (this_);
            this_ =
                utility::make_unique <new_implementation_type> (c.move_rest());
            return chop_destructive_result (
                c.move_first(), std::move (discardable));
        }

        template <class Bool>
            chop_destructive_result implementation (
                Direction const &, interface_ptr &, Bool, rime::true_type)
        { throw std::logic_error ("Bug in any_range."); }

    public:
        virtual chop_destructive_result chop_destructive (
            Direction const & direction, interface_ptr & this_)
        {
            return implementation (direction, this_,
                has <callable::chop_in_place (Direction, underlying_type &)>(),
                always_empty <Direction, typename Base::underlying_type>());
        }
    };

    // Types for implementation class.

    template <class Element, class CapabilityKeys, class Underlying>
        struct implementation_types
    {
        typedef typename capability::detect_capabilities <
            Underlying, CapabilityKeys>::type capabilities;

        typedef typename meta::fold <
                implement_lose_capability <Element, CapabilityKeys,
                    boost::mpl::_2, boost::mpl::_1>,
                base <Element, CapabilityKeys, Underlying>,
                CapabilityKeys>::type lose_capabilities_type;

        typedef typename meta::fold <
            implement <Element, boost::mpl::_2, boost::mpl::_1>,
            lose_capabilities_type,
            capabilities>::type base_type;
    };

    /* Main implementation class. */

    template <class Element, class CapabilityKeys, class Underlying>
        class implementation
    : public implementation_types <
        Element, CapabilityKeys, Underlying>::base_type
    {
        typedef implementation_types <Element, CapabilityKeys, Underlying>
            types;
    public:
        typedef typename types::capabilities capabilities;
    private:
        typedef typename types::base_type base_type;

    public:
        template <class Argument> implementation (Argument && argument)
        : base_type (std::forward <Argument> (argument)) {}
    };

}} // namespace range::any_range_implementation

#endif // RANGE_ANY_RANGE_IMPLEMENTATION_HPP_INCLUDED
