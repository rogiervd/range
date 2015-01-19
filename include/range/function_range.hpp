/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef RANGE_FUNCTION_RANGE_HPP_INCLUDED
#define RANGE_FUNCTION_RANGE_HPP_INCLUDED

#include <type_traits>

#include "rime/always.hpp"

#include "core.hpp"

namespace range {

/**
The function must be move-constructible and move-assignable.
*/
template <class Function> class function_range {
public:
    typedef typename boost::mpl::if_ <std::is_function <Function>,
        typename std::add_pointer <Function>::type, Function>::type
        function_type;

    explicit function_range (Function const & function)
    : function_ (function) {}
    explicit function_range (Function && function)
    : function_ (std::forward <Function> (function)) {}

    function_range (function_range const &) = delete;
    function_range (function_range && other)
    : function_ (other.function_) {}

    function_range & operator = (function_range const &) = delete;
    function_range & operator = (function_range && other) {
        function_ = std::move (other.function_);
        return *this;
    }

    function_type & function() { return function_; }

private:
    function_type function_;

private:
    friend class operation::member_access;

    auto empty (direction::front) const RETURNS (rime::false_);

    auto chop_in_place (direction::front) RETURNS (function_());
};

struct function_range_tag;

template <class Function> struct tag_of_qualified <function_range <Function>>
{ typedef function_range_tag type; };

template <class Function> function_range <Function>
    make_function_range (Function const & function)
{ return function_range <Function> (function); }

namespace operation {

    // first and drop are defined automatically for rvalues.

    template <class Range>
        struct chop <function_range_tag, direction::front, Range &&>
    {
        template <class Function>
            auto operator() (
                direction::front, function_range <Function> && range) const
        -> chopped <decltype (range.function()()), function_range <Function>>
        {
            return chopped <decltype (range.function()()),
                function_range <Function>> (
                    range.function()(), std::move (range));
        }
    };

} // namespace operation

} // namespace range

#endif // RANGE_FUNCTION_RANGE_HPP_INCLUDED
