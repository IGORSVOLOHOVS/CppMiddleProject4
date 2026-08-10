#include <exception>
#include <format>
#include <iterator>
#include <memory>
#include <stdexcept>
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
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "analyse.hpp"
#include "cmd_options.hpp"
#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_accumulator_impl/accumulators.hpp"
#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "metric_impl/code_lines_count.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/metrics.hpp"

namespace metric = analyser::metric::metric_impl;
namespace metric_acc = analyser::metric_accumulator::metric_accumulator_impl;

namespace vs = std::views;
namespace rs = std::ranges;

int main(int argc, char *argv[]) {
    analyser::cmd::ProgramOptions options;
    try {
        if (!options.Parse(argc, argv)) {
            throw;
        }

        analyser::metric::MetricExtractor metric_extractor{};
        metric_extractor.RegisterMetric(std::make_unique<metric::CodeLinesCountMetric>());
        metric_extractor.RegisterMetric(std::make_unique<metric::CyclomaticComplexityMetric>());
        metric_extractor.RegisterMetric(std::make_unique<metric::CountParametersMetric>());
        metric_extractor.RegisterMetric(std::make_unique<metric::NamingStyleMetric>()); 

        auto analysis_results = analyser::AnalyseFunctions(options.GetFiles(), metric_extractor);
        
        analysis_results | vs::transform([](auto&& res){
            std::println("{}{}::{}", res.func_info.filename,
                         res.func_info.class_name.has_value() ? "::" + res.func_info.class_name.value() : "",
                         res.func_info.name);
            return res.metrics;
        }) | vs::join | vs::transform([](auto&& metric){
            std::print("    {}: ", metric.metric_name);
            std::visit([](const auto& val) { std::println("{}", val); }, metric.value);
            return 0;
        }) | rs::to<std::vector>(); 

        analyser::metric_accumulator::MetricsAccumulator accumulator{};
        accumulator.RegisterAccumulator("code_lines_count", std::make_unique<metric_acc::SumAverageAccumulator>());
        accumulator.RegisterAccumulator("cyclomatic_complexity", std::make_unique<metric_acc::SumAverageAccumulator>());
        accumulator.RegisterAccumulator("parameters_count", std::make_unique<metric_acc::AverageAccumulator>());
        accumulator.RegisterAccumulator("naming_style", std::make_unique<metric_acc::CategoricalAccumulator>()); 

        auto by_files = analyser::SplitByFiles(analysis_results);
        rs::for_each(by_files, [&](const auto& file_group) {
            if (file_group.empty()) return;
            accumulator.ResetAccumulators();
            analyser::AccumulateFunctionAnalysis(file_group, accumulator);
            std::println("\nAccumulated Analysis for file {}:", file_group.front().func_info.filename);
            
            auto& sum_avg_clc = accumulator.GetFinalizedAccumulator<metric_acc::SumAverageAccumulator>("code_lines_count");
            auto clc = sum_avg_clc.Get();
            std::println("    code_lines_count: sum = {}, average = {:.2f}", clc.sum, clc.average);

            auto& sum_avg_cc = accumulator.GetFinalizedAccumulator<metric_acc::SumAverageAccumulator>("cyclomatic_complexity");
            auto cc = sum_avg_cc.Get();
            std::println("    cyclomatic_complexity: sum = {}, average = {:.2f}", cc.sum, cc.average);

            auto& avg_pc = accumulator.GetFinalizedAccumulator<metric_acc::AverageAccumulator>("parameters_count");
            std::println("    parameters_count: average = {:.2f}", avg_pc.Get());

            auto& cat_ns = accumulator.GetFinalizedAccumulator<metric_acc::CategoricalAccumulator>("naming_style");
            std::println("    naming_style:");
            for(const auto& [style, count] : cat_ns.Get()) {
                std::println("        {}: {}", style, count);
            } 
        });

        auto by_classes = analyser::SplitByClasses(analysis_results);
         rs::for_each(by_classes, [&](const auto& class_group) {
            if (class_group.empty() || !class_group.front().func_info.class_name.has_value()) return;
            accumulator.ResetAccumulators();
            analyser::AccumulateFunctionAnalysis(class_group, accumulator);
            std::println("\nAccumulated Analysis for class {}:", class_group.front().func_info.class_name.value());
            
            auto& sum_avg_clc = accumulator.GetFinalizedAccumulator<metric_acc::SumAverageAccumulator>("code_lines_count");
            auto clc = sum_avg_clc.Get();
            std::println("    code_lines_count: sum = {}, average = {:.2f}", clc.sum, clc.average);

            auto& sum_avg_cc = accumulator.GetFinalizedAccumulator<metric_acc::SumAverageAccumulator>("cyclomatic_complexity");
            auto cc = sum_avg_cc.Get();
            std::println("    cyclomatic_complexity: sum = {}, average = {:.2f}", cc.sum, cc.average);

            auto& avg_pc = accumulator.GetFinalizedAccumulator<metric_acc::AverageAccumulator>("parameters_count");
            std::println("    parameters_count: average = {:.2f}", avg_pc.Get());

            auto& cat_ns_class = accumulator.GetFinalizedAccumulator<metric_acc::CategoricalAccumulator>("naming_style");
            std::println("    naming_style:");
            for(const auto& [style, count] : cat_ns_class.Get()) {
                std::println("        {}: {}", style, count);
            }
        });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}