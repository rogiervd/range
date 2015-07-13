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

/** \file
Provide a view of boost::optional as a range.
boost::optional is not a range (this would cause confusing overloads), so it
must be explicitly converted into a range.
*/

#ifndef RANGE_STD_VIEW_OPTIONAL_HPP_INCLUDED
#define RANGE_STD_VIEW_OPTIONAL_HPP_INCLUDED

#include <type_traits>

#include "rime/always.hpp"

#include "range/core.hpp"
#include "range/empty_view.hpp"

namespace range {

template <class Optional> class optional_view;

namespace view_optional_operation {
    struct optional_view_tag {};
} // namespace view_optional_operation

template <class Optional> struct tag_of_qualified <optional_view <Optional>>
{ typedef view_optional_operation::optional_view_tag type; };

/**
A view of an optional in directions "front" and "back".
This is empty iff the optional is empty, or a one-element range if the optional
contains a value.
*/
template <class Optional> class optional_view {
    Optional & optional_;

    typedef typename std::decay <Optional>::type::value_type decayed_value_type;
public:
    optional_view (Optional & optional) : optional_ (optional) {}

    // Constness op optional propagates to value type.
    typedef typename std::conditional <std::is_const <Optional>::value,
        decayed_value_type const, decayed_value_type>::type value_type;

private:
    friend class helper::member_access;

    bool empty() const { return !this->optional_; }
    bool empty (direction::front) const { return empty(); }

    unsigned size (direction::front) const
    { return this->optional_ ? 1 : 0; }

    // first.
    value_type & first (direction::front) const {
        assert (!empty());
        return this->optional_.get();
    }
    value_type & first (direction::back) const {
        assert (!empty());
        return this->optional_.get();
    }

    // drop_one.
    empty_view drop_one (direction::front) const {
        assert (!empty());
        return empty_view();
    }
    empty_view drop_one (direction::back) const {
        assert (!empty());
        return empty_view();
    }
};

namespace callable {

    struct view_optional {
        // Do not accept temporaries because that can only lead to tears.
        template <class Optional>
            optional_view <Optional> operator() (Optional & o) const
        { return optional_view <Optional> (o); }
    };

} // namespace callable

/** \brief
View an "optional" as a range containing zero or one elements.

\param optional
    A boost::optional or std::optional (when that becomes available).
    This should not be an rvalue, since a reference to the optional is stored.
*/
static auto const view_optional = callable::view_optional();

} // namespace range

#endif  // RANGE_STD_VIEW_OPTIONAL_HPP_INCLUDED
