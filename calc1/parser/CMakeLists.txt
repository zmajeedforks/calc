# calc1/parser/CMakeLists.txt

project(calc1_parser_bison)

# tests

# test full parser with real input
add_executable(calc1_parser.gtest calc1_parser.gtest.cpp)
target_link_libraries(calc1_parser.gtest flexbisonlib.calc1 gmock_main)

# test grammar with minimal parser and simulated input
add_executable(calc1_parser.minimal.gtest calc1_parser.minimal.gtest.cpp)
target_link_libraries(calc1_parser.minimal.gtest minimalbisonlib.calc1 gmock_main)

include(GoogleTest)
gtest_discover_tests(calc1_parser.minimal.gtest DISCOVERY_MODE PRE_TEST EXTRA_ARGS --gtest_color=yes)
gtest_discover_tests(calc1_parser.gtest DISCOVERY_MODE PRE_TEST EXTRA_ARGS --gtest_color=yes)
