#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

class SumAverageAccumulatorTest : public testing::Test {
protected:
    void SetUp() override {} 
    void TearDown() override {} 

    SumAverageAccumulator acc_{};
    const double tolerance = 0.0001;
};

TEST_F(SumAverageAccumulatorTest, AccumulateSingle) {  
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Finalize();

    const auto expected = SumAverageAccumulator::SumAverage{.sum = 10, .average = 10.0};
    const auto real = acc_.Get();

    ASSERT_EQ(expected.sum, real.sum);
    ASSERT_NEAR(expected.average, real.average, tolerance);
}

TEST_F(SumAverageAccumulatorTest, AccumulateMultiple) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Accumulate({.metric_name = "metric1", .value = 20});
    acc_.Accumulate({.metric_name = "metric1", .value = 30});
    acc_.Finalize();

    const auto expected = SumAverageAccumulator::SumAverage{.sum = 60, .average = 20.0};
    const auto real = acc_.Get();
    
    ASSERT_EQ(expected.sum, real.sum);
    ASSERT_NEAR(expected.average, real.average, tolerance);
}

TEST_F(SumAverageAccumulatorTest, GetBeforeFinalize) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    ASSERT_THROW(acc_.Get(), std::runtime_error);
}

TEST_F(SumAverageAccumulatorTest, Reset) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Finalize();
    acc_.Reset();
    
    acc_.Accumulate({.metric_name = "metric2", .value = 5});
    acc_.Finalize();

    const auto expected = SumAverageAccumulator::SumAverage{.sum = 5, .average = 5.0};
    const auto real = acc_.Get();

    ASSERT_EQ(expected.sum, real.sum);
    ASSERT_NEAR(expected.average, real.average, tolerance);
}

TEST_F(SumAverageAccumulatorTest, EmptyAccumulation) {
    acc_.Reset();
    acc_.Finalize();
    
    const auto expected = SumAverageAccumulator::SumAverage{.sum = 0, .average = 0.0};
    const auto real = acc_.Get();

    ASSERT_EQ(expected.sum, real.sum);
    ASSERT_NEAR(expected.average, real.average, tolerance);
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test