#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#if __has_include(<unistd.h>)  // POSIX-only header, MSVC does not ship it
#include <unistd.h>
#endif

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

namespace analyser::metric_accumulator::metric_accumulator_impl {

    void SumAverageAccumulator::Accumulate(const metric::MetricResult& metric_result) {
        if(is_finalized) Reset();

        sum += std::get<int>(metric_result.value);
        count++;
    }

    void SumAverageAccumulator::Finalize() {
        if (count > 0) {
            average = static_cast<double>(sum) / count;
        } else {
            average = 0.0; 
        }
        is_finalized = true;
    }

    void SumAverageAccumulator::Reset() {
        sum = 0;
        count = 0;
        average = 0;
        is_finalized = false;
    }

    SumAverageAccumulator::SumAverage SumAverageAccumulator::Get() const {
        if (!is_finalized) {
            throw std::runtime_error("SumAverageAccumulator::Get() called before Finalize().");
        }
        return SumAverage{.sum = sum, .average = average};
    }

}  // namespace analyser::metric_accumulator::metric_accumulator_impl
