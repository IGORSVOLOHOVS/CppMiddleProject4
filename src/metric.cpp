#include "metric.hpp"

#include <iterator>
#include <stdexcept>
#if __has_include(<unistd.h>)  // POSIX-only header, MSVC does not ship it
#include <unistd.h>
#endif

#include <algorithm>
#include <any>
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

#include "function.hpp"

namespace analyser::metric {

void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) {
    if(metric) {
        metrics.emplace_back(std::move(metric));
    }else{
        throw std::runtime_error("MetricExtractor::RegisterMetric metric is nullptr");
    }
}

MetricResults MetricExtractor::Get(const function::Function &func) const {
    namespace vs = std::views;
    namespace rs = std::ranges;
    
    return metrics | vs::transform([&func](auto&& metric){
        return metric->Calculate(func);
    }) | rs::to<std::vector>();
}

}  // namespace analyser::metric
