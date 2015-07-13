/*
Copyright 2011-2013, 2015 Rogier van Dalen.

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

#ifndef RANGE_MEMBER_VIEW_HPP_INCLUDED
#define RANGE_MEMBER_VIEW_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

#include "core.hpp"

#include "meta/vector.hpp"

#include "utility/returns.hpp"

#include "detail/meta_is_linear_subset.hpp"

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
    static std::size_t constexpr extractor_num = meta::size <Extractors>::value;

public:
    explicit member_view (Structure structure)
    : base (static_cast <Structure> (structure)) {}

    /**
    Generalised copy constructor.
    Extractors2 must be a superset of Extractors.
    */
    template <class Extractors2>
        member_view (member_view <Structure, Extractors2> const & that,
            typename boost::enable_if <
                detail::is_linear_subset <Extractors, Extractors2>>::type * = 0)
    : base (that) {}

    typedef Structure structure_type;
    typedef Extractors extractors_type;

private:
    friend class helper::member_access;

    auto empty (direction::front) const
    RETURNS (rime::bool_<extractor_num == 0>());

    auto size (direction::front) const
    RETURNS (rime::size_t <extractor_num>());

    // first.
    template <class Direction> struct first_extractor
    : meta::first <Direction, Extractors> {};

    template <class Direction> struct first_type
    : result_of <typename first_extractor <Direction>::type (Structure)> {};

    template <class Direction>
        typename boost::lazy_enable_if_c <
            (extractor_num != 0), first_type <Direction>>::type
        first (Direction) const
    { return typename first_extractor <Direction>::type() (this->structure()); }

    // drop.
    template <class Increment, class Direction> struct drop_result {
        typedef typename meta::drop <Direction, Increment, Extractors>::type
            new_extractors;
        typedef member_view <Structure, new_extractors> type;
    };

    template <class Direction, class Increment>
        typename boost::lazy_enable_if_c <
            (std::size_t (Increment::value) <= extractor_num),
            drop_result <Increment, Direction>>::type
    drop_constant (Increment const &, Direction const &) const
    { return typename drop_result <Increment, Direction>::type (*this); }
};


namespace member_view_operation {
    struct member_view_tag {};
} // namespace member_view_operation

template <class Structure, class Extractors>
    struct tag_of_qualified <member_view <Structure, Extractors>>
{ typedef member_view_operation::member_view_tag type; };

} // namespace range

#endif  // RANGE_MEMBER_VIEW_HPP_INCLUDED
