// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "benchmark/benchmark.h"

#include <vector>

#include "arrow/compute/api_scalar.h"
#include "arrow/compute/kernels/test_util.h"
#include "arrow/testing/gtest_util.h"
#include "arrow/testing/random.h"
#include "arrow/util/benchmark_util.h"

namespace arrow {
namespace compute {

// Use a fixed hash to ensure consistent results from run to run.
constexpr auto kSeed = 0x94378165;

template <typename ArrowType, RoundMode Mode, typename CType = typename ArrowType::c_type>
static void RoundArrayBenchmark(benchmark::State& state, const std::string& func_name) {
  RegressionArgs args(state);

  const int64_t array_size = args.size / sizeof(CType);
  auto rand = random::RandomArrayGenerator(kSeed);

  // Choose values so as to avoid overflow on all ops and types.
  auto min = static_cast<CType>(6);
  auto max = static_cast<CType>(min + 15);
  auto val = std::static_pointer_cast<NumericArray<ArrowType>>(
      rand.Numeric<ArrowType>(array_size, min, max, args.null_proportion));
  RoundOptions options;
  options.round_mode = static_cast<RoundMode>(Mode);

  for (auto _ : state) {
    ABORT_NOT_OK(CallFunction(func_name, {val}, &options));
  }
  state.SetItemsProcessed(state.iterations() * array_size);
}

template <typename ArrowType, RoundMode Mode, typename CType = typename ArrowType::c_type>
static void RoundBinaryArrayBenchmark(benchmark::State& state,
                                      const std::string& func_name) {
  RegressionArgs args(state);

  const int64_t array_size = args.size / sizeof(CType);
  auto rand = random::RandomArrayGenerator(kSeed);

  // Choose values so as to avoid overflow on all ops and types.
  auto min = static_cast<CType>(6);
  auto max = static_cast<CType>(min + 15);
  auto val = std::static_pointer_cast<NumericArray<ArrowType>>(
      rand.Numeric<ArrowType>(array_size, min, max, args.null_proportion));

  auto minNDigits = static_cast<int32_t>(-6);
  auto maxNDigits = static_cast<int32_t>(6);
  auto valNDigits = std::static_pointer_cast<NumericArray<Int32Type>>(
      rand.Numeric<Int32Type>(array_size, minNDigits, maxNDigits, args.null_proportion));

  RoundBinaryOptions options;
  options.round_mode = static_cast<RoundMode>(Mode);

  for (auto _ : state) {
    ABORT_NOT_OK(CallFunction(func_name, {val, valNDigits}, &options));
  }
  state.SetItemsProcessed(state.iterations() * array_size);
}

void SetRoundArgs(benchmark::internal::Benchmark* bench) {
  bench->ArgNames({"size", "inverse_null_proportion"});

  for (const auto inverse_null_proportion : std::vector<ArgsType>({100, 0})) {
    bench->Args({static_cast<ArgsType>(kL2Size), inverse_null_proportion});
  }
}

void SetRoundBinaryArgs(benchmark::internal::Benchmark* bench) {
  bench->ArgNames({"size", "inverse_null_proportion"});

  for (const auto inverse_null_proportion : std::vector<ArgsType>({100, 0})) {
    bench->Args({static_cast<ArgsType>(kL2Size), inverse_null_proportion});
  }
}

template <typename ArrowType, RoundMode Mode>
static void Ceil(benchmark::State& state) {
  RoundArrayBenchmark<ArrowType, Mode>(state, "ceil");
}

template <typename ArrowType, RoundMode Mode>
static void Floor(benchmark::State& state) {
  RoundArrayBenchmark<ArrowType, Mode>(state, "floor");
}

template <typename ArrowType, RoundMode Mode>
static void Round(benchmark::State& state) {
  RoundArrayBenchmark<ArrowType, Mode>(state, "round");
}

template <typename ArrowType, RoundMode Mode>
static void Trunc(benchmark::State& state) {
  RoundArrayBenchmark<ArrowType, Mode>(state, "trunc");
}

template <typename ArrowType, RoundMode Mode>
static void RoundBinary(benchmark::State& state) {
  RoundBinaryArrayBenchmark<ArrowType, Mode>(state, "round_binary");
}

#ifdef ALL_ROUND_BENCHMARKS
#define DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, TYPE, SETARGS)                \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::DOWN)->Apply(SETARGS);                  \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::UP)->Apply(SETARGS);                    \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::TOWARDS_ZERO)->Apply(SETARGS);          \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::TOWARDS_INFINITY)->Apply(SETARGS);      \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_DOWN)->Apply(SETARGS);             \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_UP)->Apply(SETARGS);               \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_TOWARDS_ZERO)->Apply(SETARGS);     \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_TOWARDS_INFINITY)->Apply(SETARGS); \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_TO_EVEN)->Apply(SETARGS);          \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_TO_ODD)->Apply(SETARGS)
#else
#define DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, TYPE, SETARGS)            \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::DOWN)->Apply(SETARGS);              \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_TOWARDS_ZERO)->Apply(SETARGS); \
  BENCHMARK_TEMPLATE(OP, TYPE, RoundMode::HALF_TO_ODD)->Apply(SETARGS)
#endif

#define DECLARE_ROUND_BENCHMARKS(OP, SETARGS)                       \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, Int64Type, SETARGS);  \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, Int32Type, SETARGS);  \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, Int16Type, SETARGS);  \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, Int8Type, SETARGS);   \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, UInt64Type, SETARGS); \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, UInt32Type, SETARGS); \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, UInt16Type, SETARGS); \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, UInt8Type, SETARGS);  \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, FloatType, SETARGS);  \
  DECLARE_ROUND_BENCHMARKS_WITH_ROUNDMODE(OP, DoubleType, SETARGS);

DECLARE_ROUND_BENCHMARKS(Ceil, SetRoundArgs);
DECLARE_ROUND_BENCHMARKS(Floor, SetRoundArgs);
DECLARE_ROUND_BENCHMARKS(Round, SetRoundArgs);
DECLARE_ROUND_BENCHMARKS(Trunc, SetRoundArgs);

DECLARE_ROUND_BENCHMARKS(RoundBinary, SetRoundBinaryArgs);

}  // namespace compute
}  // namespace arrow
