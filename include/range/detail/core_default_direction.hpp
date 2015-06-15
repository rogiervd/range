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

#ifndef RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED
#define RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "core_base.hpp"

namespace range {

namespace helper {

    inline void implement_default_direction (unusable);

} // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_default_direction;

        struct default_direction {
        private:
            struct dispatch {
                template <class Range>
                    auto operator() (Range const & range, overload_order <1> *)
                    const
                RETURNS (implement_default_direction (
                    typename tag_of <Range>::type(), range));

                template <class Range>
                    auto operator() (Range const & range, overload_order <2> *)
                    const
                RETURNS (helper::member_access
                    ::default_direction (range));

                // If no function or member is defined, return range::front.
                template <class Range>
                    direction::front operator() (
                        Range const & range, overload_order <16> *) const
                { return range::front; }
            };

        public:
            template <class Range, class Enable =
                typename std::enable_if <is_range <Range>::value>::type>
            auto operator() (Range const & range) const
            RETURNS (dispatch() (range, pick_overload()));
        };

    } // namespace implementation

    using implementation::default_direction;

} // namespace callable

/** \brief
Return the default direction of a range.

This is the direction that is used for operations if none is given explicitly.
For many ranges, this returns \c range::front.

\param range
    The range to return the default direction of.
*/
static const auto default_direction = callable::default_direction();

} // namespace range

#endif  // RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED
