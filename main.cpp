#include <exception>
#include <memory>
#include <stdexcept>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
    analyser::cmd::ProgramOptions options;
    try {
        if (!options.Parse(argc, argv)) {
            return 1;
        }

        analyser::metric::MetricExtractor metric_extractor{};
        metric_extractor.RegisterMetric(std::make_unique<analyser::metric::metric_impl::CodeLinesCountMetric>());
        metric_extractor.RegisterMetric(std::make_unique<analyser::metric::metric_impl::CyclomaticComplexityMetric>());
        metric_extractor.RegisterMetric(std::make_unique<analyser::metric::metric_impl::CountParametersMetric>());
        metric_extractor.RegisterMetric(std::make_unique<analyser::metric::metric_impl::NamingStyleMetric>()); // Регистрируем новую метрику

        auto analysis_results = analyser::AnalyseFunctions(options.GetFiles(), metric_extractor);
        
        rs::for_each(analysis_results, [](const auto& res) {
            std::println("{}{}::{}", res.func_info.filename,
                         res.func_info.class_name.has_value() ? "::" + res.func_info.class_name.value() : "",
                         res.func_info.name);
            rs::for_each(res.metrics, [](const auto& metric) {
                std::print("    {}: ", metric.metric_name);
                std::visit([](const auto& val) { std::println("{}", val); }, metric.value);
            });
        });

        analyser::metric_accumulator::MetricsAccumulator accumulator{};
        accumulator.RegisterAccumulator("code_lines_count", std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>());
        accumulator.RegisterAccumulator("cyclomatic_complexity", std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>());
        accumulator.RegisterAccumulator("parameters_count", std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::AverageAccumulator>());
        accumulator.RegisterAccumulator("naming_style", std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::CategoricalAccumulator>()); // Регистрируем аккумулятор

        auto by_files = analyser::SplitByFiles(analysis_results);
        rs::for_each(by_files, [&](const auto& file_group) {
            if (file_group.empty()) return;
            accumulator.ResetAccumulators();
            analyser::AccumulateFunctionAnalysis(file_group, accumulator);
            std::println("\nAccumulated Analysis for file {}:", file_group.front().func_info.filename);
            
            auto& sum_avg_clc = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>("code_lines_count");
            auto clc = sum_avg_clc.Get();
            std::println("    code_lines_count: sum = {}, average = {:.2f}", clc.sum, clc.average);

            auto& sum_avg_cc = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>("cyclomatic_complexity");
            auto cc = sum_avg_cc.Get();
            std::println("    cyclomatic_complexity: sum = {}, average = {:.2f}", cc.sum, cc.average);

            auto& avg_pc = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::AverageAccumulator>("parameters_count");
            std::println("    parameters_count: average = {:.2f}", avg_pc.Get());

            auto& cat_ns = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::CategoricalAccumulator>("naming_style");
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
            
            auto& sum_avg_clc = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>("code_lines_count");
            auto clc = sum_avg_clc.Get();
            std::println("    code_lines_count: sum = {}, average = {:.2f}", clc.sum, clc.average);

            auto& sum_avg_cc = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>("cyclomatic_complexity");
            auto cc = sum_avg_cc.Get();
            std::println("    cyclomatic_complexity: sum = {}, average = {:.2f}", cc.sum, cc.average);

            auto& avg_pc = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::AverageAccumulator>("parameters_count");
            std::println("    parameters_count: average = {:.2f}", avg_pc.Get());

            auto& cat_ns_class = accumulator.GetFinalizedAccumulator<analyser::metric_accumulator::metric_accumulator_impl::CategoricalAccumulator>("naming_style");
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