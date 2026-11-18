# cpp-middle-project4: software requirements specification

Reverse-engineered from the code at commit `63c76b4` on 2026-11-18. Every requirement describes behaviour present in the code; the source is given as `path:line`. "Inference" marks a statement read from code and not confirmed by a run. Documentation hub: [`docs/README.md`](docs/README.md).

## 1. Purpose and scope

The system computes per-function metrics for Python source files and aggregates them per file and per class (`README.md`).

In scope:

- the analyser core in `include/` and `src/` (static library `analyser_core`, `CMakeLists.txt:24-38`);
- the `analyser` command-line program (`main.cpp`, `src/cmd_options.cpp`);
- the GoogleTest suites in `src/*/tests/`.

Out of scope:

- parsing Python itself: the system relies on the external `tree-sitter` CLI and a Python grammar (`include/file.hpp:14-21`);
- writing results to files, any network or service interface: the code writes only to stdout and stderr (`main.cpp`);
- installation: `recipe.ps1 install` prints `nothing to install` (`recipe.ps1:93`).

## 2. Context

| Actor or system | Role | Source |
| --- | --- | --- |
| Command-line user | passes Python file paths with `-f` and reads stdout | `src/cmd_options.cpp:14-18` |
| C++ program using the library | includes `include/` and links the core | `docs/manual.md`, `docs/install.md` |
| `tree-sitter` CLI + Python grammar | started through `popen`/`_popen` to print the syntax tree | `src/file.cpp:23-46,72-98` |
| Boost.Program_options | command-line parsing, CLI only | `CMakeLists.txt:40-44` |
| GoogleTest v1.13.0 | test framework, fetched at configure time | `CMakeLists.txt:51-59` |

Runtime environment: Windows (MSVC `_popen`, or mingw g++ as used by `recipe.ps1:58`) and POSIX (`popen`, `WIFEXITED`) (`src/file.cpp:11-46`). The dev container is Ubuntu with g++ 15 and tree-sitter built from source (`.devcontainer/Dockerfile`).

## 3. Functional requirements

### 3.1 Reading a file

- **FR-1.** The system shall read a Python file into a list of lines and obtain its syntax tree as the text printed by `tree-sitter parse <file> 2>&1`. Source: `src/file.cpp:53-98`, `include/file.hpp:14-25`. Verification: indirectly by `metric_tests` (all `*MetricTest.CalculateFor*` cases).
- **FR-2.** On POSIX the system shall add `--config-path /root/.config/tree-sitter/config.json` to the tree-sitter command; on Windows it shall add no config path. Source: `include/file.hpp:14-21`. Verification: no test.
- **FR-3.** The system shall throw `std::invalid_argument` with message `Can't open file <name>` when the file cannot be opened. Source: `src/file.cpp:56-57`. Verification: no test.
- **FR-4.** The system shall throw `std::runtime_error` with message `Error while getting ast from <name>` when the pipe to the tree-sitter command cannot be opened (`popen`/`_popen` returns null). Source: `src/file.cpp:88-90,99-100`. Verification: no test. Inference: a missing `tree-sitter` binary does not take this path: the shell starts and exits non-zero, and the throw from the pipe deleter ends the process (`src/file.cpp:77-86`).

### 3.2 Finding functions

- **FR-5.** The system shall report one function for every `(function_definition` found in the tree text, taking as its tree the span up to the matching closing parenthesis, and shall resume the search after that span. Source: `src/function.cpp:31-65`. Verification: indirectly by `metric_tests` (`GetFunction` takes `functions.at(0)`).
- **FR-6.** The system shall take the function name from the source line and columns given by the first `(identifier` node of the function's tree, and shall return `unknown` when those coordinates lie outside the source. Source: `src/function.cpp:67-102`. Verification: no test.
- **FR-7.** The system shall assign a class name to a function when the start of its name lies inside the row/column range of a `(class_definition` node, using the last such class in the text, and shall read the name from the class's source line between the word `class` and the first of space, `:`, `{` or `(`. Source: `src/function.cpp:104-172`. Verification: no test (no fixture contains a class).
- **FR-8.** Each function record shall hold the file name, the optional class name, the function name and the function's tree text. Source: `include/function.hpp:30-35`. Verification: no test.

### 3.3 Metrics

- **FR-9.** `MetricExtractor` shall apply every registered metric to a function in registration order and return one `MetricResult` (metric name, value of type `int` or `std::string`) per metric. Source: `include/metric.hpp:31-55`, `src/metric.cpp:37-46`. Verification: no test.
- **FR-10.** `MetricExtractor::RegisterMetric` shall throw `std::runtime_error` when given a null pointer. Source: `src/metric.cpp:29-35`. Verification: no test.
- **FR-11.** `code_lines_count` shall be the number of distinct start rows among the tree-text lines of a function that do not contain the substring `comment`. Source: `src/metric_impl/code_lines_count.cpp:27-52`. Verification: `CodeLinesCountMetricTest.*` (11 cases).
- **FR-12.** `cyclomatic_complexity` shall be the number of nodes of the types `if_statement`, `elif_clause`, `while_statement`, `for_statement`, `try_statement`, `except_clause`, `finally_clause`, `match_statement`, `case_pattern`, `assert_statement`, `conditional_expression` in the function's tree, with no added base value. Source: `src/metric_impl/cyclomatic_complexity.cpp:31-46`. Verification: `CyclomaticComplexityMetricTest.*` (11 cases; `CalculateForComments` expects 0).
- **FR-13.** `parameters_count` shall be the number of tree-text lines containing `identifier` before the first line containing `body`, minus one. Source: `src/metric_impl/parameters_count.cpp:19-27`. Verification: `CountParametersMetricTest.*` (11 cases; `CalculateForManyParameters` expects 5 for `a, b, c=5, *args, **kwargs`).
- **FR-14.** `naming_style` shall classify the function name as `snake_case` (no upper case, has `_`, only lower case, digits and `_`), `lowercase` (no upper case, no `_`), `camelCase` (starts lower case, has upper case, no `_`), `PascalCase` (starts upper case, no `_`, has lower case or is one character) or otherwise `unknown`, including for an empty name. Source: `src/metric_impl/naming_style.cpp:10-44`. Verification: `NamingStyleMetricTest.*`, `NamingStyleMetricSimpleTest.Name`.

### 3.4 Accumulators

- **FR-15.** `SumAverageAccumulator` shall return the sum and the arithmetic mean of the accumulated `int` values, with mean 0 when nothing was accumulated. Source: `src/metric_accumulator_impl/sum_average_accumulator.cpp:24-53`. Verification: `SumAverageAccumulatorTest.*` (5 cases).
- **FR-16.** `AverageAccumulator` shall return the arithmetic mean of the accumulated `int` values, 0 when nothing was accumulated. Source: `src/metric_accumulator_impl/average_accumulator.cpp:24-52`. Verification: `AverageAccumulatorTest.*` (6 cases).
- **FR-17.** `CategoricalAccumulator` shall count occurrences per value, converting `int` values to their decimal string. Source: `src/metric_accumulator_impl/categorical_accumulator.cpp:24-52`. Verification: `CategoricalAccumulatorTest.*` (6 cases).
- **FR-18.** Each accumulator's `Get` shall throw `std::runtime_error` when called before `Finalize`. Source: `src/metric_accumulator_impl/average_accumulator.cpp:47-50`, `categorical_accumulator.cpp:47-50`, `sum_average_accumulator.cpp:48-51`. Verification: `*AccumulatorTest.GetBeforeFinalize`.
- **FR-19.** `Accumulate` after `Finalize` shall reset the accumulator before adding the value. Source: line 25-26 of each accumulator source. Verification: `AverageAccumulatorTest.AccumulateAfterFinalizeResets`; no test for the other two.
- **FR-20.** `MetricsAccumulator` shall keep one accumulator per metric name (the first registration of a name is kept), pass each `MetricResult` to the accumulator of the same name, finalize an accumulator when it is fetched with `GetFinalizedAccumulator`, and reset all accumulators on `ResetAccumulators`. Source: `include/metric_accumulator.hpp:40-57`, `src/metric_accumulator.cpp:28-36`. Verification: no test.

### 3.5 Analysis pipeline

- **FR-21.** `AnalyseFunctions` shall, for each file in the given order, extract its functions and return their metric results as a vector of `FunctionAnalysisResult`. Source: `include/analyse.hpp:36-39,57-77`. Verification: no test.
- **FR-22.** `SplitByFiles` shall group consecutive results with the same file name. Source: `include/analyse.hpp:91-100`. Verification: no test.
- **FR-23.** `SplitByClasses` shall drop results without a class and group consecutive remaining results with the same class name. Source: `include/analyse.hpp:79-89`. Verification: no test.

### 3.6 Command-line program

- **FR-24.** The program shall accept `-f`/`--file` with one or more paths (required) and `-h`/`--help`. Source: `src/cmd_options.cpp:14-18`. Verification: no test.
- **FR-25.** For each function the program shall print `<file>[::<class>]::<name>` followed by one line `    <metric>: <value>` for `code_lines_count`, `cyclomatic_complexity`, `parameters_count`, `naming_style`, in that order. Source: `main.cpp:54-73`. Verification: no test.
- **FR-26.** For each file group the program shall print `Accumulated Analysis for file <file>:` with sum and average of `code_lines_count` and `cyclomatic_complexity`, average of `parameters_count` (two decimals) and the count per `naming_style`. Source: `main.cpp:75-107`. Verification: no test.
- **FR-27.** For each class group the program shall print the same block under `Accumulated Analysis for class <class>:`. Source: `main.cpp:109-136`. Verification: no test.
- **FR-28.** On any `std::exception` the program shall print `Error: <message>` to stderr and exit with code 1. Source: `main.cpp:138-141`. Verification: no test.

### 3.7 Declared but not implemented, or inconsistent

- An installer: "there is no installer yet" (`docs/install.md`); `recipe.ps1:93` prints `nothing to install`.
- A gate for the CLI: "No gate builds or checks it yet" (`docs/manual.md`).
- Manual tests: `tests/manual/tests.json` has an empty `tests` list.
- `range-v3/0.12.0` is required in `conanfile.py:54`, but no file in `include/`, `src/` or `main.cpp` includes it.
- `CMakePresets.json:17` asks for C++26 while `CMakeLists.txt:5` sets 23; inference: the `set()` in `CMakeLists.txt` takes precedence over the preset's cache value.
- `.vscode/launch.json` runs `build/src/metric_impl/metric_test` and `build/src/metric_accumulator_impl/metric_accumulator_test`; `CMakeLists.txt` builds `metric_tests` and `accumulator_tests` instead.
- `main.cpp:50-52` runs `throw;` when option parsing returns false; with no exception being handled this calls `std::terminate` (inference from the C++ rule), so `--help` and parse errors do not end through FR-28.

## 4. Interfaces

### 4.1 Command line

| Option | Argument | Required | Effect | Source |
| --- | --- | --- | --- | --- |
| `-f`, `--file` | one or more paths | yes | files to analyse | `src/cmd_options.cpp:16-17` |
| `-h`, `--help` | none | no | prints `Allowed options` and returns false from `Parse` | `src/cmd_options.cpp:15,27-30` |

### 4.2 Library API

| Type / function | Header | Contract |
| --- | --- | --- |
| `file::File(const std::string&)` | `include/file.hpp` | fields `name`, `ast`, `source_lines`; FR-1..FR-4 |
| `function::FunctionExtractor::Get(const File&)` | `include/function.hpp` | `std::vector<Function>`; FR-5..FR-8 |
| `metric::IMetric` | `include/metric.hpp` | `Calculate` = `{Name(), CalculateImpl(f)}` |
| `metric::MetricExtractor` | `include/metric.hpp` | `RegisterMetric`, `Get`; FR-9, FR-10 |
| `metric_accumulator::IAccumulator` | `include/metric_accumulator.hpp` | `Accumulate`, `Finalize`, `Reset` |
| `metric_accumulator::MetricsAccumulator` | `include/metric_accumulator.hpp` | FR-20 |
| `AnalyseFunctions`, `SplitByFiles`, `SplitByClasses`, `AccumulateFunctionAnalysis` | `include/analyse.hpp` | FR-21..FR-23 |

### 4.3 External process and file formats

| Item | Format | Source |
| --- | --- | --- |
| Input | Python source file, read line by line | `src/file.cpp:63-70` |
| Tree-sitter output | S-expression text with `[row, column] - [row, column]` coordinates, parsed with `std::string::find` and `std::from_chars` | `src/function.cpp:67-89`, `src/metric_impl/code_lines_count.cpp:33-40` |
| stdout | text blocks of FR-25..FR-27 | `main.cpp:62-136` |
| stderr | `Error: <message>`, or Boost parse errors `Error parsing command line: ...` | `main.cpp:139`, `src/cmd_options.cpp:41-43` |

### 4.4 Project commands (`recipe.ps1`)

| Verb | Effect here | Source |
| --- | --- | --- |
| `build` | CMake + Ninja, g++, Release, `ANALYSER_CLI=OFF` | `recipe.ps1:56-62,89-92` |
| `test` | build, `ctest`, `clang-tidy`, `clang-format -i`, word checks | `recipe.ps1:130-147` |
| `run` | build, run `build\metric_tests.exe` with the remaining arguments | `recipe.ps1:101-110` |
| `publish` | copy `include`, `src` under `$env:LAB_SHARE\needs\cpp-middle-project4\v1.0.0` | `recipe.ps1:111-121` |

## 5. Data

| Entity | Fields | Source |
| --- | --- | --- |
| `Function` | `filename`, `class_name` (optional), `name`, `ast` | `include/function.hpp:30-35` |
| `MetricResult` | `metric_name`, `value` (`std::variant<int, std::string>`) | `include/metric.hpp:31-35` |
| `FunctionAnalysisResult` | `func_info`, `metrics` | `include/analyse.hpp:36-39` |
| `SumAverage` | `sum` (`int`), `average` (`double`) | `include/metric_accumulator_impl/sum_average_accumulator.hpp:27-31` |

Storage: none. All data lives in memory for one run. Test fixtures: ten Python files in `src/metric_impl/tests/files/`, copied into the build directory by `CMakeLists.txt:81-85`. Sample inputs: `files/comments.py`, `files/loops.py`, `files/sample.py`.

## 6. Non-functional requirements

- **NFR-1.** The code shall compile as C++23 without compiler extensions. Source: `CMakeLists.txt:5-7`.
- **NFR-2.** Outside MSVC, the core and test targets shall compile with `-Wall -Wextra -Wpedantic -Werror`. Source: `CMakeLists.txt:16-22`.
- **NFR-3.** The sources under `src/` (except `src/cmd_options.cpp`) shall pass `clang-tidy` with the checks of `.clang-tidy`, warnings as errors; `include/*.hpp`, `src/*.cpp` and `main.cpp` shall be formatted by `.clang-format` (LLVM base, 120 columns). Source: `.clang-tidy`, `.clang-format`, `recipe.ps1:139-143`.
- **NFR-4.** `include/` and `src/` shall contain no locking primitive names. Source: `recipe.ps1:144`.
- **NFR-5.** Within `include/` and `src/`, `throw`, `try` and `catch` shall appear only in the nine files of `$ThrowsOnPurpose`, on exactly 17 lines. Source: `recipe.ps1:34-35,145-146`. Checked on 2026-11-18 with `git grep`: 17 lines, none elsewhere in `include/` or `src/`; `main.cpp`, which the check does not cover, has three such lines (49, 51, 138).
- **NFR-6.** The core shall build without Boost when `ANALYSER_CLI=OFF`. Source: `CMakeLists.txt:11-13,40-44`.
- **NFR-7.** The GoogleTest archive shall be checked against a fixed SHA256. Source: `CMakeLists.txt:51-56`.
- **NFR-8.** The repository follows the lab tooling of repo-quality-template; `README.md` states compliance with revision 4.0.0 of the lab requirements except ten listed items.

No performance, memory or throughput targets appear in the code or configuration.

## 7. Constraints and dependencies

| Dependency | Version | Source |
| --- | --- | --- |
| CMake | 3.28 or newer (presets file says 3.25) | `CMakeLists.txt:1`, `CMakePresets.json:3-7` |
| g++ | 13 or newer for the core, 14 or newer for the CLI (`<print>`) | `docs/install.md` |
| GoogleTest | v1.13.0 (FetchContent); `gtest/1.13.0` in Conan | `CMakeLists.txt:53`, `conanfile.py:53` |
| Boost | `program_options` via `find_package`; `boost/1.83.0` in Conan | `CMakeLists.txt:41`, `conanfile.py:52` |
| range-v3 | `0.12.0`, declared only | `conanfile.py:54` |
| CMake (Conan tool) | `3.30.0` | `conanfile.py:55` |
| tree-sitter CLI, tree-sitter-python | not pinned: the Dockerfile installs the CLI with `cargo install tree-sitter-cli` (no version) and clones the grammar from its default branch | `.devcontainer/Dockerfile` |
| Python + pytest | not pinned; needed by the `manual_list` ctest test | `CMakeLists.txt:88-92` |

No lock file exists for Conan or Python.

## 8. Acceptance

1. `.\recipe.ps1 test` exits 0: builds the core, runs `accumulator_tests` and `manual_list` through `ctest`, and checks NFR-1..NFR-5 (FR-15..FR-19).
2. With tree-sitter and its Python grammar installed, `.\recipe.ps1 run` exits 0 with all 39 cases of `metric_tests` passing (FR-1, FR-5, FR-11..FR-14).
3. Build the CLI as in `docs/install.md`, run `analyser -f files/comments.py files/loops.py`, and compare the output with the shapes of FR-25..FR-27. No expected-output file exists in the repository, so this step has no fixed reference.

## 9. Open questions

1. Who uses the analyser beyond the course, and on which Python versions and tree-sitter grammar versions it must work.
2. Whether a cyclomatic complexity without a base of 1 (FR-12) is intended; the tests pin the current values.
3. Whether `throw;` in `main.cpp:51` is meant to end the program on `--help`, and which exit code `--help` should have.
4. Whether the CLI remains a deliverable, given that no gate builds it.
5. Whether a Gitea runner has executed the workflows; `CLAUDE.md` says branch protection stays at stage 1 until `propose` and `release-gate` have run once.
6. The content of the ten exceptions (issues #8-#17) named in `README.md`; the issues are not in the repository.
7. What `tree-sitter-sample.txt` is for: it holds one command line and no parser output, and has been in the repository since the initial commit `5c9c262` (2025-06-30).
8. Why `range-v3` is a Conan requirement; it has been in `conanfile.py` since the initial commit `5c9c262`.
9. How names with non-ASCII characters should be classified by `naming_style`. What the code does: it tests single `char` bytes with `<cctype>` functions (`src/metric_impl/naming_style.cpp:16-21,31,35`); passing them a negative `char` is undefined behaviour, so for UTF-8 names on a signed-`char` platform the result is not defined.
