cmake_minimum_required(VERSION 3.20)

project(GaleShapley LANGUAGES CXX)

# Define the executable and its source files.
add_executable(gale_shapley_tests
    src/StableMatching.cpp
    tests/testGS.cpp
)

# Require C++17 and disable compiler-specific language extensions.
set_target_properties(gale_shapley_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Equivalent to -Iinclude.
target_include_directories(gale_shapley_tests PRIVATE
    include
)

# Your existing warning flags, for GCC or Clang.
target_compile_options(gale_shapley_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Register the executable as a test.
enable_testing()
add_test(NAME gale_shapley_tests COMMAND gale_shapley_tests)