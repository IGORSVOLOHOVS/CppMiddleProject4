#include <iterator>
#include <map>
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

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

namespace analyser {

namespace rv = std::ranges::views;
namespace rs = std::ranges;

<<<<<<< Updated upstream
auto AnalyseFunctions(const std::vector<std::string> &files,
                      const analyser::metric::MetricExtractor &metric_extractor) {
=======

// AnalyseFunctions, которая:
// принимает список имен файлов и объект MetricExtractor,
// запускает создание структур File на основе имен файлов,
// для каждого файла с помощью FunctionExtractor выделяет список объектов Function,
// объединяет списки объектов Function в один список,
// выделяет из каждой функции метрики,
// возвращает вектор пар из объекта Function и набора результатов вычисления метрик MetricResult;
struct FunctionAnalysisResult {
    analyser::function::Function func_info;
    analyser::metric::MetricResults metrics;
};

template <typename T>
concept MetricExtractorConcept = requires(T& extractor, const function::Function& func) {
    { extractor.Get(func) } -> std::same_as<std::vector<metric::MetricResult>>;
};

template <typename T>
concept MetricsAccumulatorConcept = requires(T& accumulator, const metric::MetricResult& res) {
    { accumulator.Accumulate(res) };
};

template <typename R>
concept AnalysisResultRange = rs::range<R> && std::same_as<rs::range_value_t<R>, FunctionAnalysisResult>;

template <typename R>
concept AnalysisResult2DRange = rs::range<R> && AnalysisResultRange<rs::range_value_t<R>>;


AnalysisResultRange auto AnalyseFunctions(const std::vector<std::string>& files,
                      const analyser::metric::MetricExtractor& metric_extractor) {
>>>>>>> Stashed changes
    // здесь ваш код
    std::vector<FunctionAnalysisResult> res{};
    rs::for_each(files, [&metric_extractor, &res](const auto& str){
        auto file = file::File{str};
        auto functions = function::FunctionExtractor{}.Get(file);

        rs::for_each(functions, [&metric_extractor, &res](const auto& f){
            auto metrics = metric_extractor.Get(f);
            res.push_back({.func_info = f, .metrics = metrics});
        });
    });

    return res;
}

<<<<<<< Updated upstream
auto SplitByClasses(const auto &analysis) {
    // здесь ваш код
}

auto SplitByFiles(const auto &analysis) {
    // здесь ваш код
}

void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyser::metric_accumulator::MetricsAccumulator &accumulator) {
    // здесь ваш код
=======
AnalysisResult2DRange auto SplitByClasses(const AnalysisResultRange auto& analysis) {
    auto res = analysis | std::views::filter([](const auto& far){
        return far.func_info.class_name.has_value();
    }) | std::views::chunk_by([](const auto& lfar, const auto& rfar){
        return lfar.func_info.class_name.value() == rfar.func_info.class_name.value();
    }) | rs::to<std::vector<std::vector<FunctionAnalysisResult>>>();
    return res;
}

AnalysisResult2DRange auto SplitByFiles(const AnalysisResultRange auto& analysis) {
    auto res = analysis | std::views::chunk_by([](const auto& lfar, const auto& rfar){
        return lfar.func_info.filename == rfar.func_info.filename;
    }) | rs::to<std::vector<std::vector<FunctionAnalysisResult>>>();
    return res;
}

void AccumulateFunctionAnalysis(
    const AnalysisResultRange auto& analysis, const analyser::metric_accumulator::MetricsAccumulator& accumulator) {
    
    rs::for_each(analysis, [&accumulator](const auto& far){
        accumulator.AccumulateNextFunctionResults(far.metrics);
    
    });

       
>>>>>>> Stashed changes
}

}  // namespace analyser
