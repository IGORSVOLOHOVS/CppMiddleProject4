#include "metric_accumulator_impl/average_accumulator.hpp"

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

void AverageAccumulator::Accumulate(const metric::MetricResult& metric_result){
    if(is_finalized) Reset();

    sum += std::get<int>(metric_result.value);
    count++;
}
void AverageAccumulator::Finalize() {
    if (count > 0) {
        average = static_cast<double>(sum) / count;
    } else {
        average = 0.0; 
    }
    is_finalized = true;
}

void AverageAccumulator::Reset() {
    sum = 0;
    count = 0;
    average = 0;
    is_finalized = false;
}

double AverageAccumulator::Get() const{
    if (!is_finalized) {
        throw std::runtime_error("AverageAccumulator::Get() called before Finalize().");
    }
    return average;
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl
