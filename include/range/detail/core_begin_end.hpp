// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RANGE_DETAIL_CORE_BEGIN_END_INCLUDED
#define RANGE_DETAIL_CORE_BEGIN_END_INCLUDED

#include <variant>  // For std::monostate.

#include "core_base.hpp"
#include "core_empty.hpp"
#include "core_is_homogeneous.hpp"
#include "core_view.hpp"

namespace range {

namespace helper {

    /** \brief
    Return an iterator pointing to the first element of the range, counting from
    the front.

    This enables some compatibility with standard C++ ranges, and "range-based
    for loops". This can only be used for homogeneous ranges.

    If there is a genuine iterator, then it makes sense to return that.
    If there is not, then begin() can return a thin wrapper around the range.
    The default implementation probably works.

    \param tag The range tag.
    \param range The range.
    */
    void implement_begin(unusable);

    /** \brief
    Return an iterator pointing to the past-the-end position in the range,
    counting from the front.

    This enables some compatibility with standard C++ ranges, and "range-based
    for loops". This can only be used for homogeneous ranges.

    The end iterator can be std::monostate, for the case when begin() returns a
    thin wrapper around the range..

    \param tag The range tag.
    \param range The range.
    */
    void implement_end(unusable);

}  // namespace helper

namespace begin_end_detail {

    using helper::implement_begin;
    using helper::implement_end;

    /** \brief
     * Adaptor that makes standard C++ "range-based" iteration possible, for
     * example for a range-based for looop.
     *
     * This requires a range to define "first" and "drop" in the "front"
     * direction.
     * Even if the range is more sophisticated, this adaptor does not provide
     * access to that sophistication.
     *
     * This is the begin iterator, which contains the information.
     * The end iterator, is a "monostate", i.e. just a sentinel.
     */
    template <class View> class iterable_adaptor
    {
    public:
        static_assert(is_view<View>::value);

        iterable_adaptor(View const & range_view) : view_(range_view) {}

        auto operator*() const { return ::range::first(view_, front); }

        iterable_adaptor & operator++()
        {
            view_ = ::range::drop(view_, front);
            return *this;
        }

        bool operator==(std::monostate) const
        {
            return ::range::empty(view_, front);
        }

        bool operator!=(std::monostate) const
        {
            return !::range::empty(view_, front);
        }

    private:
        View view_;
    };

}  // namespace begin_end_detail


namespace callable {

    namespace implementation {
        struct begin
        {
            struct use_specialised_implementation
            {
                // Forward to "implement_begin".
                template <class Range>
                auto operator()(Range & range, overload_order<0> *) const
                    RETURNS(
                        implement_begin(typename tag_of<Range>::type(), range));

                // Forward to member if possible.
                template <class Range>
                auto operator()(Range & range, overload_order<1> *) const
                    RETURNS(::range::helper::member_access::begin(range));
            };

            struct forward_plain_or_view
            {
                // First, forward the range to "use_specialised_implementation".

                template <class Range>
                auto operator()(Range & range, overload_order<0> *) const
                    RETURNS(
                        use_specialised_implementation()(
                            range, utility::pick_overload()));

                // Next, make a view and forward it to
                // "use_specialised_implementation".
                template <class Range>
                auto operator()(Range & range, overload_order<1> *) const
                    RETURNS(
                        use_specialised_implementation()(
                            ::range::view(range, ::range::front),
                            utility::pick_overload()));
            };

            /// \brief
            /// Use a specialisation, or failing that, the iterable_adaptor to
            /// make a begin iterator.
            struct dispatch
            {
                template <class Range>
                auto operator()(Range & range, overload_order<0> *) const
                    RETURNS(
                        forward_plain_or_view()(
                            range, utility::pick_overload()));

                // Use iterable_adaptor.
                template <
                    class Range,
                    class Enable =
                        std::enable_if_t<is_homogeneous<Range>::value>>
                auto operator()(Range & range, overload_order<1> *) const
                    RETURNS(
                        ::range::begin_end_detail::iterable_adaptor(
                            ::range::view(range, ::range::front)));
            };

            template <class Range> auto operator()(Range & range) const
                RETURNS(dispatch()(range, utility::pick_overload()));
        };

        struct end
        {
            struct use_specialised_implementation
            {
                // Forward to "implement_end".
                template <class Range>
                auto operator()(Range & range, overload_order<0> *) const
                    RETURNS(implement_end(range, utility::pick_overload()));

                // Forward to member if possible.
                template <class Range>
                auto operator()(Range & range, overload_order<1> *) const
                    RETURNS(::range::helper::member_access::end(range));
            };

            struct forward_plain_or_view
            {
                // First, forward the range to "use_specialised_implementation".

                template <class Range>
                auto operator()(Range & range, overload_order<0> *) const
                    RETURNS(
                        use_specialised_implementation()(
                            range, utility::pick_overload()));

                // Next, make a view and forward it to
                // "use_specialised_implementation".
                template <class Range>
                auto operator()(Range & range, overload_order<1> *) const
                    RETURNS(
                        use_specialised_implementation()(
                            ::range::view(range, ::range::front),
                            utility::pick_overload()));
            };

            /// \brief
            /// Use a specialisation, or failing that, begin() should be an
            /// iterable_adaptor, so end() should be std::monostate.
            struct dispatch
            {
                template <class Range>
                auto operator()(Range & range, overload_order<0> *) const
                    RETURNS(
                        forward_plain_or_view()(
                            range, utility::pick_overload()));

                // Use iterable_adaptor.
                template <
                    class Range,
                    class Enable =
                        std::enable_if_t<is_homogeneous<Range>::value>>
                auto operator()(Range &, overload_order<1> *) const
                {
                    return std::monostate {};
                }
            };

            template <class Range> auto operator()(Range & range) const
                RETURNS(dispatch()(range, utility::pick_overload()));
        };

    }  // namespace implementation

    using implementation::begin;
    using implementation::end;

}  // namespace callable


/** \brief
Return an iterator pointing to the first element of the range, counting from the
front.

This provides compatibility with standard C++ ranges, and "range-based for
loops". This only works for some homogeneous ranges. begin() and end() may be
the same type, or they may be different types.

\param range
    The range to iterate over.
    This range must be copyable, because end() needs to be called on the same
range.
*/
template <class Range> inline auto begin(Range & range)
{
    return callable::begin()(range);
}


/** \brief
Return an iterator pointing to past-the-end of the range, counting from the
front.

This provides compatibility with standard C++ ranges, and "range-based for
loops". This only works for some homogeneous ranges. begin() and end() may be
the same type, or they may be different types.

\param range
    The range to iterate over.
    This range must be copyable, because begin() needs to be called on the same
range.
*/
template <class Range> inline auto end(Range & range)
{
    return callable::end()(range);
}

}  // namespace range

#endif  // RANGE_DETAIL_CORE_BEGIN_END_INCLUDED
