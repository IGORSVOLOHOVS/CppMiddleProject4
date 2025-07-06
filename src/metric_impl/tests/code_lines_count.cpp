#include "metric_impl/code_lines_count.hpp"
#include "file.hpp"
#include "function.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

class CodeLinesCountMetricTest : public testing::Test {
protected:
    function::FunctionExtractor extr_{};
    CodeLinesCountMetric m_{};

    auto GetFunction(const std::string& file_name) {
        file::File file(file_name);
        auto functions = extr_.Get(file);
        return functions.at(0);
    }
};

TEST_F(CodeLinesCountMetricTest, Name) { 
    ASSERT_EQ(m_.Name(), "code_lines_count");
}

TEST_F(CodeLinesCountMetricTest, CalculateForSimple) { 
    const auto func = GetFunction("simple.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 6);
} 

TEST_F(CodeLinesCountMetricTest, CalculateForComments) { 
    const auto func = GetFunction("comments.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 4);
} 

TEST_F(CodeLinesCountMetricTest, CalculateForManyLines) { 
    const auto func = GetFunction("many_lines.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 11);
} 

TEST_F(CodeLinesCountMetricTest, CalculateForIf) { 
    const auto func = GetFunction("if.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 4);
}

TEST_F(CodeLinesCountMetricTest, CalculateForLoops) { 
    const auto func = GetFunction("loops.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 7);
}

TEST_F(CodeLinesCountMetricTest, CalculateForExceptions) { 
    const auto func = GetFunction("exceptions.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 8);
}

TEST_F(CodeLinesCountMetricTest, CalculateForMatchCase) { 
    const auto func = GetFunction("match_case.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 8);
}

TEST_F(CodeLinesCountMetricTest, CalculateForNestedIf) { 
    const auto func = GetFunction("nested_if.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 9);
}

TEST_F(CodeLinesCountMetricTest, CalculateForManyParameters) { 
    const auto func = GetFunction("many_parameters.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 2);
}

TEST_F(CodeLinesCountMetricTest, CalculateForTernary) { 
    const auto func = GetFunction("ternary.py");
    ASSERT_EQ(std::get<int>(m_.CalculateImpl(func)), 2);
}

}  // namespace analyser::metric::metric_impl