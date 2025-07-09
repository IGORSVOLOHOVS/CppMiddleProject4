#include "metric_impl/parameters_count.hpp"
#include "file.hpp"
#include "function.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

class CountParametersMetricTest : public testing::Test {
protected:
    function::FunctionExtractor extr_{};
    CountParametersMetric m_{};

    auto GetFunction(const std::string& file_name) {
        file::File file(file_name);
        auto functions = extr_.Get(file);
        return functions.at(0);
    }
};

TEST_F(CountParametersMetricTest, Name) { 
    ASSERT_EQ(m_.Name(), "parameters_count");
}

TEST_F(CountParametersMetricTest, CalculateForSimple) { 
    const auto func = GetFunction("simple.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 0);
} 

TEST_F(CountParametersMetricTest, CalculateForComments) { 
    const auto func = GetFunction("comments.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 3);
} 

TEST_F(CountParametersMetricTest, CalculateForManyLines) { 
    const auto func = GetFunction("many_lines.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 0);
} 

TEST_F(CountParametersMetricTest, CalculateForIf) { 
    const auto func = GetFunction("if.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1);
}

TEST_F(CountParametersMetricTest, CalculateForLoops) { 
    const auto func = GetFunction("loops.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1);
}

TEST_F(CountParametersMetricTest, CalculateForExceptions) { 
    const auto func = GetFunction("exceptions.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 0);
}

TEST_F(CountParametersMetricTest, CalculateForMatchCase) { 
    const auto func = GetFunction("match_case.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1);
}

TEST_F(CountParametersMetricTest, CalculateForNestedIf) { 
    const auto func = GetFunction("nested_if.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 2);
}

TEST_F(CountParametersMetricTest, CalculateForManyParameters) { 
    const auto func = GetFunction("many_parameters.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 5);
}

TEST_F(CountParametersMetricTest, CalculateForTernary) { 
    const auto func = GetFunction("ternary.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1);
}

}  // namespace analyser::metric::metric_impl