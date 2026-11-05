// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
 * \brief Simple benchmark to check iteration speed compared to a for-loop.
 */

#include <benchmark/benchmark_api.h>
#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>

#include <vector>

#include "range/fold.hpp"
#include "range/for_each.hpp"
#include "range/for_each_macro.hpp"
#include "range/std/vector.hpp"

class Iterating : public benchmark::Fixture
{
public:
    Iterating()
    {
        RangeMultiplier(8)->Range(1 << 6, 1 << 16)->Complexity(benchmark::oN);
    }

    using benchmark::Fixture::SetUp;

    void SetUp(benchmark::State & state) override
    {
        auto size = state.range(0);
        data.clear();
        for (int i = 0; i < size; ++i) {
            data.push_back(i);
        }
        state.SetComplexityN(size);
    }

protected:
    std::vector<int> data;
};

BENCHMARK_F(Iterating, For)(benchmark::State & state)
{
    for (auto _ : state) {
        int sum = 0;
        for (auto const & value : data) {
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
}

BENCHMARK_F(Iterating, RangeForEach)(benchmark::State & state)
{
    for (auto _ : state) {
        int sum = 0;
        range::for_each(data, [&](int value) { sum += value; });
        benchmark::DoNotOptimize(sum);
    }
}

BENCHMARK_F(Iterating, RangeForEachMacro)(benchmark::State & state)
{
    for (auto _ : state) {
        int sum = 0;
        RANGE_FOR_EACH(value, data) { sum += value; }
        benchmark::DoNotOptimize(sum);
    }
}

BENCHMARK_F(Iterating, RangeFold)(benchmark::State & state)
{
    for (auto _ : state) {
        int sum = range::fold(
            0, data, [](auto left, auto right) { return left + right; });
        benchmark::DoNotOptimize(sum);
    }
}
