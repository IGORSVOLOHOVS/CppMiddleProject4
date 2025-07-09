#include "metric_impl/naming_style.hpp"
#include "file.hpp"
#include "function.hpp"

#include <gtest/gtest.h>


namespace analyser::metric::metric_impl {

class NamingStyleMetricTest : public testing::Test {
protected:
    NamingStyleMetric m_{};
    function::Function func_info_{};

    void CheckStyle(const std::string& function_name, const std::string& expected_style) {
        func_info_.name = function_name;
        const auto result = m_.CalculateImpl(func_info_);
        ASSERT_EQ(std::get<std::string>(result), expected_style);
    }
};

TEST_F(NamingStyleMetricTest, HandlesPascalCase) {
    CheckStyle("MyClass", "PascalCase");
    CheckStyle("Function1", "PascalCase");
    CheckStyle("A", "PascalCase");
}

TEST_F(NamingStyleMetricTest, HandlesCamelCase) {
    CheckStyle("myVariable", "camelCase");
    CheckStyle("variable2Name", "camelCase");
}

TEST_F(NamingStyleMetricTest, HandlesSnakeCase) {
    CheckStyle("my_function_name", "snake_case");
}

TEST_F(NamingStyleMetricTest, HandlesLowercase) {
    CheckStyle("anotherfunction", "lowercase");
    CheckStyle("a", "lowercase");
}

TEST_F(NamingStyleMetricTest, HandlesUnknown) {
    CheckStyle("ALL_CAPS", "unknown");
    CheckStyle("my_Mixed_Case", "unknown");
    CheckStyle("", "unknown");
}

TEST(NamingStyleMetricSimpleTest, Name) {
    NamingStyleMetric m;
    ASSERT_EQ(m.Name(), "naming_style");
}

} // namespace analyser::metric::metric_impl
