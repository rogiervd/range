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

/** \file
Provide a polymorphic interface for actual ranges underlying any_range.
This requires defining virtual methods for each direction, and for copy
construction.
This works as follows.

range::any_range_interface::interface has a type parameter CapabilityKeys, which
is a set of directions (and possibly "default_construct") to provide virtual
methods for.
This class inherits linearly from classes that provide these virtual methods, by
passing as a type parameter to those classes the class they should derive from.

The interface has virtual methods for all operations that could be implemented
for a direction.
It is up to any_range to make sure only operations that are implemented are
called.

The implementation of this interface is in implementation.hpp, and this follows
the same structure.
*/

#ifndef RANGE_ANY_RANGE_INTERFACE_HPP_INCLUDED
#define RANGE_ANY_RANGE_INTERFACE_HPP_INCLUDED

#include <type_traits>
#include <memory>
#include <stdexcept>

#include <boost/mpl/placeholders.hpp>
#include <boost/utility/enable_if.hpp>

#include "meta/range.hpp"
#include "meta/fold.hpp"
#include "meta/vector.hpp"

#include "capability.hpp"

namespace range { namespace any_range_interface {

    /** \brief
    Base class that provides a type-erased interface to an underlying range.

    This contains unimplemented virtual methods for each direction, to be
    implemented by the implementation in implementation.hpp.
    This class linearly inherits from \ref base, and \ref implement for each
    direction.

    Because it must be possible to remove capabilities from an interface, the
    number of template instantiations is quadratic in the number of
    capabilities.
    */
    template <class Element, class CapabilityKeys, class DefaultDirection>
        struct interface;

    /** \brief
    Provide the first element of a range, and a pointer to the type-erased
    interface with the rest of the range.
    */
    template <class Element, class InterfacePtr> struct chopped {
        Element first_;
        // InterfacePtr new_interface_;
        InterfacePtr discardable_;

        // OriginalElement must be convertible to Element.
        template <class OriginalElement>
        chopped (OriginalElement && first, InterfacePtr && discardable)
        : first_ (std::forward <OriginalElement> (first)),
            discardable_ (std::move (discardable)) {}

        // This needs to be explicit; first_ may throw, but discardable_ cannot
        // be copied.
        chopped (chopped && that)
        : first_ (std::forward <Element> (that.first_)),
            discardable_ (std::move (that.discardable_)) {}

        Element && move_first()
        { return std::forward <Element> (first_); }
    };

    /** \brief
    Base class for the interface to access type-erased ranges.

    \internal
    One thing this contains is functions with the names of virtual functions
    in subclasses.
    This is so that subclasses can say <c>using Base::empty;</c> whether or not
    \c Base has a meaningful \c empty or a not-so-meaningful one.
    */
    template <class Element, class CapabilityKeys, class DefaultDirection>
        struct base
    {
        static_assert (capability::is_capability_keys <CapabilityKeys>::value,
            "");

        void lose_capability();

        void empty();
        void size();
        void first();
        void drop_one();
        void drop_n();
        void chop_destructive();

        typedef interface <Element, CapabilityKeys, DefaultDirection>
            interface_type;
        typedef std::unique_ptr <interface_type> interface_ptr;

        template <class CapabilityKey> struct interface_type_without {
            typedef interface <Element,
                typename meta::remove <CapabilityKey, CapabilityKeys>::type,
                DefaultDirection> type;
        };

        template <class CapabilityKey> struct interface_ptr_without {
            typedef std::unique_ptr <
                typename interface_type_without <CapabilityKey>::type> type;
        };

        virtual ~base() {}
    };

    template <class Element, class CapabilityKey, class Base,
            class Enable = void>
        struct implement;

    /**
    Implement default_direction.

    If the direction contains meaningful information, the function is virtual
    and must be overridden.
    If not, the direction can be default-constructed.
    (The implementation may try to override the function, but since it is not
    virtual, that does not work.)
    */
    template <class Direction, class Base, class Enable = void>
        struct implement_default_direction;

    // Direction cannot be default-constructed.
    template <class Direction, class Base>
        struct implement_default_direction <Direction, Base,
            typename boost::disable_if <
                capability::default_construct_direction <Direction>>::type>
    : Base
    { virtual Direction default_direction() const = 0; };

    // Direction can be default-constructed.
    // (And the method does not need to be virtual.)
    template <class Direction, class Base>
        struct implement_default_direction <Direction, Base,
            typename boost::enable_if <
                capability::default_construct_direction <Direction>>::type>
    : Base
    { Direction default_direction() const { return Direction(); } };

    // Provide interface for copying a range.
    template <class Element, class Base>
        struct implement <Element, capability::copy_construct, Base>
    : Base
    {
        virtual typename Base::template
            interface_ptr_without <capability::copy_construct>::type
                lose_capability (capability::type <capability::copy_construct>)
                const
        { throw std::logic_error ("Bug in any_range."); }

        virtual typename Base::interface_ptr copy() const = 0;
    };

    /*
    Implement capabilities for one direction.
    Implement all capabilities, because that reduces the number of
    instantiations.
    */
    template <class Element, class Direction, class Base>
        struct implement <Element, Direction, Base>
    : Base
    {
        using Base::lose_capability;

        using Base::empty;
        using Base::size;
        using Base::first;
        using Base::drop_one;
        using Base::drop_n;
        using Base::chop_destructive;

        /** \brief
        Return a pointer to a newly constructed object that provides the same
        capabilities, except for \a Direction.
        */
        virtual typename Base::template interface_ptr_without <Direction>::type
            lose_capability (capability::type <Direction>) const
        { throw std::logic_error ("Bug in any_range."); }

        /** \brief
        Return the result of applying \c empty to the underlying range.
        */
        virtual bool empty (Direction const &) const
        { throw std::logic_error ("Bug in any_range."); }

        /** \brief
        Return the result of applying \c size to the underlying range.
        */
        virtual std::size_t size (Direction const &) const
        { throw std::logic_error ("Bug in any_range."); }

        /** \brief
        Return the result of applying \c first to the underlying range.
        */
        virtual Element first (Direction const &) const
        { throw std::logic_error ("Bug in any_range."); }

        typedef typename Base::interface_ptr interface_ptr;

        /** \brief
        Return a pointer to a newly constructed object that represents the
        result of calling \c drop on the underlying range.
        */
        virtual interface_ptr drop_one (Direction const &) const
        { throw std::logic_error ("Bug in any_range."); }

        /** \brief
        Return a pointer to a newly constructed object that represents the
        result of calling \c drop with an increment on the underlying range.
        */
        virtual interface_ptr drop_n (Direction const &, std::size_t) const
        { throw std::logic_error ("Bug in any_range."); }

        /**
        Return the first element of the range, and make this object start at the
        next element.
        If it is impossible to change this object in-place, then the pointer
        \a this_ should be changed to point at a newly-constructed object.
        In that case, the second element of the return type can be used to pass
        a pointer that owns this object back to the caller, so this is
        destructed only after the call has finished.
        */
        virtual chopped <Element, interface_ptr>
            chop_destructive (Direction const &, interface_ptr & this_)
        { throw std::logic_error ("Bug in any_range."); }
    };

    template <class Element, class CapabilityKeys, class DefaultDirection>
        struct interface
    // Linearly inherit from the appropriate base types.
    : meta::fold <
        implement <Element, boost::mpl::_2, boost::mpl::_1>,
        implement_default_direction <DefaultDirection,
            base <Element, CapabilityKeys, DefaultDirection>>,
        CapabilityKeys>::type
    {};

    /**
    Function object that converts an interface.
    */
    template <class TargetInterfacePtr, class TargetCapabilityKeysLeft,
        class CurrentCapabilityKeysLeft>
    struct convert_interface;

    // This direction can remain.
    template <class TargetInterfacePtr, class First, class ... RestOne,
            class ... RestTwo>
        struct convert_interface <TargetInterfacePtr,
            meta::set <First, RestOne ...>,
            meta::set <First, RestTwo ...>>
    : convert_interface <TargetInterfacePtr,
        meta::set <RestOne ...>, meta::set <RestTwo ...>> {};

    // Remove this direction.
    template <class TargetInterfacePtr, class ... TargetCapabilityKeysLeft,
            class First, class ... Rest>
        struct convert_interface <TargetInterfacePtr,
            meta::set <TargetCapabilityKeysLeft ...>,
            meta::set <First, Rest ...>>
     {
        convert_interface <TargetInterfacePtr,
            meta::set <TargetCapabilityKeysLeft ...>,
            meta::set <Rest ...>> recursive;

        template <class OtherInterfacePtr>
            TargetInterfacePtr operator() (OtherInterfacePtr const & input)
            const
        {
            return recursive (
                input->lose_capability (capability::type <First>()));
        }
    };

    // Finished.
    template <class TargetInterfacePtr>
        struct convert_interface <TargetInterfacePtr, meta::set<>, meta::set<>>
    {
        TargetInterfacePtr operator() (TargetInterfacePtr && interface) const
        { return std::move (interface); }

        TargetInterfacePtr operator() (TargetInterfacePtr const & interface)
            const
        { return interface->copy(); }
    };

}} // namespace range::any_range_interface

#endif // RANGE_ANY_RANGE_INTERFACE_HPP_INCLUDED
