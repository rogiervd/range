/*
Copyright 2014 Rogier van Dalen.

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

#ifndef CONTAINER_TEST_CONTAINER_TEST_HPP_INCLUDED
#define CONTAINER_TEST_CONTAINER_TEST_HPP_INCLUDED

#include <utility>
#include <string>

#include <boost/mpl/apply.hpp>

#include "range/for_each_macro.hpp"

#include "utility/test/test_allocator.hpp"
#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

namespace range { namespace test {

/**
Embody an enumeration of test cases that consist of a sequence of operations.
For example, it is possible to go through all possible sequences, or to pick
sequences at random.

Essentially, this generates a number with radixes which are different for
each position, and depend on the choices of the previous positions.
For example, it may count 000 001 002 01 020 021 030 031 032 033 034 035 100.

This class can generate the same sequence multiple times before switching to
the next sequence.
This is useful for checking exception safety: the same test can be run many
times, throwing at different places.
*/
class choices {
public:
    /**
    \return The operation to pick at this stage in the sequence, out of
    \a number possibilities.
    */
    virtual int operator() (int number) = 0;

    /**
    Restart the current sequence from the first position.
    */
    virtual void start() = 0;

    /**
    Go to the next sequence of operations.
    */
    virtual bool next() = 0;
};

/**
Test a container by running through sequences of operations.
Derive from this class, with template parameters indicating the container to
be tested and the reference container to compare against.
Subclass (within the class is easiest) the base classes construct, mutate,
and compare, providing operations to be tested.
Override the virtual methods get_construct_tests, get_mutate_tests, and
get_compare_tests.
Then call operator()() on this class to run through all or many sequences of
test cases.

For containers that share elements underneath, the make_memento class is
provided, which is a mutation that makes a copy of the current container, that
at the end of the sequence of operations must still be the same.

The container will be instantiated with a test object class and a checking
allocator.
The reference type will be instantiated with an integer element type and a
standard allocator, and it should not in general throw.

\tparam ContainerExpression
    A Boost.MPL metafunction that is passed an element type and an allocator
    type, and which returns the container type to be tested.
\tparam ReferenceExpression
    A Boost.MPL metafunction that is passed an element type and an allocator
    type, and which returns the reference container type.

\todo Optionally customise the throwing points.
This is only necessary to test operations that behave differently depending on
whether the object promises not to throw.
*/
template <class ContainerExpression, class ReferenceExpression>
    struct container_test
{
    typedef int basic_object;
    typedef std::allocator <basic_object> reference_allocator_type;
    typedef utility::throwing <utility::tracked <int>,
        true, true, true, true, true, true> test_object;
    typedef utility::test_allocator <std::allocator <test_object>>
        container_allocator_type;
    typedef typename boost::mpl::apply <ContainerExpression,
        test_object, container_allocator_type>::type container_type;
    typedef typename boost::mpl::apply <ReferenceExpression,
        basic_object, reference_allocator_type>::type reference_type;

    struct memento {
        container_type container;
        reference_type reference;

        memento (container_type const & container,
            reference_type const & reference)
        : container (container), reference (reference) {}
    };

    /**
    Produce a sequence of unique integers, either one at a time, or a
    \c std::vector<int> of the desired length at once.
    */
    class int_factory {
        int current_index;
    public:
        int_factory() : current_index (0) {}

        int_factory (int_factory const &) = delete;
        int_factory (int_factory &&) = default;

        int operator() () { return ++ current_index; }

        std::vector <int> operator() (int n) {
            std::vector <int> result;
            for (int i = 0; i != n; ++ i)
                result.push_back ((*this)());
            return std::move (result);
        }
    };

    /**
    Produce a unique test_object, or a \c std::vector of them at once.
    */
    class test_object_factory {
        int_factory underlying;
        utility::thrower & thrower;
        utility::tracked_registry & registry;
    public:
        test_object_factory (
            utility::thrower & thrower, utility::tracked_registry & registry)
        : thrower (thrower), registry (registry) {}

        test_object operator() () {
            return test_object (thrower, registry, underlying());
        }

        std::vector <test_object> operator() (int n) {
            std::vector <test_object> result;
            for (int i = 0; i != n; ++ i)
                result.push_back ((*this)());
            return std::move (result);
        }
    };

    /**
    Values to indicate the exception safety of an operation.
    */
    enum exception_safety {
        /// Do not throw any exceptions.
        no_exceptions,
        /// If an exception occurs, leave the container in an undefined but
        /// valid state.
        /// Leak no memory.
        basic,
        /// If an exception occurs, leave the container in the initial state.
        /// Leak no memory.
        strong
    };

    class operation {
    public:
        operation (std::string description) : description_ (description) {}

        std::string const & description() const { return description_; }
    private:
        std::string description_;
    };

    /**
    Operation that constructs a container.
    The overloaded operator(), one for the container type and one for the
    reference type, should perform the same operation, so the container and the
    reference can be compared for equality.
    */
    class construct : public operation {
    public:
        construct (std::string description) : operation (description) {}

        /**
        Return a newly constructed container_type object using the factory and
        the allocator.
        */
        virtual container_type operator() (
            test_object_factory &, container_allocator_type &) const = 0;

        /**
        Return a reference_type object using the factory and the allocator.
        */
        virtual reference_type operator() (
            int_factory &, reference_allocator_type &) const = 0;

        /**
        Return the exception specification.
        By default, this returns \c basic.
        */
        virtual exception_safety exception_specification() const
        { return basic; }
    };

    /**
    Operation that changes a container.
    */
    class mutate : public operation {
    public:
        mutate (std::string description) : operation (description) {}

        /**
        Mutate the container, using the factory and the allocator where useful.
        */
        virtual void operator() (
            container_type &, test_object_factory &, container_allocator_type &)
            const = 0;

        /**
        Mutate the reference, using the factory and the allocator where useful.
        */
        virtual void operator() (
            reference_type &, int_factory &, reference_allocator_type &)
        const = 0;

        /**
        Produce a memento, a copy of the container that should retain its value
        as the original container is added.
        The default implementation does nothing.
        You probably want to use the make_memento class to test copying
        functionality.
        */
        virtual void copy_memento (container_type &, reference_type &,
            std::vector <memento> & mementos) const {}

        /**
        Return the exception specification.
        By default, this returns \c strong.
        */
        virtual exception_safety exception_specification() const
        { return strong; }
    };

    /**
    Operation that compares a container and the reference.
    When implementing this, use test_object_value to extract a value from an
    element of the container of type basic_object, so that it can be compared to
    an element of the reference.
    */
    class compare : public operation {
    public:
        compare (std::string description) : operation (description) {}

        virtual void operator() (
            container_type const &, reference_type const &) const = 0;
    };

    virtual std::vector <std::shared_ptr <construct>>
        get_construct_tests() const = 0;
    virtual std::vector <std::shared_ptr <mutate>>
        get_mutate_tests() const = 0;
    virtual std::vector <std::shared_ptr <compare>>
        get_compare_tests() const = 0;

    class make_memento : public mutate {
    public:
        make_memento()
        : mutate ("Make memento of container that should not change") {}

        virtual void operator() (container_type &,
            test_object_factory &, container_allocator_type &) const {}

        virtual void operator() (reference_type &,
            int_factory &, reference_allocator_type &) const {}

        virtual void copy_memento (container_type & container,
            reference_type & reference, std::vector <memento> & mementos) const
        { mementos.push_back (memento (container, reference)); }
    };

    static basic_object test_object_value (test_object const & object)
    { return object.content().content(); }

private:
    /**
    \internal
    Test one test case, as indicated by the current setting of test_case.
    */
    struct test_one_case {
        int mutation_num;
        choices & test_case;
        std::vector <std::shared_ptr <construct>> const & construct_tests;
        std::vector <std::shared_ptr <mutate>> const & mutate_tests;
        std::vector <std::shared_ptr <compare>> const & compare_tests;

        void check_mementos (std::vector <memento> const & mementos) const {
            // std::cout << "Comparing mementos..." << std::endl;
            RANGE_FOR_EACH (memento, mementos) {
                RANGE_FOR_EACH (compare, compare_tests)
                    (*compare) (memento.container, memento.reference);
            }
        }

    public:
        test_one_case (int mutation_num, choices & test_case,
            std::vector <std::shared_ptr <construct>> const & construct_tests,
            std::vector <std::shared_ptr <mutate>> const & mutate_tests,
            std::vector <std::shared_ptr <compare>> const & compare_tests)
        : mutation_num (mutation_num), test_case (test_case),
            construct_tests (construct_tests), mutate_tests (mutate_tests),
            compare_tests (compare_tests) {}

        void operator() (utility::thrower & thrower) const {
            test_case.start();

            utility::tracked_registry registry;
            reference_allocator_type reference_allocator;
            container_allocator_type allocator (thrower, reference_allocator);
            test_object_factory container_examples (thrower, registry);
            int_factory reference_examples;

            auto const & construct
                = * construct_tests [test_case (construct_tests.size())];

            // std::cout << construct.description() << " -> ";

            std::vector <memento> mementos;

            bool exceptions_allowed =
                (construct.exception_specification() != no_exceptions);
            try {
                container_type container =
                    construct (container_examples, allocator);

                exceptions_allowed = false;

                reference_type reference =
                    construct (reference_examples, reference_allocator);

                RANGE_FOR_EACH (compare, compare_tests)
                    (*compare) (container, reference);

                for (int mutation_index = 0; mutation_index != mutation_num;
                    ++ mutation_index)
                {
                    auto const & mutate =
                        * mutate_tests [test_case (mutate_tests.size())];

                    // std::cout << mutate.description() << " -> ";

                    try {
                        mutate (container, container_examples, allocator);
                    } catch (...) {
                        // std::cout << "Exception! ";
                        switch (mutate.exception_specification()) {
                        case no_exceptions:
                            BOOST_CHECK (false);
                            break;
                        case basic:
                            // No requirements that can be checked here:
                            // No resources must be leaked.
                            break;
                        case strong:
                            // Contents must be the same as before the
                            // operation.
                            RANGE_FOR_EACH (compare, compare_tests)
                                (*compare) (container, reference);
                            break;
                        default:
                            assert (false);
                        }
                        // Don't trigger errors in the next exception handler.
                        exceptions_allowed = true;
                        throw;
                    }
                    mutate (reference, reference_examples, reference_allocator);

                    mutate.copy_memento (container, reference, mementos);

                    RANGE_FOR_EACH (compare, compare_tests)
                        (*compare) (container, reference);
                }
            } catch (...) {
                // std::cout << "Exception! ";
                BOOST_CHECK (exceptions_allowed);
                check_mementos (mementos);
                throw;
            }

            check_mementos (mementos);
            mementos.clear(); // Before the registry goes out of scope.
        }
    };

public:
    /**
    Perform the tests that \a test_case indicates.

    \param test_case
        The test cases to run.
        This will be called to choose a construction operation and
        \a mutation_num mutate operations.
        All compare operations will be used after each step.
    \param mutation_num
        The number of mutation operations to test for each test case.
    \param test_exceptions
        Whether to test all points where exceptions can be thrown.
        By default set to \c false.
    */
    void operator() (choices & test_case,
        int mutation_num = 1, bool test_exceptions = false) const
    {
        auto construct_tests = get_construct_tests();
        auto mutate_tests = get_mutate_tests();
        auto compare_tests = get_compare_tests();

        do {
            test_one_case t (mutation_num, test_case,
                construct_tests, mutate_tests, compare_tests);
            if (test_exceptions) {
                utility::check_all_throw_points (t);
            } else {
                utility::thrower thrower;
                t (thrower);
            }
        } while (test_case.next());
    }
};

/**
Embodies an enumeration of test cases that goes through all of them.
*/
class all_choices : public choices {
public:
    all_choices()
    : state(), next_index (-1), last_not_last (-1) {}

    virtual int operator() (int number) {
        // start() must have been called.
        assert (next_index != -1);
        assert (number != 0);
        int result;
        if (next_index < int (state.size())) {
            result = state [next_index];
        } else {
            result = 0;
            state.push_back (0);
        }
        if (result < number - 1)
            last_not_last = next_index;
        ++ next_index;
        return result;
    }

    virtual void start() {
        // std::cout << "Restart test." << std::endl;
        next_index = 0;
        last_not_last = -1;
    }

    virtual bool next() {
        // std::cout << "*** Finished test case ***" << std::endl;
        if (last_not_last != -1) {
            ++ state [last_not_last];
            state.erase (state.begin() + last_not_last + 1, state.end());
            next_index = -1;
            last_not_last = -1;
            return true;
        } else
            return false;
    }

private:
    std::vector <int> state;
    int next_index;
    int last_not_last;
};

}} // namespace range::test

#endif // CONTAINER_TEST_CONTAINER_TEST_HPP_INCLUDED
