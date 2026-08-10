#include "metric_impl/cyclomatic_complexity.hpp"

#if __has_include(<flat_set>)  // C++23, still missing from MSVC 19.44 STL
#include <flat_set>
#endif
#include <numeric>
#include <string_view>
#if __has_include(<unistd.h>)  // POSIX-only header, MSVC does not ship it
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

using namespace std::string_view_literals;

namespace analyser::metric::metric_impl {

    MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function& f) const{
        static const std::unordered_set STATEMENTS = {
            "if_statement"sv,
            "elif_clause"sv,
            "while_statement"sv,
            "for_statement"sv,
            "try_statement"sv,
            "except_clause"sv,
            "finally_clause"sv,
            "match_statement"sv,
            "case_pattern"sv,
            "assert_statement"sv,
            "conditional_expression"sv
        };
        
        auto res = f.ast | std::views::split('(') | std::views::drop(1) | std::views::transform([](auto&& r){
            auto sv = std::string_view{r};
            return sv.substr(0, sv.find_first_of(" :"));
        }) | std::views::filter([](auto&& sv){
            return STATEMENTS.contains(std::string{sv});
        }) | std::ranges::to<std::vector>();

        return static_cast<int>(res.size());
    }

    std::string CyclomaticComplexityMetric::Name() const {
        return "cyclomatic_complexity";
    }

}  // namespace analyser::metric::metric_impl
