# Manual

## The library

A file is read once, its functions are extracted, and each function is measured:

```cpp
#include "file.hpp"
#include "function.hpp"
#include "metric_impl/metrics.hpp"

using namespace analyser;

file::File file("example.py");  // runs `tree-sitter parse example.py`
auto functions = function::FunctionExtractor{}.Get(file);

metric::MetricExtractor extractor;
extractor.RegisterMetric(std::make_unique<metric::metric_impl::CyclomaticComplexityMetric>());
for (const auto& function : functions) {
    auto results = extractor.Get(function);  // one MetricResult per registered metric
}
```

`file::File` throws `std::invalid_argument` for a file that does not open and `std::runtime_error` when the tree-sitter command cannot start. A tree-sitter command that exits with a non-zero code ends the process: the throw leaves the destructor of a pipe.

`metric_accumulator::MetricsAccumulator` adds the results of many functions up. Call `Finalize` on each accumulator before `Get`, or `Get` throws `std::runtime_error`.

## The tests

`.\recipe.ps1 run` builds and runs the metric tests from `build\`, and hands them any further arguments. They need the tree-sitter CLI:

| command | what it does |
| --- | --- |
| `.\recipe.ps1 run` | every metric test case |
| `.\recipe.ps1 run --gtest_list_tests` | names the test cases |
| `.\recipe.ps1 run --gtest_filter=CyclomaticComplexityMetricTest.*` | runs one suite |

`build\accumulator_tests.exe` runs the accumulator tests, which need nothing else; `ctest` runs them too.

## The CLI

Built as `docs/install.md` says, `analyser` takes Python files and prints each function's metrics, then the sums and averages per file and per class. No gate builds or checks it yet.
