#include "metric.hpp"

#include <iterator>
#include <stdexcept>
#include <unistd.h>

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
