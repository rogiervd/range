/*
Copyright 2013, 2014 Rogier van Dalen.

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
Define a general heterogeneous container.
*/

#ifndef RANGE_CONTAINER_TUPLE_HPP_INCLUDED
#define RANGE_CONTAINER_TUPLE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include "utility/storage.hpp"
#include "utility/is_assignable.hpp"
#include "utility/type_sequence_traits.hpp"
#include "utility/disable_if_same.hpp"

#include "rime/core.hpp"
#include "rime/assert.hpp"
#include "rime/always.hpp"

#include "core.hpp"
#include "heavyweight.hpp"
#include "element_types.hpp"

#include "detail/enable_if_front_back.hpp"

namespace range {

/**
Heterogeneous fixed-length container.
That is, the types contained can all be different, and the list of types is
given explicitly as template parameters.

The container can be constructed in three ways.
One is default construction.
The next is construction from a value for each contained type.
The third is construction from another range.
If the length of this range is known at compile time to be of the same length
and all elements can be implicitly converted, this constructor is implicit.
If the length of this range is not know at compile time and/or not all elements
can be implicitly converted (though the types must be constructible), then the
constructor is explicit.

The implementation aims to instantiate as few template classes as possible.

Unlike for \c std::tuple, there is no \c get function; its functionality is
provided by \c range::at and \c range::at_c.

The layout of the elements in memory is undefined.
Connected with this is the layout

\todo Implement tuple_cat? Probably rather make_tuple_from (concatenate (...)).
\todo uses_allocator
\todo Comparison between tuples.
\todo Using delegate constructors, it is probably possible to allow
initialisation from input ranges, by recursively delegating to constructors with
a longer and longer list of arguments (all temporaries) until the correct number
have been extracted.

\todo no_except specification.
\todo constexpr specification.
*/
template <class ... Types> class tuple;

template <class ... Types> struct tag_of_qualified <tuple <Types...>>
{ typedef heavyweight_tag <tuple <Types ...>> type; };

namespace tuple_detail {
    template <class Type> struct is_tuple_implementation : rime::false_type {};
    template <class ... Types>
        struct is_tuple_implementation <tuple <Types ...>>
    : rime::true_type {};
} // namespace tuple_detail

/**
Evaluate to \c true iff \a Type is a (possibly qualified) tuple.
*/
template <class Type> struct is_tuple
: tuple_detail::is_tuple_implementation <typename std::decay <Type>::type> {};

/**
Evaluate to the size of the tuple.
*/
template <class Tuple> struct tuple_size
: tuple_size <typename std::decay <Tuple>::type> {};
template <class ... Types> struct tuple_size <tuple <Types ...>>
: rime::size_t <sizeof ... (Types)> {};

namespace tuple_detail {

    /**
    Extract the element at \a Index from the tuple.
    */
    template <std::size_t Index> class extract;

    // Contain all elements of the tuple.
    template <class ... Types> class elements;

    /**
    Dummy structure that indicates an index into a tuple (counting from the end)
    at compile time.
    */
    template <std::size_t Index> struct contain_index {};

    /**
    Dummy structure that indicates the type at an index in a tuple.
    */
    template <class Type> struct contained_type { typedef Type type; };

    /**
    Contain a value and provide access to it.
    This derived from by tuple.
    The second template parameter is \a Index, which makes this type
    identifiable in case there is more than one of the same type in the tuple.

    An important feature of this class is the get_contain and get_contained_type
    methods.
    These are called with as a parameter <c> contain_index \<Index> </c>.
    Because tuple derives from many contain classes, all with different indices,
    and uses these methods, it becomes possible to reach just one "contain".
    "get_contain" returns a reference to the appropriate "contain" class.
    "get_contained_type" is not implemented, just declared as returning
    contained_type <Type>.
    By using <c>decltype (t.get_contained_type (contain_index <..>()))</c>,
    therefore, the contained type can be found.
    This works with whatever complexity the compiler performs overload
    resolution.

    Default-construction default-constructs the element, and should only be
    called if Type is default-constructible.

    Copy- and move-construction work as expected.
    Copy- and move-assignment perform assignment on <c>Type &</c>, not on the
    type as it is stored.

    \todo If Type is empty, then derive from it privately instead of containing
    it.
    */
    template <class Type, std::size_t Index> struct contain {
        typedef Type element_type;
    private:
        typedef typename utility::storage::store <Type>::type stored_type;

        stored_type element_;

        friend class extract <Index>;
        template <class ... Types> friend class elements;

    public:
        contain() : element_() {};

        template <class Argument, class Enable = typename
            utility::disable_if_same_or_derived <contain, Argument>::type>
        explicit contain (Argument && argument)
        : element_ (std::forward <Argument> (argument)) {}

        contain (contain const & that) : element_ (that.element_) {}
        contain (contain && that)
        : element_ (static_cast <stored_type &&> (that.element_)) {}

        template <class Type2 = Type, class Enable = typename boost::enable_if <
            utility::is_assignable <Type2 &, Type2 const &>>::type>
        contain & operator= (contain const & that) {
            static_cast <Type &> (this->element_) =
                static_cast <Type const &> (that.element_);
            return *this;
        }

        template <class Type2 = Type, class Enable = typename boost::enable_if <
            utility::is_assignable <Type2 &, Type2 &&>>::type>
        contain & operator= (contain && that) {
            static_cast <Type &> (this->element_) =
                static_cast <Type &&> (that.element_);
            return *this;
        }

        /**
        Return \c *this, the \c contain class corresponding to \a Index.
        This is useful since elements usually derives from many \c contain
        classes.
        */
        contain & get_contain (contain_index <Index>)
        { return *this; }

        contain const & get_contain (contain_index <Index>) const
        { return *this; }

        /**
        Return the type at \a Index.
        This is not actually defined, merely declared.
        */
        contained_type <Type> get_contained_type (contain_index <Index>) const;
    };

    /**
    Extract member at index \a Index from a tuple, counting from the end.
    */
    template <std::size_t Index> class extract {
    public:
        template <class Tuple> class element {
            static_assert (Index < tuple_size <Tuple>::value,
                "Tuple index out of range.");

            typedef decltype (std::declval <Tuple>().elements()
                .get_contained_type (contain_index <Index>())) contain_type;
        public:
            typedef typename contain_type::type type;
        };

        template <class Tuple> struct result
        : utility::storage::get <typename element <Tuple>::type, Tuple &&> {};

        template <class Tuple>
            typename result <Tuple>::type operator() (Tuple && tuple) const
        {
            result <Tuple> implementation;
            return implementation (tuple.elements()
                .get_contain (contain_index <Index>()).element_);
        }
    };

    /**
    Extractor for member at given \a Index, counted from the start.
    */
    template <std::size_t Index, class Tuple> struct extract_at
    : extract <tuple_size <Tuple>::value - 1 - Index> {};

    /**
    Replacement for std::is_default_constructible which works for reference
    types as well.
    */
    template <class Type, class Dummy = void> struct is_default_constructible
    : std::is_constructible <Type> {};

    template <class Type> struct is_default_constructible <Type &>
    : std::integral_constant <bool, false> {};

    /**
    Return <c>first (direction, range)</c> after testing that the range is not
    empty.
    \throw size_mismatch if the range is empty.
    */
    template <class Direction, class Range> inline
        auto first_if_size_matches (
            Direction const & direction, Range const & range)
    -> decltype (range::first (direction, range))
    {
        if (empty (direction, range))
            throw size_mismatch();
        return range::first (direction, range);
    }

    /**
    Return <c>drop (direction, range)</c> after testing that the range is not
    empty.
    \throw size_mismatch if the range is empty.
    */
    template <class Direction, class Range> inline
        auto drop_if_size_matches (Direction const & direction, Range && range)
    -> decltype (range::drop (direction, std::forward <Range> (range)))
    {
        if (empty (direction, range))
            throw size_mismatch();
        return range::drop (direction, std::forward <Range> (range));
    }

    struct from_elements {};
    struct from_range {};

    /*
    Contain all elements of a tuple.
    This derives from contain <Type, n> for all types Type in the list.

    Thus, elements <int, double, int> derives from:
        contain <int, 2>
        contain <double, 1>
        contain <int, 0>.
    n is an integer that descends.
    Descends? Why?
    Yes; this means that elements <First, Rest ...> can derive from
    elements <Rest ...>.
    This way, tuples with the same tail instantiate the same base type, which
    saves template instantiations in many cases.
    (The peeling off of types could be done the other way around, but it's not
    clear that all compilers will optimise that as well.)
    Of course, the n'th element is still straightforward to find; it will be in
    contains <sizeof...(Types) - n, Type>.

    \todo It might be useful to sort the tuple by alignment, so that it is
    smaller.
    Alignments are rounded up to powers of two, so this could be done with at
    most max. O (N log N) instantiations.
    */
    // Base case.
    template<> class elements<> {
        template <class ... Types2> friend class elements;
        template <class ... Types2> friend class ::range::tuple;

    private:
        // Useless definitions so "using base_type::get_contain" does not yield
        // a compiler error.
        void get_contain();
        void get_contained_type();
        template <std::size_t> friend class tuple_detail::extract;

        // Always default-constructible.
        static constexpr bool default_constructible = true;

        // convertible if \a Range is known to be empty.
        template <class Range> struct range_is_convertible
        : always_empty <direction::front, Range> {};

        // constructible if \a Range can be empty.
        template <class Range> struct range_is_constructible
        : boost::mpl::not_ <never_empty <direction::front, Range>> {};

        static constexpr bool is_copy_assignable = true;
        static constexpr bool is_move_assignable = true;

        // assignable if \a Range can be empty.
        template <class Range> struct range_is_assignable
        : boost::mpl::not_ <never_empty <direction::front, Range>> {};

    public:
        elements() {}

        elements (elements const &) {}
        elements (elements &&) {}

        explicit elements (from_elements) {}

        template <class Range> explicit elements (from_range, Range && range) {
            if (!empty (front, range))
                throw size_mismatch();
        }

        template <class Range> elements & operator= (Range && range) {
            if (!empty (front, range))
                throw size_mismatch();
            return *this;
        }

        elements & operator= (elements const &) noexcept { return *this; }
        elements & operator= (elements &&) noexcept { return *this; }

        void swap (elements & that) const {}
    };

    // Recursive case.
    template <class First, class ... Rest> class elements <First, Rest ...>
    : tuple_detail::contain <First, sizeof... (Rest)>, elements <Rest ...>
    {
    private:
        template <class ... Types2> friend class elements;
        template <class ... Types2> friend class ::range::tuple;
        template <std::size_t> friend class tuple_detail::extract;

        typedef typename utility::storage::store <First>::type
            first_stored_type;
        typedef tuple_detail::contain <First, sizeof... (Rest)> contain_type;
        typedef elements <Rest ...> rest_type;

        using contain_type::get_contain;
        using rest_type::get_contain;
        using contain_type::get_contained_type;
        using rest_type::get_contained_type;

        typename utility::storage::get <First, int &>::type first_element() {
            utility::storage::get <First, int &> convert;
            return convert (static_cast <contain_type *> (this)->element_);
        }

        /**
        Whether all elements are default-constructible.
        */
        static constexpr bool default_constructible =
            is_default_constructible <First>::value
            && rest_type::default_constructible;

        /**
        Whether all elements of \a Range are convertible and the length is known
        to be equal.
        */
        template <class Range, bool NeverEmpty
            = never_empty <direction::front, Range>::value>
        struct range_is_convertible;

        template <class Range> struct range_is_convertible <Range, false>
        : rime::false_type {};

        // Range is never empty: it might well be convertible then!
        template <class Range> struct range_is_convertible <Range, true>
        : boost::mpl::and_ <
            std::is_convertible <typename result_of <
                    callable::first (direction::front, Range const &)>::type,
                first_stored_type>,
            typename rest_type::template range_is_convertible <typename
                result_of <callable::drop (direction::front, Range)>::type>
        > {};

        /**
        Whether all elements of \a Range are constructible and the length may be
        equal.
        */
        template <class Range, bool AlwaysEmpty
            = always_empty <direction::front, Range>::value>
        struct range_is_constructible;

        // Range is always empty: not constructible.
        template <class Range> struct range_is_constructible <Range, true>
        : rime::false_type {};

        template <class Range> struct range_is_constructible <Range, false>
        : boost::mpl::and_ <
            std::is_constructible <first_stored_type, typename result_of <
                    callable::first (direction::front, Range const &)>::type>,
            typename rest_type::template range_is_constructible <typename
                result_of <callable::drop (direction::front, Range)>::type>
        > {};

        /**
        Whether all elements of \a Range are assignable and the length could be
        equal.
        */
        template <class Range, bool KnownEmpty
            = always_empty <direction::front, Range>::value>
        struct range_is_assignable;

        template <class Range> struct range_is_assignable <Range, true>
        : rime::false_type {};

        template <class Range> struct range_is_assignable <Range, false>
        : boost::mpl::and_ <
            utility::is_assignable <
                First &, typename result_of <
                    callable::first (direction::front, Range const &)>::type>,
            typename rest_type::template range_is_assignable <typename
                result_of <callable::drop (direction::front, Range)>::type>
        > {};

        static constexpr bool is_copy_assignable =
            rest_type::is_copy_assignable &&
                utility::is_assignable <First &, First const &>::value;
        static constexpr bool is_move_assignable =
            rest_type::is_move_assignable &&
                utility::is_assignable <First &, First &&>::value;

    public:
        elements() : contain_type(), rest_type() {}

        template <class FirstArgument, class ... OtherArguments>
            explicit elements (from_elements, FirstArgument && first_argument,
                OtherArguments && ... other_arguments)
        : contain_type (std::forward <FirstArgument> (first_argument)),
            rest_type (from_elements(),
                std::forward <OtherArguments> (other_arguments) ...) {}

        template <class Range>
        elements (from_range, Range && range)
        : contain_type (first_if_size_matches (front, range)),
            rest_type (from_range(),
                drop_if_size_matches (front, std::forward <Range> (range))) {}

        // This could be faster than the generic constructors.
        elements (elements const & that)
        : contain_type (that), rest_type (that) {}

        elements (elements && that)
        : contain_type (std::move (that)), rest_type (std::move (that)) {}

        // Don't use assignment.
        elements & operator= (elements const &) = default;
        elements & operator= (elements &&) = default;

        template <class Range> elements & operator= (Range && range) {
            this->first_element() = first_if_size_matches (front, range);
            *static_cast <rest_type *> (this) =
                drop_if_size_matches (front, range);
            return *this;
        }

        void swap (elements & that) {
            using std::swap;
            swap (this->first_element(), that.first_element());
            rest_type::swap (that);
        }
    };

    template <class Dummy> struct dummy {};

} // namespace tuple_detail


/*
tuple itself.
*/
template <class ... Types> class tuple {
    typedef tuple_detail::elements <Types ...> elements_type;
    elements_type elements_;

    elements_type & elements() { return elements_; }
    elements_type const & elements() const { return elements_; }

    template <std::size_t> friend class tuple_detail::extract;

    typedef meta::vector <typename utility::storage::store <Types>::type ...>
        stored_types;

    template <class View> struct range_is_constructible_but_not_convertible
    : rime::bool_ <
        elements_type::template range_is_constructible <View>::value
        && !elements_type::template range_is_convertible <View>::value>
    {};

    struct dummy_type {};

    /**
    Type that is \c tuple if all elements are copy-assignable, and otherwise
    is \c dummy_type.
    This is used to conditionally enable the copy constructor.
    */
    typedef typename std::conditional <elements_type::is_copy_assignable,
        tuple, dummy_type>::type tuple_if_copy_assignable;

    /**
    Type that is \c tuple if all elements are move-assignable, and otherwise
    is \c dummy_type.
    This is used to conditionally enable the move constructor.
    */
    typedef typename std::conditional <elements_type::is_move_assignable,
        tuple, dummy_type>::type tuple_if_move_assignable;

public:
    /**
    Default-construct all elements.
    This is provided only if all elements are default-constructible.
    \internal
    Compilers, like GCC 4.6, that don't like SFINAE when the type is
    instantiated (because enable<false> is already known to contain no type)
    need the indirection of Types2.
    */
    template <class Types2 = meta::vector <Types ...>,
        class Enable = typename boost::enable_if <
            utility::are_default_constructible <Types2>>::type>
    tuple() : elements_() {}

    tuple (tuple const & that) : elements_ (that.elements_) {}
    tuple (tuple && that) : elements_ (std::move (that.elements_)) {}

    /**
    Construct element-wise.
    Each type must be constructible from the corresponding argument passed in.

    The order in which the elements are constructed is not defined.
    */
    template <class ... Arguments, class Enable = typename
        boost::enable_if <utility::are_constructible <
            stored_types, meta::vector <Arguments ...>>
    >::type>
    explicit tuple (Arguments && ... arguments)
    : elements_ (tuple_detail::from_elements(),
        std::forward <Arguments> (arguments) ...) {}

    /**
    Construct from another range.
    If the range is known to be the same length, and the types are pairwise
    convertible, then this implicit constructor is provided.

    The order in which the range is traversed and in which the elements are
    constructed is not defined.
    */
    template <class Range, class Enable = typename
        boost::enable_if <typename elements_type::template
            range_is_convertible <typename range::result_of <
                callable::view_once (direction::front, Range)>::type>>::type>
    tuple (Range && range, dummy_type = dummy_type())
    : elements_ (tuple_detail::from_range(),
        view_once (front, std::forward <Range> (range)))
    {}

    /**
    Construct from another range.
    If the range is not known to be the same length, or the types are pairwise
    constructible but not convertible, then this explicit constructor is
    provided.

    The order in which the range is traversed and in which the elements are
    constructed is not defined.

    \throw size_mismatch Iff the size of the range passed in is not the size of
    this.
    */
    template <class Range, class Enable = typename
        boost::enable_if <range_is_constructible_but_not_convertible <
            typename range::result_of <
                callable::view_once (direction::front, Range)>::type>>::type>
    explicit tuple (Range && range)
    : elements_ (tuple_detail::from_range(),
        view_once (front, std::forward <Range> (range)))
    {}

    /**
    Assign from another range.

    If traversal of the range or assignment of any element throws an exception,
    then it is not defined which elements will have their new values and which
    ones do not.

    \throw size_mismatch Iff the size of the range passed in is not the size of
    this.
    */
    template <class Range,
        class Enable = typename boost::enable_if <typename
            elements_type::template range_is_assignable <typename
                range::result_of <callable::view_once (direction::front, Range)
            >::type>>::type,
        class Enable2 = typename
            utility::disable_if_same_or_derived <tuple, Range>::type>
    tuple & operator= (Range && range) {
        elements_ = view_once (front, std::forward <Range> (range));
        return *this;
    }

    // Explicit copy and move assignment.
    tuple & operator= (tuple_if_copy_assignable const & that) {
        elements_ = view_once (front, that);
        return *this;
    }
    tuple & operator= (tuple_if_move_assignable && that) {
        elements_ = view_once (front, std::move (that));
        return *this;
    }

    /**
    Call \c swap (found with argument-dependent lookup or using \c std::swap)
    for each pair of elements.
    */
    void swap (tuple & that) { elements().swap (that.elements()); }
};

/**
Call \c swap (found with argument-dependent lookup or using \c std::swap)
for each pair of elements.
*/
template <class ... Types>
inline void swap (tuple <Types ...> & tuple1, tuple <Types ...> & tuple2)
{ tuple1.swap (tuple2); }

namespace make_tuple_from_detail {

    template <template <class> class Transform, class Types>
        struct tuple_from_types
    : tuple_from_types <Transform, typename meta::as_vector <Types>::type> {};

    template <template <class> class Transform, class ... Types>
        struct tuple_from_types <Transform, meta::vector <Types ...>>
    { typedef tuple <typename Transform <Types>::type ...> type; };

    template <template <class> class Transform, class Range> struct tuple_from {
        typedef typename std::result_of <callable::view_once (Range)>::type
            view_type;
        static_assert (!is_homogeneous <direction::front, view_type>::value,
            "The range passed in is homogeneous and potentially infinite. "
            "Unable to convert it into a tuple.");
        typedef typename tuple_from_types <Transform,
            typename element_types <view_type>::type>::type type;
    };

    template <class Type> struct add_reference { typedef Type & type; };
    template <class Type> struct identity { typedef Type type; };

} // namespace make_tuple_from_detail

/**
Make a tuple from the arguments passed in.
The arguments are stripped of qualifications.

This is equivalent to \c std::make_tuple except that it returns a
\c range::tuple.
*/
template <class ... Types> inline
    tuple <Types ...> make_tuple (Types const & ... arguments)
{ return tuple <Types ...> (arguments ...); }

/** \brief
Make a tuple from the range passed in, stripping the types of qualifications.

The range is traversed in direction::front.
The range must be known to end after a fixed number of elements.
This could be a heterogeneous container, like std::tuple.
It could also be an adapted heterogeneous container.

For a variable-length range, the resulting tuple has the longest number of
elements possible.
If \a range is a standard homogeneous container, this is obviously impossible,
and a compile error about recursive template instantiations will result.

\throw size_mismatch
    If the range turns out at compile time to finish sooner than it could.
    In this case, the tuple type will have more elements than the range turns
    out to have, so constructing the tuple type elicits the exception.
\param range
    The range to construct the range from.
*/
template <class Range> inline auto make_tuple_from (Range && range)
RETURNS (typename make_tuple_from_detail::tuple_from <std::decay, Range>::type (
    std::forward <Range> (range)));

/** \brief
Make a tuple of references to each of the arguments.

This is equivalent to \c std::tie except that it returns a \c range::tuple.
*/
template <class ... Types> inline
    tuple <Types & ...> tie (Types & ... arguments)
{ return tuple <Types & ...> (arguments ...); }

/** \brief
Make a tuple of references to the elements of the range passed in.

The range is traversed in direction::front.
The range must be known to end after a fixed number of elements.
This could be a heterogeneous container, like std::tuple.
It could also be an adapted heterogeneous container.

For a variable-length range, the resulting tuple has the longest number of
elements possible.
If \a range is a standard homogeneous container, this is obviously impossible,
and a compile error about recursive template instantiations will result.

\throw size_mismatch
    If the range turns out at compile time to finish sooner than it could.
    In this case, the tuple type will have more elements than the range turns
    out to have, so constructing the tuple type elicits the exception.
\param range
    The range to construct the range from.
*/
template <class Range> inline auto tie_from (Range && range)
RETURNS (typename make_tuple_from_detail::tuple_from <
    make_tuple_from_detail::add_reference, Range>::type (
        std::forward <Range> (range)));

/** \brief
Make a tuple of rvalue references to each of the arguments.

This is equivalent to \c std::forward_as_tuple except that it returns a
\c range::tuple.
*/
template <class ... Types> inline
    tuple <Types && ...> forward_as_tuple (Types && ... arguments)
{ return tuple <Types && ...> (std::forward <Types> (arguments) ...); }

/** \brief
Make a tuple that is a copy of the range passed in, down to the exact element
types that first() returns.

The types will often be references.
Notably, <c>copy_tuple_from</c> applied to a tuple will usually result in a
tuple with a different type.

The range is traversed in direction::front.
The range must be known to end after a fixed number of elements.
This could be a heterogeneous container, like std::tuple.
It could also be an adapted heterogeneous container.

For a variable-length range, the resulting tuple has the longest number of
elements possible.
If \a range is a standard homogeneous container, this is obviously impossible,
and a compile error about recursive template instantiations will result.

\throw size_mismatch
    If the range turns out at compile time to finish sooner than it could.
    In this case, the tuple type will have more elements than the range turns
    out to have, so constructing the tuple type elicits the exception.
\param range
    The range to construct the range from.
*/
template <class Range> inline auto copy_tuple_from (Range && range)
RETURNS (typename make_tuple_from_detail::tuple_from <
    make_tuple_from_detail::identity, Range>::type (
        std::forward <Range> (range)));

namespace tuple_detail {
    /**
    Class that anything can be assigned to.
    Assignment is a const operation (!) and a no-op.
    */
    struct vacuously_assignable {
        template <class Anything>
            vacuously_assignable const & operator = (Anything const &) const
        { return *this; }
    };
} // namespace tuple_detail

/**
Evaluate to the type at position \a Position in \a Tuple, counted from the
start of the tuple.
*/
template <std::size_t Position, class Tuple>
    struct element_type
: tuple_detail::extract_at <Position, Tuple>::template element <Tuple> {};

/**
Object that anything can be assigned to syntactically, but it has no effect.
This is useful in conjunction with \c tie.
std::ignore could be used just as well, but this can be awkward in practice.
Because of argument-dependent lookup,
    using range::tie; tie (std::ignore);
is ambiguous between range::tie and std::tie. On the other hand,
    using range::tie; tie (range::ignore);
is fine.
*/
static auto const ignore = tuple_detail::vacuously_assignable();

/*
tuple_view: the lightweight view of tuple.
*/

namespace tuple_detail {

    template <std::size_t Begin, std::size_t End, class TupleReference>
        class tuple_view
    {
        static_assert (std::is_reference <TupleReference>::value, "");
        typedef typename std::add_pointer <TupleReference>::type pointer_type;
        pointer_type tuple_;
    public:
        static constexpr std::size_t begin_position = Begin;
        static constexpr std::size_t end_position = End;
        static constexpr std::size_t tuple_size
            = range::tuple_size <TupleReference>::value;

        tuple_view (TupleReference tuple) : tuple_ (&tuple) {}

        template <std::size_t ThatBegin, std::size_t ThatEnd>
            tuple_view (tuple_view <ThatBegin, ThatEnd, TupleReference> const &
                that)
        : tuple_ (that.pointer())
        {
            static_assert (ThatBegin <= Begin, "Views can only shrink.");
            static_assert (End <= ThatEnd, "Views can only shrink.");
        }

        TupleReference tuple() const
        { return static_cast <TupleReference> (*tuple_); }

        pointer_type pointer() const { return tuple_; }
    };

    template <std::size_t Size> struct tuple_view_tag;

} // namespace tuple_detail

template <std::size_t Begin, std::size_t End, class TupleReference>
    struct tag_of_qualified <
        tuple_detail::tuple_view <Begin, End, TupleReference>>
{ typedef tuple_detail::tuple_view_tag <(End - Begin)> type; };

namespace operation {

    // empty.
    template <std::size_t Size>
        struct empty <tuple_detail::tuple_view_tag <Size>, direction::front>
    : rime::callable::always_default <rime::bool_<(Size == 0)>> {};

    // size.
    template <std::size_t Size>
        struct size <tuple_detail::tuple_view_tag <Size>, direction::front>
    : rime::callable::always_default <rime::size_t <Size>> {};

    // first.
    template <std::size_t Size>
        struct first <tuple_detail::tuple_view_tag <Size>, direction::front,
            typename std::enable_if <(Size > 0)>::type>
    {
        template <class View> auto operator() (
            direction::front, View const & view) const
        RETURNS (range::tuple_detail::extract <
            (View::tuple_size - View::begin_position - 1)>() (view.tuple()));
    };
    template <std::size_t Size>
        struct first <tuple_detail::tuple_view_tag <Size>, direction::back,
            typename std::enable_if <(Size > 0)>::type>
    {
        template <class View> auto operator() (
            direction::back, View const & view) const
        RETURNS (range::tuple_detail::extract <
            (View::tuple_size - View::end_position)>() (view.tuple()));
    };

    // at.
    template <std::size_t Size, class Index>
        struct at_constant <tuple_detail::tuple_view_tag <Size>,
            direction::front, Index,
            typename std::enable_if <(Index::value < Size)>::type>
    {
        template <class View> auto operator() (
            direction::front, Index const &, View const & view) const
        RETURNS (range::tuple_detail::extract <
            (View::tuple_size - View::begin_position - Index::value - 1)>() (
                view.tuple()));
    };
    template <std::size_t Size, class Index>
        struct at_constant <tuple_detail::tuple_view_tag <Size>,
            direction::back, Index,
            typename std::enable_if <(Index::value < Size)>::type>
    {
        template <class View> auto operator() (
            direction::back, Index const &, View const & view) const
        RETURNS (range::tuple_detail::extract <
            (View::tuple_size - View::end_position + Index::value)>() (
                view.tuple()));
    };

    // drop.
    template <class Increment, std::size_t Size>
        struct drop_constant <
            tuple_detail::tuple_view_tag <Size>, direction::front, Increment,
            typename std::enable_if <(Increment::value <= Size)>::type>
    {
        template <std::size_t Begin, std::size_t End, class TupleReference>
        tuple_detail::tuple_view <Begin + Increment::value, End, TupleReference>
        operator() (direction::front, Increment,
            tuple_detail::tuple_view <Begin, End, TupleReference> const & v)
        const
        {
            return tuple_detail::tuple_view <
                Begin + Increment::value, End, TupleReference> (v);
        }
    };
    template <class Increment, std::size_t Size>
        struct drop_constant <
            tuple_detail::tuple_view_tag <Size>, direction::back, Increment,
            typename std::enable_if <(Increment::value <= Size)>::type>
    {
        template <std::size_t Begin, std::size_t End, class TupleReference>
        tuple_detail::tuple_view <Begin, End - Increment::value, TupleReference>
        operator() (direction::back, Increment,
            tuple_detail::tuple_view <Begin, End, TupleReference> const & v)
        const
        {
            return tuple_detail::tuple_view <
                Begin, End - Increment::value, TupleReference> (v);
        }
    };

    // It would be possible to specialised "fold" to improve compile times.
    // However, the return type must still be computed, so the improvement would
    // not be great.

    // for_each.
    // This does not need to be specialised, but this gives a constant-time
    // improvement in compile time.
    // These specialisations are only for direction::front.
    // This is unrolled for up to 4 elements.
    // Then, a recursive call is made for every next 4 elements.
    template <class Function> struct for_each <
        tuple_detail::tuple_view_tag <0>, direction::front, Function>
    {
        template <class View>
        void operator() (direction::front, Function &&, View const &) const {}
    };
    template <class Function> struct for_each <
        tuple_detail::tuple_view_tag <1>, direction::front, Function>
    {
        template <class View>
        void operator() (direction::front, Function && function,
            View const & view) const
        {
            static constexpr std::size_t begin_index =
                View::tuple_size - View::begin_position - 1;
            function (range::tuple_detail::extract <
                (begin_index - 0)>() (view.tuple()));
        }
    };
    template <class Function> struct for_each <
        tuple_detail::tuple_view_tag <2>, direction::front, Function>
    {
        template <class View>
        void operator() (direction::front, Function && function,
            View const & view) const
        {
            static constexpr std::size_t begin_index =
                View::tuple_size - View::begin_position - 1;
            function (range::tuple_detail::extract <
                (begin_index - 0)>() (view.tuple()));
            function (range::tuple_detail::extract <
                (begin_index - 1)>() (view.tuple()));
        }
    };
    template <class Function> struct for_each <
        tuple_detail::tuple_view_tag <3>, direction::front, Function>
    {
        template <class View>
        void operator() (direction::front, Function && function,
            View const & view) const
        {
            static constexpr std::size_t begin_index =
                View::tuple_size - View::begin_position - 1;
            function (range::tuple_detail::extract <
                (begin_index - 0)>() (view.tuple()));
            function (range::tuple_detail::extract <
                (begin_index - 1)>() (view.tuple()));
            function (range::tuple_detail::extract <
                (begin_index - 2)>() (view.tuple()));
        }
    };
    // Size >= 4
    template <std::size_t Size, class Function> struct for_each <
        tuple_detail::tuple_view_tag <Size>, direction::front, Function>
    {
        template <std::size_t Begin, std::size_t End, class TupleReference>
        void operator() (direction::front const & direction,
            Function && function,
            tuple_detail::tuple_view <Begin, End, TupleReference> const & view)
        const
        {
            typedef tuple_detail::tuple_view <Begin, End, TupleReference>
                view_type;
            static constexpr std::size_t begin_index =
                view_type::tuple_size - view_type::begin_position - 1;
            function (range::tuple_detail::extract <
                (begin_index - 0)>() (view.tuple()));
            function (range::tuple_detail::extract <
                (begin_index - 1)>() (view.tuple()));
            function (range::tuple_detail::extract <
                (begin_index - 2)>() (view.tuple()));
            function (range::tuple_detail::extract <
                (begin_index - 3)>() (view.tuple()));

            for_each <tuple_detail::tuple_view_tag <Size - 4>,
                direction::front, Function> recursive;
            recursive (direction, std::forward <Function> (function),
                tuple_detail::tuple_view <Begin + 4, End, TupleReference> (
                    view));
        }
    };

    namespace tuple_detail {

        template <bool Move, class ... Types> struct make_view_tuple;

        template <class ... Types> struct make_view_tuple <false, Types ...> {
            typedef range::tuple <Types ...> tuple_type;

            typedef range::tuple_detail::tuple_view <0, sizeof ... (Types),
                tuple_type &> view_type;
            typedef range::tuple_detail::tuple_view <0, sizeof ... (Types),
                tuple_type const &> const_view_type;

            view_type operator() (tuple_type & t) const
            { return view_type (t); }

            const_view_type operator() (tuple_type const & t) const
            { return const_view_type (t); }
        };

        template <class ... Types> struct make_view_tuple <true, Types ...>
        : make_view_tuple <false, Types ...>
        {
            typedef make_view_tuple <false, Types ...> base_type;
            using base_type::operator();
            typedef typename base_type::tuple_type tuple_type;

            typedef range::tuple_detail::tuple_view <0, sizeof ... (Types),
                tuple_type &&> rvalue_view_type;

            rvalue_view_type operator() (tuple_type && t) const
            { return rvalue_view_type (std::move (t)); }
        };

    } // namespace tuple_detail

    template <bool Move, class ... Types, class Directions>
        struct make_view <Move, heavyweight_tag <tuple <Types ...>>,
            Directions,
            typename detail::enable_if_front_back <Directions>::type>
    : helper::call_with_last <1, Directions,
        tuple_detail::make_view_tuple <Move, Types ...>> {};

} // namespace operation

} // namespace range

#endif  // RANGE_CONTAINER_TUPLE_HPP_INCLUDED
