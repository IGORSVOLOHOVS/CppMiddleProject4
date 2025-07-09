#include "metric_impl/code_lines_count.hpp"

#include <charconv>
#include <set>
#include <stdexcept>
#include <unistd.h>

#include <algorithm>
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
#include <unordered_set>
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {
    MetricResult::ValueType CodeLinesCountMetric::CalculateImpl(const function::Function& f) const{
        auto res = f.ast | std::views::split('\n') | std::views::transform([](auto&& r){
            return std::string_view{r}; 
        }) | std::views::filter([](auto&& str){
            return !str.contains("comment");
        }) | std::views::transform([](auto&& r){
            int tmp_val = {0};

            auto start_pos = r.find_first_of('[');
            auto end_pos = r.find_first_of(',');
            auto [e,errc] = std::from_chars(r.begin() + start_pos + 1, r.begin() + end_pos, tmp_val);
            if(errc != std::errc()){
                throw std::runtime_error("CodeLinesCountMetric::CalculateImpl parsing line index error!");
            }

            return tmp_val; 
        }) | std::ranges::to<std::unordered_set>(); // unique

        return static_cast<int>(res.size());
    }

    std::string CodeLinesCountMetric::Name() const {
        return "code_lines_count";
    }

}  // namespace analyser::metric::metric_impl
