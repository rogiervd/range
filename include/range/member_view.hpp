/*
Copyright 2011, 2012, 2013 Rogier van Dalen.

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

#ifndef RANGE_MEMBER_VIEW_HPP_INCLUDED
#define RANGE_MEMBER_VIEW_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

#include "core.hpp"

#include "meta/vector.hpp"

#include "utility/returns.hpp"

namespace range {

/**
Extract something from a struct or a class.
The operator() takes a reference to a structure and returns this something.
There are three types of ways that this "something" can be extracted.

1. The value of a member variable.
For a struct "structure" with a member "int i", this looks like
    member_extractor <int structure::*, &structure::i>.
This works with any type that is implicitly convertible to structure &,
structure const &, or structure &&.
This will return the same type of reference to the member variable.

2. The return value of a nullary member function.
For a member function
    int get_int(),
this looks like
    member_extractor <int (structure::*)() [const], &structure::get_int>.
The member function must be correctly const-qualified.
The return value is the exact return value of the member function.

3. The return value of a unary free function that takes a reference to the
structure.
For a function
    int get_int (structure const &),
this looks like
    member_extractor <int (*) (structure const &), &get_int>.
The return value is the exact return value of the function.

Preferably, of course, this would have worked somehow without specifying the
type.
However, I do not think this is possible for compile-time values.
*/
template <typename Member, Member> struct member_extractor;

// Specialisation for a member variable.
template <typename Structure, typename Type, Type Structure::* member>
    struct member_extractor <Type Structure::*, member>
{
    Type & operator() (Structure & structure) const
    { return structure.*member; }

    Type const & operator() (Structure const & structure) const
    { return structure.*member; }

    Type && operator() (Structure && structure) const
    { return static_cast <Type &&> (structure.*member); }
};

// Specialisation for a member function.
template <typename Structure, typename ReturnType,
    ReturnType (Structure::*member_function) ()>
struct member_extractor <ReturnType (Structure::*)(), member_function> {
    ReturnType operator() (Structure & structure) const
    { return (structure.*member_function)(); }
};

template <typename Structure, typename ReturnType,
    ReturnType (Structure::*member_function) () const>
struct member_extractor <ReturnType (Structure::*)() const, member_function> {
    ReturnType operator() (Structure const & structure) const
    { return (structure.*member_function)(); }
};

// Specialisation for a free function.
template <typename Structure, typename ReturnType,
    ReturnType (* function) (Structure)>
struct member_extractor <ReturnType (*) (Structure), function> {
    typedef ReturnType value_type;

    ReturnType operator() (Structure structure) const
    { return (function) (std::forward <Structure> (structure)); }
};

namespace detail {
    /**
    Base class for member_view.
    Different views of the same member have the same instantiation of this
    class as a base.
    This makes conversion (such as in "drop") easy.
    */
    template <class Structure> class member_view_base {
        static_assert (std::is_reference <Structure>::value,
            "Structure must be an lvalue or rvalue reference.");
    public:
        member_view_base (Structure structure_)
        : structure_ (&structure_) {}

        Structure structure() const
        { return static_cast <Structure> (*structure_); }

    private:
        typename std::add_pointer <Structure>::type structure_;
    };
} // namespace detail

/**
Lightweight range that traverses a struct or class.
The range can be used from either "front" or "back" direction.

The Extractors is a meta-list of member_extractor<> (or similar) classes,
which each extract a specific member from a structure.

\a Structure must be a reference; it can be a lvalue reference, a const
reference, or an rvalue reference.
Extractors are passed the structure qualified in exactly that way, and are
free to do with this with they please.
*/
template <class Structure, class Extractors> class member_view
: public detail::member_view_base <Structure> {
    typedef detail::member_view_base <Structure> base;
public:
    explicit member_view (Structure structure)
    : base (static_cast <Structure> (structure)) {}

    /**
    Generalised copy constructor.
    It is not checked whether Extractors2 is a superset of Extractors.
    */
    template <class Extractors2>
        member_view (member_view <Structure, Extractors2> const & that)
    : base (that) {}

    typedef Structure structure_type;
    typedef Extractors extractors_type;
};

template <std::size_t Size> struct member_view_tag;

template <class Structure, class Extractors>
    struct tag_of_qualified <member_view <Structure, Extractors> >
{
    typedef member_view_tag <meta::size <Extractors>::type::value> type;
};

namespace operation {

    // empty
    template <std::size_t size>
        struct empty <member_view_tag <size>, direction::front>
    : helper::return_default_constructed <rime::false_type> {};

    template <> struct empty <member_view_tag <0u>, direction::front>
    : helper::return_default_constructed <rime::true_type> {};

    // size
    template <std::size_t Size>
        struct size <member_view_tag <Size>, direction::front>
    : helper::return_default_constructed <rime::size_t <Size>> {};

    // first
    template <std::size_t Size, class Direction>
        struct first <member_view_tag <Size>, Direction,
            typename boost::enable_if <boost::mpl::or_ <
                std::is_same <Direction, direction::front>,
                std::is_same <Direction, direction::back>
        >>::type>
    {
        template <class Range> struct extractor
        : meta::first <Direction, typename Range::extractors_type> {};

        template <class Range> auto operator() (Direction const &,
            Range const & range) const
        RETURNS (typename extractor <Range>::type() (range.structure()));
    };

    template <> struct first <member_view_tag <0u>, direction::front>
    : unimplemented {};
    template <> struct first <member_view_tag <0u>, direction::back>
    : unimplemented {};

    // drop_constant (instantiated only if Increment is constant).
    template <std::size_t size, class Direction, class Increment>
        struct drop_constant <member_view_tag <size>, Direction, Increment,
            // Only implement if Increment <= size.
            typename std::enable_if <(std::size_t (Increment::value) <= size)
        >::type>
    {
        template <class Range> struct result {
            typedef typename std::decay <Range>::type range;
            typedef typename meta::drop <Direction, Increment,
                typename range::extractors_type>::type new_extractors;
            typedef member_view <typename range::structure_type, new_extractors>
                type;
        };

        template <class Range> auto
            operator() (Direction const &, Increment const &,
                Range const & range) const
        RETURNS (typename result <Range>::type (range));
    };

} // namespace operation

} // namespace range

#endif  // RANGE_MEMBER_VIEW_HPP_INCLUDED
