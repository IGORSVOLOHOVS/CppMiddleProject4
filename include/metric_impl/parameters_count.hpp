#ifndef ANALYSER_METRIC_IMPL_PARAMETERS_COUNT_HPP
#define ANALYSER_METRIC_IMPL_PARAMETERS_COUNT_HPP

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

#include "metric.hpp"

namespace analyser::metric::metric_impl {

struct CountParametersMetric final : public IMetric {
    MetricResult::ValueType CalculateImpl(const function::Function &f) const;
    std::string Name() const;
};

}  // namespace analyser::metric::metric_impl

#endif  // ANALYSER_METRIC_IMPL_PARAMETERS_COUNT_HPP
