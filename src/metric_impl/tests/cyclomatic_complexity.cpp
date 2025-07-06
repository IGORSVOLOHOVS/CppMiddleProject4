#include "metric_impl/cyclomatic_complexity.hpp"
#include "file.hpp"
#include "function.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

class CyclomaticComplexityMetricTest : public testing::Test {
protected:
    function::FunctionExtractor extr_{};
    CyclomaticComplexityMetric m_{};

    auto GetFunction(const std::string& file_name) {
        file::File file(file_name);
        auto functions = extr_.Get(file);
        return functions.at(0);
    }
};

TEST_F(CyclomaticComplexityMetricTest, Name) { 
    ASSERT_EQ(m_.Name(), "cyclomatic_complexity");
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForSimple) { 
    const auto func = GetFunction("simple.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1);
} 

TEST_F(CyclomaticComplexityMetricTest, CalculateForComments) { 
    const auto func = GetFunction("comments.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 0);
} 

TEST_F(CyclomaticComplexityMetricTest, CalculateForManyLines) { 
    const auto func = GetFunction("many_lines.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1); 
} 

TEST_F(CyclomaticComplexityMetricTest, CalculateForIf) { 
    const auto func = GetFunction("if.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1);
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForLoops) { 
    const auto func = GetFunction("loops.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 3); 
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForExceptions) { 
    const auto func = GetFunction("exceptions.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 4); 
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForMatchCase) { 
    const auto func = GetFunction("match_case.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 4); 
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForNestedIf) { 
    const auto func = GetFunction("nested_if.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 4); 
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForManyParameters) { 
    const auto func = GetFunction("many_parameters.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 1); 
}

TEST_F(CyclomaticComplexityMetricTest, CalculateForTernary) { 
    const auto func = GetFunction("ternary.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 2); 
}

}  // namespace analyser::metric::metric_impl