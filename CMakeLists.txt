cmake_minimum_required(VERSION 3.28)
project(analyser VERSION 1.0.0 LANGUAGES CXX)

# C++23: g++ 13 on the lab machine knows no C++26, and nothing here needs it.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# clang-tidy reads this: without it the FetchContent headers are not found.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The command-line front end is the only part that needs Boost.Program_options and <print>.
# recipe.ps1 switches it off, so the analyser core builds and tests with g++ 13 and no Conan.
option(ANALYSER_CLI "Build the analyser executable, which needs Boost.Program_options" ON)
option(BUILD_TESTING "Build tests" ON)

function(warn_as_errors target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8 /bigobj)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

add_library(analyser_core STATIC
    src/file.cpp
    src/function.cpp
    src/metric.cpp
    src/metric_accumulator.cpp
    src/metric_accumulator_impl/average_accumulator.cpp
    src/metric_accumulator_impl/categorical_accumulator.cpp
    src/metric_accumulator_impl/sum_average_accumulator.cpp
    src/metric_impl/code_lines_count.cpp
    src/metric_impl/cyclomatic_complexity.cpp
    src/metric_impl/naming_style.cpp
    src/metric_impl/parameters_count.cpp
)
target_include_directories(analyser_core PUBLIC include)
warn_as_errors(analyser_core)

if(ANALYSER_CLI)
    find_package(Boost REQUIRED COMPONENTS program_options)
    add_executable(analyser main.cpp src/cmd_options.cpp)
    target_link_libraries(analyser PRIVATE analyser_core Boost::program_options)
endif()

if(BUILD_TESTING)
    enable_testing()

    # An archive and its hash, not a git clone: nothing here can ask for a credential on the runner.
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz
        URL_HASH SHA256=ad7fdba11ea011c1d925b3289cf4af2c66a352e18d4c7264392fead75e919363
        SYSTEM
    )
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(accumulator_tests
        src/metric_accumulator_impl/tests/average_accumulator.cpp
        src/metric_accumulator_impl/tests/categorical_accumulator.cpp
        src/metric_accumulator_impl/tests/sum_average_accumulator.cpp
        src/metric_accumulator_impl/tests/headers.cpp
    )
    target_link_libraries(accumulator_tests PRIVATE analyser_core GTest::gtest_main)
    warn_as_errors(accumulator_tests)
    add_test(NAME accumulator_tests COMMAND accumulator_tests)

    # These run the tree-sitter CLI on the Python files beside them, and the lab machine has no
    # tree-sitter. They are built and linted, and they stay out of ctest: `recipe.ps1 run` starts them.
    add_executable(metric_tests
        src/metric_impl/tests/code_lines_count.cpp
        src/metric_impl/tests/cyclomatic_complexity.cpp
        src/metric_impl/tests/naming_style.cpp
        src/metric_impl/tests/parameters_count.cpp
    )
    target_link_libraries(metric_tests PRIVATE analyser_core GTest::gtest_main)
    warn_as_errors(metric_tests)
    file(GLOB metric_test_files "${CMAKE_SOURCE_DIR}/src/metric_impl/tests/files/*.py")
    foreach(metric_test_file IN LISTS metric_test_files)
        get_filename_component(metric_test_file_name ${metric_test_file} NAME)
        configure_file(${metric_test_file} "${CMAKE_CURRENT_BINARY_DIR}/${metric_test_file_name}" COPYONLY)
    endforeach()

    # The manual list is judged where every other file is judged (section 9).
    add_test(
        NAME manual_list
        COMMAND python -m pytest tests/test_manual_list.py -q -p no:cacheprovider
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
