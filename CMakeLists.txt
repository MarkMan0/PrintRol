cmake_minimum_required(VERSION 3.16.0)


project(PrintRol VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DUNICODE -D_UNICODE)

if (WIN32)
    add_definitions(-DWIN32)
elseif (UNIX)
    add_definitions(-DUNIX)
endif()

# from QT
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# needed for not, or, and keywords
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/permissive-")
endif()

add_subdirectory("printrol")
