

add_library(PrinterMonitor STATIC "src/PrinterMonitor.cpp")
target_include_directories(PrinterMonitor PUBLIC "include")

add_executable(PrinterMonitorTest "test/PrintMonTest.cpp")
target_link_libraries(PrinterMonitorTest PUBLIC GTest::gtest_main PrinterMonitor)
gtest_discover_tests(PrinterMonitorTest)
