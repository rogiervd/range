/*
Copyright 2011-2015 Rogier van Dalen.

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
This file defines a skeleton and helper classes to define range operations.
The other core_*.hpp in this directory give examples of how to define the
innards of such operations.
The operation "default_direction" is needed by this file, so it is defined here
too.
*/

#ifndef RANGE_DETAIL_CORE_BASE_HPP_INCLUDED
#define RANGE_DETAIL_CORE_BASE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/transform.hpp"

#include "rime/core.hpp"

#include "utility/returns.hpp"
#include "utility/nested_callable.hpp"

#include "callable_traits.hpp"
#include "../direction.hpp"

#include "core_tag.hpp"

namespace range {

using utility::overload_order;
using utility::pick_overload;

using ::callable_traits::has;
using ::callable_traits::result_of;
using ::callable_traits::decayed_result_of;

/** \brief
Contain names for operations on ranges, and helpers.

Real implementations must be found through argument-dependent lookup.
*/
namespace helper {

    struct unusable;

    // Forward declarations for operations so they can refer to each other.
    void implement_default_direction (unusable);
    void implement_empty (unusable);
    void implement_size (unusable);
    void implement_first (unusable);
    void implement_drop (unusable);
    void implement_chop (unusable);
    void implement_chop_in_place (unusable);
    void implement_at (unusable);

    /** \brief
    Class that is convertible from direction::front and direction::back.
    */
    struct front_or_back {
        front_or_back() {}

        front_or_back (direction::front) {}
        front_or_back (direction::back) {}
    };

} // namespace helper

namespace callable { namespace implementation {

    // Forward declarations
    struct default_direction;
    struct empty;
    struct first;
    struct size;
    struct drop;
    struct chop;
    struct chop_in_place;

}} // namespace callable::implementation

static const direction::front front = {};
static const direction::back back = {};

} // namespace range

#endif  // RANGE_DETAIL_CORE_BASE_HPP_INCLUDED
