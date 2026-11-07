# cpp-middle-project4

A metrics analyser for Python functions in modern C++23. It began as the sprint 4 project of the Yandex Practicum "Middle C++ developer" course. It asks the `tree-sitter` CLI for the syntax tree of each Python file, finds every function and method in it, and measures each one: lines of code, cyclomatic complexity, number of parameters and naming style. Then it adds the results up per file and per class.

## What it does

- **Files**: `file::File` reads a Python file and its tree-sitter syntax tree.
- **Functions**: `function::FunctionExtractor` finds each function, its class and its source lines.
- **Metrics**: `CodeLinesCountMetric`, `CyclomaticComplexityMetric`, `CountParametersMetric` and `NamingStyleMetric`, run together by `metric::MetricExtractor`.
- **Accumulators**: `SumAverageAccumulator`, `AverageAccumulator` and `CategoricalAccumulator`, run together by `metric_accumulator::MetricsAccumulator`.
- **The CLI**: `analyser`, in `main.cpp` and `src/cmd_options.cpp`, takes file names through Boost.Program_options and prints the analysis.

Errors are exceptions, on purpose: a file that does not open, a tree-sitter command that fails, and an accumulator read before `Finalize` all throw, and `.\recipe.ps1 test` pins how many such lines there are.

## Layout

```text
include/                   the analyser, as headers
src/                       the core, and src/cmd_options.cpp for the CLI
src/*/tests/               GoogleTest cases, and the Python files the metric tests read
main.cpp                   the CLI
```

## Build and test

`.\recipe.ps1 test` builds the core, runs `ctest`, `clang-tidy` and `clang-format`, and searches `include` and `src` for locks and for `throw`, `try` and `catch`. It needs mingw g++, cmake, ninja and LLVM, and neither Conan nor Boost: it configures with `-DANALYSER_CLI=OFF`. `ctest` runs the accumulator tests. The metric tests start the tree-sitter CLI, so they are built and linted but left out of `ctest`; `.\recipe.ps1 run` starts them. `docs/install.md` has the details, and `docs/manual.md` covers the library and the CLI.

MIT licensed: `LICENSE` is the course's, copyright Yandex Practicum.

## Lab requirements

This repository complies with [revision 4.0.0](http://127.0.0.1:3000/igors-lab/software-requirements/src/tag/v4.0.0/BEST_REQUIREMENTS.md) of the lab requirements, except R-02 (#8), R-09 (#9), R-12 (#10), R-13 (#11), R-13 (#12), R-13 (#13), R-14 (#14), R-37 (#15), R-41 (#16), R-42 (#17).
