

if (UNIX)

    add_library(LinuxSerial "src/LinuxSerial.cpp")
    target_include_directories(LinuxSerial PUBLIC "include")
    target_link_libraries(LinuxSerial PUBLIC ISerial)

endif()
