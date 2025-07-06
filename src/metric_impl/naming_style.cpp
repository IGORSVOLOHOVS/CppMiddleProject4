#include "metric_impl/naming_style.hpp"

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
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

// здесь ваш код
    MetricResult::ValueType NamingStyleMetric::CalculateImpl(const function::Function& f) const{        
        throw std::runtime_error("NamingStyleMetric::CalculateImpl is not implemented yet!");
    }

    std::string NamingStyleMetric::Name() const {
        throw std::runtime_error("NamingStyleMetric::CalculateImpl is not implemented yet!");
    }

}  // namespace analyser::metric::metric_impl
