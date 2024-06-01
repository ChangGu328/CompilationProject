cmake_minimum_required(VERSION 3.28)
project(CompilationProject)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        src/compiler.cpp
        src/mpg_analysis.cpp
        src/lex_analysis.cpp
)


add_executable(CompilationProject ${SOURCES})

target_include_directories(CompilationProject
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
