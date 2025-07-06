#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

class CategoricalAccumulatorTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    CategoricalAccumulator acc_{};
};

TEST_F(CategoricalAccumulatorTest, AccumulateSingle) {
    acc_.Accumulate({.metric_name = "metric1", .value = 5});
    acc_.Finalize();

    const auto result = acc_.Get();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at("5"), 1);
}

TEST_F(CategoricalAccumulatorTest, AccumulateMultipleSameValue) {
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Finalize();

    const auto result = acc_.Get();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at("10"), 3);
}

TEST_F(CategoricalAccumulatorTest, AccumulateMultipleDifferentValues) {
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Accumulate({.metric_name = "metric1", .value = 20});
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Finalize();

    const auto result = acc_.Get();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at("10"), 2);
    ASSERT_EQ(result.at("20"), 1);
}

TEST_F(CategoricalAccumulatorTest, GetBeforeFinalize) {
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    ASSERT_THROW(acc_.Get(), std::runtime_error);
}

TEST_F(CategoricalAccumulatorTest, Reset) {
    acc_.Accumulate({.metric_name = "metric1", .value = 10});
    acc_.Finalize();
    acc_.Reset();

    acc_.Accumulate({.metric_name = "metric2", .value = 5});
    acc_.Finalize();

    const auto result = acc_.Get();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at("5"), 1);
    ASSERT_TRUE(result.find("10") == result.end());
}

TEST_F(CategoricalAccumulatorTest, EmptyAccumulation) {
    acc_.Finalize();
    
    const auto result = acc_.Get();
    ASSERT_TRUE(result.empty());
}

} // namespace analyser::metric_accumulator::metric_accumulator_impl::test