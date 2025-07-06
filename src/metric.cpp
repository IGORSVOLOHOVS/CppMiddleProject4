#include "metric.hpp"

#include <iterator>
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
    if(metric) metrics.emplace_back(std::move(metric));
}

MetricResults MetricExtractor::Get(const function::Function &func) const {
    // здесь ваш код
    MetricResults res{};
    std::ranges::transform(metrics, std::back_inserter(res), [&func](auto&& m){
        return m->Calculate(func);
    });
    return res;
}

}  // namespace analyser::metric
