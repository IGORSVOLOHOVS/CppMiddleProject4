#include "metric_impl/parameters_count.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

    // здесь ваш код
    MetricResult::ValueType CountParametersMetric::CalculateImpl(const function::Function& f) const{        
        auto res = f.ast | std::views::split('\n') | 
        std::views::transform([](auto&& r){
            return std::string_view{r};
        }) | std::views::take_while([](auto&& sv){
            return !sv.contains("body");
        }) | std::views::filter([](auto&& sv){
            return sv.contains("identifier");
        }) | std::ranges::to<std::vector>();

        return static_cast<int>(res.size() - 1); // identifier like function name
    }

    std::string CountParametersMetric::Name() const {
        return "parameters_count";
    }

}  // namespace analyser::metric::metric_impl
