cmake_minimum_required(VERSION 3.23.0)


project(PrintRol)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DUNICODE)

# needed for not, or, and keywords
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/permissive-")
endif()

add_subdirectory("printrol")
