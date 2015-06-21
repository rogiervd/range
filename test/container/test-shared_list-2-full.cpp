/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_range_shared_list_full
#include "utility/test/boost_unit_test.hpp"
#include "utility/test/boost_unit_test_command_line.hpp"

#include "container/shared_list.hpp"

#include "container_test.hpp"

#include <vector>
#include <list>
#include <ostream>
#include <iostream>

#include "range/std.hpp"

#include <boost/mpl/placeholders.hpp>

BOOST_AUTO_TEST_SUITE (test_suite_range_shared_list_full)

using boost::mpl::_1;
using boost::mpl::_2;

struct tests
: range::test::container_test <
    range::shared_list <_1, direction::front, _2>,
    std::list <_1, _2>>
{
    class default_construct : public construct {
    public:
        default_construct() : construct ("Default-construct") {}

        virtual container_type operator() (
            test_object_factory &, container_allocator_type & allocator) const
        { return container_type (allocator); }

        virtual reference_type operator() (
            int_factory &, reference_allocator_type & allocator) const
        { return reference_type (allocator); }
    };

    class construct_from_range : public construct {
        int example_num;
    public:
        construct_from_range (int example_num)
        : construct ("Construct from range"), example_num (example_num) {}

        virtual container_type operator() (
            test_object_factory & factory, container_allocator_type & allocator)
            const
        {
            std::vector <test_object> v;
            for (int i = 0; i != example_num; ++ i)
                v.push_back (factory());
            return container_type (v, allocator);
        }

        virtual reference_type operator() (
            int_factory & factory, reference_allocator_type & allocator) const
        {
            reference_type r (allocator);
            for (int i = 0; i != example_num; ++ i)
                r.push_back (factory());
            return std::move (r);
        }
    };

    class push : public mutate {
        bool with_direction;
    public:
        push (bool with_direction)
        : mutate ("l.push (front, element)"), with_direction (with_direction) {}

        virtual void operator() (
            container_type & container, test_object_factory & factory,
            container_allocator_type &) const
        {
            if (with_direction)
                container.push (range::front, factory());
            else
                container.push (factory());
        }

        virtual void operator() (
            reference_type & reference, int_factory & factory,
            reference_allocator_type &) const
        { reference.push_front (factory()); }
    };

    class push_range : public mutate {
        bool with_direction;
        int example_num;
    public:
        push_range (bool with_direction, int example_num)
        : mutate ("l.push_range (front, r)"),
            with_direction (with_direction), example_num (example_num) {}

        virtual void operator() (container_type & container,
            test_object_factory & factory, container_allocator_type &) const
        {
            std::vector <test_object> examples = factory (example_num);
            if (with_direction)
                container.push_range (range::front, examples);
            else
                container.push_range (examples);
        }

        virtual void operator() (reference_type & reference,
            int_factory & factory, reference_allocator_type &) const
        {
            std::vector <basic_object> examples = factory (example_num);
            reference.insert (reference.begin(),
                examples.begin(), examples.end());
        }
    };

    class pop : public mutate {
        bool with_direction;
    public:
        pop (bool with_direction)
        : mutate ("l.pop (front)"), with_direction (with_direction) {}

        virtual void operator() (container_type & container,
            test_object_factory &, container_allocator_type &) const
        {
            if (!range::empty (container)) {
                if (with_direction)
                    container.pop (range::front);
                else
                    container.pop();
            }
        }

        virtual void operator() (reference_type & reference,
            int_factory &, reference_allocator_type &) const
        {
            if (!reference.empty())
                reference.erase (reference.begin());
        }
    };

    /**
    Make a view, drop a number of elements from the front, and re-assign to the
    container.
    */
    class drop : public mutate {
        std::size_t number;
    public:
        drop (std::size_t number)
        : mutate ("drop (drop (drop (l)))"), number (number) {}

        virtual void operator() (container_type & container,
            test_object_factory &, container_allocator_type &) const
        {
            auto v = range::view (container);
            std::size_t n = 0;
            while (n != number && !range::empty (v)) {
                v = range::drop (v);
                ++ n;
            }
            container = v;
        }

        virtual void operator() (reference_type & reference,
            int_factory &, reference_allocator_type &) const
        {
            auto delete_end = reference.begin();
            std::size_t n = 0;
            while (n != number && delete_end != reference.end()) {
                ++ delete_end;
                ++ n;
            }

            reference.erase (reference.begin(), delete_end);
        }
    };

    class check_elements : public compare {
    public:
        check_elements() : compare ("Compare element-wise") {}

        virtual void operator() (container_type const & container,
            reference_type const & reference) const
        {
            auto container_view = range::view (container);
            auto reference_view = range::view (reference);

            while (! range::empty (reference_view)) {
                BOOST_CHECK (!range::empty (container_view));
                if (range::empty (container_view))
                    return;
                BOOST_CHECK_EQUAL (
                    test_object_value (range::first (container_view)),
                    range::first (reference_view));
                container_view = range::drop (container_view);
                reference_view = range::drop (reference_view);
            }
        }
    };

    class print_elements : public compare {
    public:
        print_elements() : compare ("Print all elements") {}

        virtual void operator() (container_type const & container,
            reference_type const & reference) const
        {
            std::cout << "Contents: [ ";
            RANGE_FOR_EACH (element, container)
                std::cout << element.content().content() << " ";
            std::cout << "]\n";
        }
    };

    virtual std::vector <std::shared_ptr <construct>>
        get_construct_tests() const
    {
        std::vector <std::shared_ptr <construct>> tests;

        tests.push_back (std::make_shared <default_construct>());
        tests.push_back (std::make_shared <construct_from_range> (0));
        tests.push_back (std::make_shared <construct_from_range> (1));
        tests.push_back (std::make_shared <construct_from_range> (3));
        tests.push_back (std::make_shared <construct_from_range> (7));

        return std::move (tests);
    }

    virtual std::vector <std::shared_ptr <mutate>> get_mutate_tests() const {
        std::vector <std::shared_ptr <mutate>> tests;

        tests.push_back (std::make_shared <push> (false));
        tests.push_back (std::make_shared <push> (true));
        tests.push_back (std::make_shared <push_range> (false, 0));
        tests.push_back (std::make_shared <push_range> (true, 1));
        tests.push_back (std::make_shared <push_range> (false, 3));

        tests.push_back (std::make_shared <make_memento>());

        tests.push_back (std::make_shared <pop> (true));
        tests.push_back (std::make_shared <pop> (false));

        tests.push_back (std::make_shared <drop> (0));
        tests.push_back (std::make_shared <drop> (1));
        tests.push_back (std::make_shared <drop> (4));

        return std::move (tests);
    }

    virtual std::vector <std::shared_ptr <compare>> get_compare_tests() const {
        std::vector <std::shared_ptr <compare>> tests;

        tests.push_back (std::make_shared <check_elements>());

        // tests.push_back (std::make_shared <print_elements>());

        return std::move (tests);
    }
};

BOOST_AUTO_TEST_CASE (test_range_container_type_all) {
    tests t;
    range::test::all_choices c;
    size_t mutation_num = command_line_contains ("--full") ? 4 : 3;
    t (c, mutation_num, false);
}

BOOST_AUTO_TEST_CASE (test_range_container_type_exception_safety) {
    tests t;
    range::test::all_choices c;
    size_t mutation_num = command_line_contains ("--full") ? 3 : 2;
    t (c, mutation_num, true);
}

BOOST_AUTO_TEST_SUITE_END()
