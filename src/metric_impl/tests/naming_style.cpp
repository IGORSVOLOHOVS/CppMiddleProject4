#include "metric_impl/naming_style.hpp"
#include <cctype>
#include <ranges>
#include <algorithm>

namespace analyser::metric::metric_impl {

namespace {
    bool is_all_lower(const std::string& name) {
        return std::ranges::all_of(name, [](char c){ return std::islower(c) || std::isdigit(c); });
    }

    bool has_upper(const std::string& name) {
        return std::ranges::any_of(name, ::isupper);
    }

    bool has_underscore(const std::string& name) {
        return name.find_first_of('_') != std::string::npos;
    }
}

MetricResult::ValueType NamingStyleMetric::CalculateImpl(const function::Function& f) const {
    const auto& name = f.name;
    if (name.empty()) {
        return "unknown";
    }

    if (std::isupper(name[0]) && !has_underscore(name)) {
        return "PascalCase";
    }
    if (std::islower(name[0]) && has_upper(name) && !has_underscore(name)) {
        return "camelCase";
    }
    if (is_all_lower(name) && has_underscore(name)) {
        return "snake_case";
    }
    if (is_all_lower(name) && !has_underscore(name)) {
        return "lowercase";
    }
    
    return "unknown";
}

std::string NamingStyleMetric::Name() const {
    return "naming_style";
}

} // namespace analyser::metric::metric_impl