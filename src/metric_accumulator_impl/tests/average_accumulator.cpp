#include "metric_accumulator_impl/average_accumulator.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

class AverageAccumulatorTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    AverageAccumulator acc_{};
    const double tolerance = 0.0001;
};

TEST_F(AverageAccumulatorTest, AccumulateSingle) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 15});
    acc_.Finalize();

    ASSERT_NEAR(acc_.Get(), 15.0, tolerance);
}

TEST_F(AverageAccumulatorTest, AccumulateMultiple) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Accumulate({.metric_name = "metric1", .value = 20});
    acc_.Accumulate({.metric_name = "metric1", .value = 3});
    acc_.Finalize();

    ASSERT_NEAR(acc_.Get(), 11.0, tolerance); // (10+20+3)/3
}

TEST_F(AverageAccumulatorTest, GetBeforeFinalize) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    ASSERT_THROW(acc_.Get(), std::runtime_error);
}

TEST_F(AverageAccumulatorTest, Reset) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 100});
    acc_.Finalize();
    acc_.Reset();

    acc_.Accumulate({.metric_name = "metric2", .value = 20});
    acc_.Finalize();
    
    ASSERT_NEAR(acc_.Get(), 20.0, tolerance);
}

TEST_F(AverageAccumulatorTest, EmptyAccumulation) {
    acc_.Reset();
    acc_.Finalize();
    
    ASSERT_NEAR(acc_.Get(), 0.0, tolerance);
}

TEST_F(AverageAccumulatorTest, AccumulateAfterFinalizeResets) {
    acc_.Reset();
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Accumulate({.metric_name = "metric1", .value = 20});
    acc_.Finalize();
    
    acc_.Accumulate({.metric_name = "metric2", .value = 30});
    acc_.Finalize();

    ASSERT_NEAR(acc_.Get(), 30.0, tolerance);
}

} // namespace analyser::metric_accumulator::metric_accumulator_impl::test