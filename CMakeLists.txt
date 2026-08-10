cmake_minimum_required(VERSION 3.25)
project(analyser VERSION 1.0.0 LANGUAGES CXX)

# MSVC has no separate C++26 switch: its newest mode is /std:c++latest, which
# CMake exposes as CXX23. Asking for 26 makes CMake refuse to generate at all,
# so on MSVC we ask for the newest dialect it does have. GCC/Clang keep 26.
if(MSVC)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 26)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure local githooks
execute_process(
    COMMAND git config core.hooksPath .githooks
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_COMMAND_RESULT
)

find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(range-v3 REQUIRED) 

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

set(target analyser)

add_executable(${target} main.cpp)
target_include_directories(${target} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${target}
    PRIVATE
        metric_accumulator
        metric
        cmd_options
        range-v3::range-v3
)

include(CTest)
enable_testing()

add_subdirectory(src)