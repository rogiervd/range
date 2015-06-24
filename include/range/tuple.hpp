/*
Copyright 2013-2015 Rogier van Dalen.

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
Define a general heterogeneous container.
*/

#ifndef RANGE_CONTAINER_TUPLE_HPP_INCLUDED
#define RANGE_CONTAINER_TUPLE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/if.hpp>
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
#include "equal.hpp"
#include "less_lexicographical.hpp"

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

Tuples can be compared for equality and order.
They are equal if the number of elements is equal and the elements compare equal
in parallel.
The ordering operators perform lexicographical comparison.
Views on tuples do not define comparison operators.

The layout of the elements in memory is undefined.
Connected with this is the layout

\todo Implement tuple_cat? Probably rather make_tuple_from (concatenate (...)).
\todo uses_allocator
\todo Comparison between tuples.

\todo no_except specification.
\todo constexpr specification.
*/
template <class ... Types> class tuple;

namespace operation {
    struct tuple_tag : heavyweight_tag {};
} // namespace operation

template <class ... Types> struct tag_of_qualified <tuple <Types...>>
{ typedef operation::tuple_tag type; };

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
    contained_type \c \<Type>.
    By using <c>decltype (t.get_contained_type (contain_index \<..>()))</c>,
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
        : always_empty <Range, direction::front> {};

        // constructible if \a Range can be empty.
        template <class Range> struct range_is_constructible
        : boost::mpl::not_ <never_empty <Range, direction::front>> {};

        static constexpr bool is_copy_assignable = true;
        static constexpr bool is_move_assignable = true;

        // assignable if \a Range can be empty.
        template <class Range> struct range_is_assignable
        : boost::mpl::not_ <never_empty <Range, direction::front>> {};

    public:
        elements() {}

        elements (elements const &) {}
        elements (elements &&) {}

        explicit elements (from_elements) {}

        /**
        \throw size_mismatch if the range is not empty.
        */
        template <class Range> static Range && maybe_chop (Range && range) {
            if (!empty (range, front))
                throw size_mismatch();
            return static_cast <Range &&> (range);
        }

        template <class Range> explicit elements (from_range, Range &&) {}

        template <class Range> elements & operator= (Range && range) {
            if (!empty (range, front))
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
            = never_empty <Range, direction::front>::value>
        struct range_is_convertible;

        template <class Range> struct range_is_convertible <Range, false>
        : rime::false_type {};

        // Range is never empty: it might well be convertible then!
        template <class Range> struct range_is_convertible <Range, true>
        : boost::mpl::and_ <
            std::is_convertible <typename result_of <
                    callable::first (Range, direction::front)>::type,
                first_stored_type>,
            typename rest_type::template range_is_convertible <typename
                result_of <callable::drop (Range, direction::front)>::type>
        > {};

        /**
        Whether all elements of \a Range are constructible and the length may be
        equal.
        */
        template <class Range, bool AlwaysEmpty
            = always_empty <Range, direction::front>::value>
        struct range_is_constructible;

        // Range is always empty: not constructible.
        template <class Range> struct range_is_constructible <Range, true>
        : rime::false_type {};

        template <class Range> struct range_is_constructible <Range, false>
        : boost::mpl::and_ <
            std::is_constructible <first_stored_type, typename result_of <
                    callable::first (Range, direction::front)>::type>,
            typename rest_type::template range_is_constructible <typename
                result_of <callable::drop (Range, direction::front)>::type>
        > {};

        /**
        Whether all elements of \a Range are assignable and the length could be
        equal.
        */
        template <class Range, bool KnownEmpty
            = always_empty <Range, direction::front>::value>
        struct range_is_assignable;

        template <class Range> struct range_is_assignable <Range, true>
        : rime::false_type {};

        template <class Range> struct range_is_assignable <Range, false>
        : boost::mpl::and_ <
            utility::is_assignable <
                First &, typename result_of <
                    callable::first (Range, direction::front)>::type>,
            typename rest_type::template range_is_assignable <typename
                result_of <callable::drop (Range, direction::front)>::type>
        > {};

        static constexpr bool is_copy_assignable =
            rest_type::is_copy_assignable &&
                utility::is_assignable <First &, First const &>::value;
        static constexpr bool is_move_assignable =
            rest_type::is_move_assignable &&
                utility::is_assignable <First &, First &&>::value;

    public:
        elements() : contain_type(), rest_type() {}

        /**
        Construct from elements, given as separate arguments.
        */
        template <class FirstArgument, class ... OtherArguments>
            explicit elements (from_elements, FirstArgument && first_argument,
                OtherArguments && ... other_arguments)
        : contain_type (std::forward <FirstArgument> (first_argument)),
            rest_type (from_elements(),
                std::forward <OtherArguments> (other_arguments) ...) {}

        /*
        Construct from a range.
        This works around delegate constructors, for compilers that do support
        those.
        Construct this type with
            elements <...> (from_range(), elements <...>::maybe_chop (range))
        where "range" may be an rvalue reference.
        "maybe_chop" will either return the range itself, if first() and drop()
        can be applied apart from each other, or return a chopped <...> if not.
        The actual constructor will then use either the range, or the
        chopped <...> to construct itself.
        */
        // Range that allows first (Range const &) and drop (Range)
        /**
        Forward the range...
        \throw size_mismatch if the range is not empty.
        */
        template <class Range> static
        typename boost::enable_if <boost::mpl::and_ <
                has <callable::first (Range const &, direction::front)>,
                has <callable::drop (Range, direction::front)>>,
            Range &&>::type
        maybe_chop (Range && range) {
            if (empty (range, front))
                throw size_mismatch();
            return static_cast <Range &&> (range);
        }

        // ... to be used by the corresponding constructor.
        template <class Range>
        elements (from_range, Range && range)
        :   contain_type (range::first (range, front)),
            rest_type (from_range(), rest_type::maybe_chop (
                    range::drop (std::forward <Range> (range), front)))
        {}

        // Range that does not allow first (Range const &) and drop (Range).
        /**
        Actually apply "chop" on the range...
        \throw size_mismatch if the range is not empty.
        */
        template <class Range> static
        typename boost::lazy_disable_if <boost::mpl::and_ <
                has <callable::first (Range const &, direction::front)>,
                has <callable::drop (Range, direction::front)>>,
            result_of <callable::chop (Range, direction::front)>>::type
        maybe_chop (Range && range)
        {
            if (empty (range, front))
                throw size_mismatch();
            return range::chop (std::forward <Range> (range), range::front);
        }

        // ... to be used by the corresponding constructor.
        template <class FirstInput, class RestInput>
            elements (from_range, chopped <FirstInput, RestInput> && c)
        : contain_type (c.move_first()),
            rest_type (from_range(), rest_type::maybe_chop (c.move_rest())) {}

        // This could be faster than the generic constructors.
        elements (elements const & that)
        : contain_type (that), rest_type (that) {}

        elements (elements && that)
        : contain_type (std::move (that)), rest_type (std::move (that)) {}

        // Don't use assignment.
        elements & operator= (elements const &) = default;
        elements & operator= (elements &&) = default;

        template <class Range> elements & operator= (Range && range) {
            if (empty (range, front))
                throw size_mismatch();
            auto chopped = range::chop (std::forward <Range> (range), front);
            this->first_element() = chopped.move_first();
            *static_cast <rest_type *> (this) = chopped.move_rest();
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

    template <class Type> struct not_this_tuple
    : boost::mpl::not_ <std::is_same <typename std::decay <Type>::type, tuple>>
    {};

    template <class Range> struct range_is_convertible_impl
    : elements_type::template range_is_convertible <typename
        range::result_of <callable::view_once (Range, direction::front)>::type>
    {};
    template <class Range> struct range_is_convertible
    : boost::mpl::and_ <
        has <callable::view_once (Range, direction::front)>,
        range_is_convertible_impl <Range>> {};

    template <class Range> struct range_is_constructible_impl
    : elements_type::template range_is_constructible <typename
        range::result_of <callable::view_once (Range, direction::front)>::type>
    {};
    template <class Range> struct range_is_constructible
    : boost::mpl::and_ <
        has <callable::view_once (Range, direction::front)>,
        range_is_constructible_impl <Range>> {};

    template <class Range> struct range_is_constructible_but_not_convertible
    : boost::mpl::and_ <
        range_is_constructible <Range>,
        boost::mpl::not_ <range_is_convertible <Range>>
    > {};

    template <class Range> struct range_is_assignable_impl
    : elements_type::template range_is_assignable <typename
        range::result_of <callable::view_once (Range, direction::front)>::type>
    {};
    template <class Range> struct range_is_assignable
    : boost::mpl::and_ <
        has <callable::view_once (Range, direction::front)>,
        range_is_assignable_impl <Range>> {};

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
        boost::enable_if <boost::mpl::and_<
            not_this_tuple <Range>, is_range <Range>,
            range_is_convertible <Range>>>::type>
    tuple (Range && range, dummy_type = dummy_type())
    : elements_ (tuple_detail::from_range(), elements_type::maybe_chop (
        view_once (std::forward <Range> (range), front)))
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
        boost::enable_if <boost::mpl::and_<
            not_this_tuple <Range>, is_range <Range>,
            range_is_constructible_but_not_convertible <Range>
        >>::type>
    explicit tuple (Range && range)
    : elements_ (tuple_detail::from_range(), elements_type::maybe_chop (
        view_once (std::forward <Range> (range), front)))
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
        class Enable = typename boost::enable_if <
            boost::mpl::and_<not_this_tuple <Range>, is_range <Range>,
            range_is_assignable <Range>
        >>::type>
    tuple & operator= (Range && range) {
        elements_ = view_once (std::forward <Range> (range), front);
        return *this;
    }

    // Explicit copy and move assignment.
    tuple & operator= (tuple_if_copy_assignable const & that) {
        elements_ = view_once (that, front);
        return *this;
    }
    tuple & operator= (tuple_if_move_assignable && that) {
        elements_ = view_once (std::move (that), front);
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
        typedef typename result_of <
            callable::view_once (Range, direction::front)>::type view_type;
        static_assert (!is_homogeneous <view_type, direction::front>::value,
            "The range passed in is homogeneous and potentially infinite. "
            "Unable to convert it into a tuple.");
        typedef typename tuple_from_types <Transform,
            typename element_types <view_type>::type>::type type;
    };

    template <class Type> struct add_reference { typedef Type & type; };
    template <class Type> struct identity { typedef Type type; };

} // namespace make_tuple_from_detail

/** \brief
Compare two tuples for equality.
*/
template <class ... Types1, class ... Types2> inline
    auto operator== (
        tuple <Types1 ...> const & t1, tuple <Types2 ...> const & t2)
RETURNS (equal (t1, t2));

/** \brief
Compare two tuples for inequality.
*/
template <class ... Left, class ... Right> inline
    auto operator!= (
        tuple <Left ...> const & left, tuple <Right ...> const & right)
RETURNS (!equal (left, right));

/** \brief
Compare two tuples lexicographically.
*/
template <class ... Left, class ... Right> inline
    auto operator< (
        tuple <Left ...> const & left, tuple <Right ...> const & right)
RETURNS (less_lexicographical (left, right));

/** \brief
Compare two tuples lexicographically.
*/
template <class ... Left, class ... Right> inline
    auto operator<= (
        tuple <Left ...> const & left, tuple <Right ...> const & right)
RETURNS (!less_lexicographical (right, left));

/** \brief
Compare two tuples lexicographically.
*/
template <class ... Left, class ... Right> inline
    auto operator> (
        tuple <Left ...> const & left, tuple <Right ...> const & right)
RETURNS (less_lexicographical (right, left));

/** \brief
Compare two tuples lexicographically.
*/
template <class ... Left, class ... Right> inline
    auto operator>= (
        tuple <Left ...> const & left, tuple <Right ...> const & right)
RETURNS (!less_lexicographical (left, right));

namespace callable {

    struct make_tuple {
        template <class ... Types> inline
            tuple <Types ...> operator() (Types const & ... arguments) const
        { return tuple <Types ...> (arguments ...); }
    };

    struct make_tuple_from {
        template <class Range, class Result = typename
            make_tuple_from_detail::tuple_from <std::decay, Range>::type>
        Result operator() (Range && range) const
        { return Result (std::forward <Range> (range)); }
    };

    struct tie {
        template <class ... Types>
            tuple <Types & ...> operator() (Types & ... arguments) const
        { return tuple <Types & ...> (arguments ...); }
    };

    struct tie_from {
        template <class Range, class Result = typename
            make_tuple_from_detail::tuple_from <
                make_tuple_from_detail::add_reference, Range>::type>
        Result operator() (Range && range) const
        { return Result (std::forward <Range> (range)); }
    };

    struct forward_as_tuple {
        template <class ... Types>
            tuple <Types && ...> operator() (Types && ... arguments) const
        { return tuple <Types && ...> (std::forward <Types> (arguments) ...); }
    };

    struct copy_tuple_from {
        template <class Range, class Result = typename
            make_tuple_from_detail::tuple_from <
                make_tuple_from_detail::identity, Range>::type>
        Result operator() (Range && range) const
        { return Result (std::forward <Range> (range)); }
    };

} // namespace callable

/** \brief
Make a tuple from the arguments passed in.

The arguments are stripped of qualifications.

This is equivalent to \c std::make_tuple except that it returns a
\c range::tuple.
*/
static auto const make_tuple = callable::make_tuple();

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
    If the range turns out at run time to finish sooner than it should.
    In this case, the tuple type will have more elements than the range turns
    out to have, so constructing the tuple type elicits the exception.
\param range
    The range to construct the range from.
*/
static auto const make_tuple_from = callable::make_tuple_from();

/** \brief
Make a tuple of references to each of the arguments.

This is equivalent to \c std::tie except that it returns a \c range::tuple.
*/
static auto const tie = callable::tie();

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
    If the range turns out at run time to finish sooner than it should.
    In this case, the tuple type will have more elements than the range turns
    out to have, so constructing the tuple type elicits the exception.
\param range
    The range to construct the range from.
*/
static auto const tie_from = callable::tie_from();

/** \brief
Make a tuple of rvalue references to each of the arguments.

This is equivalent to \c std::forward_as_tuple except that it returns a
\c range::tuple.
*/
static auto const forward_as_tuple = callable::forward_as_tuple();

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
    If the range turns out at run time to finish sooner than it should.
    In this case, the tuple type will have more elements than the range turns
    out to have, so constructing the tuple type elicits the exception.
\param range
    The range to construct the range from.
*/
static auto const copy_tuple_from = callable::copy_tuple_from();

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
\code
    using range::tie; tie (std::ignore);
\endcode
is ambiguous between range::tie and std::tie.

On the other hand,
\code
    using range::tie; tie (range::ignore);
\endcode
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
        static constexpr bool is_empty = (Begin == End);
        static constexpr std::size_t view_size = (End - Begin);
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

    private:
        friend class helper::member_access;

        rime::bool_ <is_empty> empty (direction::front) const
        { return rime::bool_ <is_empty>(); }

        rime::size_t <(End - Begin)> size (direction::front) const
        { return rime::size_t <(End - Begin)>(); }

        // first.
        struct not_front;
        typedef typename std::conditional <is_empty, not_front, direction::front
            >::type front_if_not_empty;

        struct not_back;
        typedef typename std::conditional <is_empty, not_back, direction::back
            >::type back_if_not_empty;

        template <bool Empty = is_empty,
            class Enable = typename std::enable_if <!Empty>::type,
            class Extract = extract <(tuple_size - Begin - 1)>>
        typename Extract::template result <TupleReference>::type
        first (direction::front const &) const
        { return Extract() (tuple()); }

        template <bool Empty = is_empty,
            class Enable = typename std::enable_if <!Empty>::type,
            class Extract = extract <(tuple_size - end_position)>>
        typename Extract::template result <TupleReference>::type
        first (direction::back const &) const
        { return Extract() (tuple()); }

        // at.
        template <class Index, class Enable = typename
                std::enable_if <(Index::value < view_size)>::type,
            class Extract = extract <
                (tuple_size - begin_position - Index::value - 1)>>
        typename Extract::template result <TupleReference>::type
        at_constant (Index const & index, direction::front) const
        { return Extract() (tuple()); }

        template <class Index, class Enable = typename
                std::enable_if <(Index::value < view_size)>::type,
            class Extract = extract <
                (tuple_size - end_position + Index::value)>>
        typename Extract::template result <TupleReference>::type
        at_constant (Index const & index, direction::back) const
        { return Extract() (tuple()); }

        // drop.
        template <class Increment, class Enable = typename
                std::enable_if <(Increment::value <= view_size)>::type,
            class Result = tuple_detail::tuple_view <
                Begin + Increment::value, End, TupleReference>>
        Result drop_constant (Increment const & increment, direction::front)
            const
        { return Result (tuple()); }

        template <class Increment, class Enable = typename
                std::enable_if <(Increment::value <= view_size)>::type,
            class Result = tuple_detail::tuple_view <
                Begin, End - Increment::value, TupleReference>>
        Result drop_constant (Increment const & increment, direction::back)
            const
        { return Result (tuple()); }

        // It would be possible to specialised "fold" to improve compile times.
        // However, the return type must still be computed, so the improvement
        // would not be great.

        /* for_each. */
        /// Call a function, and return an int.
        /// This is just to homogenise the return value of the actual function.
        template <class Function> struct call_function {
            Function && function_;
            call_function (Function && function)
            : function_ (std::forward <Function> (function)) {}

            template <class Element> int operator() (Element && element) const {
                function_ (element);
                return 0;
            }
        };

        /**
        Use a trick: expand a template pack into an initialiser list.
        By using "Indices::value...", this calls the function with the correct
        elements.
        */
        template <class Function, class ... Indices>
            void apply_for_each (
                Function && function, meta::vector <Indices ...>) const
        {
            call_function <Function> call (std::forward <Function> (function));
            int dummy [] = { call (extract <
                ((tuple_size - begin_position - 1) - Indices::value)>() (
                    tuple())) ...};
            (void) dummy;
        }

        template <class Function> void for_each (
            direction::front, Function && function) const
        {
            apply_for_each (std::forward <Function> (function),
                typename meta::count <view_size>::type());
        }
    };

    template <std::size_t Size> struct tuple_view_tag {};

} // namespace tuple_detail

template <std::size_t Begin, std::size_t End, class TupleReference>
    struct tag_of_qualified <
        tuple_detail::tuple_view <Begin, End, TupleReference>>
{ typedef tuple_detail::tuple_view_tag <(End - Begin)> type; };

namespace operation {

    // implement_make_view for std::tuple.
    // Const reference.
    template <class ... Types,
        class Result = typename tuple_detail::tuple_view <
            0, sizeof ... (Types), tuple <Types ...> const &>>
    inline Result implement_make_view (tuple_tag,
        bool once, tuple <Types ...> const & tuple,
        helper::front_or_back, helper::front_or_back = helper::front_or_back())
    { return Result (tuple); }

    // Reference.
    template <class ... Types,
        class Result = typename tuple_detail::tuple_view <
            0, sizeof ... (Types), tuple <Types ...> &>>
    inline Result implement_make_view (tuple_tag,
        bool once, tuple <Types ...> & tuple,
        helper::front_or_back, helper::front_or_back = helper::front_or_back())
    { return Result (tuple); }

    // Rvalue reference.
    template <class ... Types,
        class Result = typename tuple_detail::tuple_view <
            0, sizeof ... (Types), tuple <Types ...> &&>>
    inline Result implement_make_view (tuple_tag,
        rime::true_type once, tuple <Types ...> && tuple,
        helper::front_or_back, helper::front_or_back = helper::front_or_back())
    { return Result (std::move (tuple)); }

} // namespace operation

} // namespace range

#endif  // RANGE_CONTAINER_TUPLE_HPP_INCLUDED
