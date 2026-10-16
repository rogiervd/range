// SPDX-FileCopyrightText: Copyright 2011-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED
#define RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED

#include <type_traits>

#include "utility/returns.hpp"

#include "../direction.hpp"
#include "core_base.hpp"
#include "core_member_access.hpp"
#include "core_tag.hpp"

namespace range {

namespace helper {

    inline void implement_default_direction(unusable);

}  // namespace helper

namespace callable {

    namespace implementation {

        using helper::implement_default_direction;

        struct default_direction
        {
        private:
            struct dispatch
            {
                template <class Range>
                auto operator()(Range const & range, overload_order<1> *) const
                    RETURNS(implement_default_direction(
                        typename tag_of<Range>::type(), range));

                template <class Range>
                auto operator()(Range const & range, overload_order<2> *) const
                    RETURNS(helper::member_access::default_direction(range));

                // If no function or member is defined, return range::front.
                template <class Range> direction::front operator()(
                    Range const & range, overload_order<16> *) const
                {
                    return range::front;
                }
            };

        public:
            template <
                class Range,
                class Enable =
                    typename std::enable_if<is_range<Range>::value>::type>
            auto operator()(Range const & range) const
                RETURNS(dispatch()(range, pick_overload()));
        };

    }  // namespace implementation

    using implementation::default_direction;

}  // namespace callable

/** \brief
Return the default direction of a range.

This is the direction that is used for operations if none is given explicitly.
For many ranges, this returns \c range::front.

\param range
    The range to return the default direction of.
*/
static auto const default_direction = callable::default_direction();

}  // namespace range

#endif  // RANGE_DETAIL_CORE_DEFAULT_DIRECTION_HPP_INCLUDED
