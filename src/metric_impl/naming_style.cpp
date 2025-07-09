#include "metric_impl/naming_style.hpp"
#include <cctype>
#include <ranges>
#include <algorithm>

namespace rs = std::ranges;

namespace analyser::metric::metric_impl {

MetricResult::ValueType NamingStyleMetric::CalculateImpl(const function::Function& f) const {
    const auto& name = f.name;
    if (name.empty()) {
        return "unknown";
    }

    bool has_upper = rs::any_of(name, ::isupper);
    bool has_lower = rs::any_of(name, ::islower);
    bool has_underscore = name.find('_') != std::string::npos;

    if (!has_upper && has_underscore) {
        bool is_valid_snake = rs::all_of(name, [](char c){ 
            return std::islower(c) || std::isdigit(c) || c == '_'; 
        });
        if (is_valid_snake) {
            return "snake_case";
        }
    }

    if (!has_upper && !has_underscore) {
        return "lowercase";
    }

    if (std::islower(name[0]) && has_upper && !has_underscore) {
        return "camelCase";
    }

    if (std::isupper(name[0]) && !has_underscore) {
        if (has_lower || name.length() == 1) {
            return "PascalCase";
        }
    }
    
    return "unknown";
}

std::string NamingStyleMetric::Name() const {
    return "naming_style";
}

} // namespace analyser::metric::metric_impl