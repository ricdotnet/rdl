cmake_minimum_required(VERSION 3.15)
project(RDL VERSION 0.1 LANGUAGES CXX)

# Match your Makefile flags (-std=c++20 -Wall -Wextra -g)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce debug symbols (-g) when building in Debug mode
set(CMAKE_BUILD_TYPE Debug)

# Match your Makefile's wildcard strategy safely
file(GLOB_RECURSE SRC_FILES
        "main.cpp"
        "src/*.cpp"
)

# Match your TARGET = language
add_executable(language ${SRC_FILES})

# Match your Makefile warning flags (-Wall -Wextra)
if (NOT MSVC)
    target_compile_options(language PRIVATE -Wall -Wextra)
endif ()