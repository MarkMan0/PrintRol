

add_library(LineFilter "src/LineFilter.cpp")
target_include_directories(LineFilter PUBLIC "include")

add_executable(LineFilterTest "test/LineFilterTest.cpp")
target_link_libraries(LineFilterTest PUBLIC GTest::gtest_main LineFilter)
gtest_discover_tests(LineFilterTest)
