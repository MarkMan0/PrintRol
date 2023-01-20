

if (WIN32)
    add_library(WinSerial STATIC "src/WinSerial.cpp" "src/ComList.cpp")
    target_include_directories(WinSerial PUBLIC "include")
    target_link_libraries(WinSerial PUBLIC ISerial)
endif()
