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
