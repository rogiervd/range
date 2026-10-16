// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_FUNCTION_RANGE_HPP_INCLUDED
#define RANGE_FUNCTION_RANGE_HPP_INCLUDED

#include <type_traits>

#include "rime/always.hpp"

#include "core.hpp"

namespace range {

/**
The function must be move-constructible and move-assignable.
*/
template <class Function> class function_range
{
public:
    typedef typename boost::mpl::if_<
        std::is_function<Function>, typename std::add_pointer<Function>::type,
        Function>::type function_type;

    explicit function_range(Function const & function) : function_(function) {}
    explicit function_range(Function && function)
    : function_(std::forward<Function>(function))
    {}

    function_range(function_range const &) = delete;
    function_range(function_range && other) : function_(other.function_) {}

    function_range & operator=(function_range const &) = delete;
    function_range & operator=(function_range && other)
    {
        function_ = std::move(other.function_);
        return *this;
    }

    function_type & function() { return function_; }

private:
    function_type function_;

private:
    friend class helper::member_access;

    auto empty(direction::front) const RETURNS(rime::false_);

    auto chop_in_place(direction::front) RETURNS(function_());
};

namespace function_range_operation {

    struct function_range_tag {};

    template <class Function> inline auto implement_chop(
        function_range_tag const & tag, function_range<Function> && range,
        direction::front const & direction)
        RETURNS(helper::chop_by_chop_in_place(std::move(range), direction));

}  // namespace function_range_operation

template <class Function> struct tag_of_qualified<function_range<Function>>
{
    typedef function_range_operation::function_range_tag type;
};

/** \brief
Create a range whose elements are the results of consecutive function calls.

The resulting range implements operations on rvalues only: it is not possible
to make a copy of it.
The range is noncopyable, but movable.
*/
template <class Function>
function_range<Function> make_function_range(Function const & function)
{
    return function_range<Function>(function);
}

}  // namespace range

#endif  // RANGE_FUNCTION_RANGE_HPP_INCLUDED
