cmake_minimum_required(VERSION 3.10)
project(LPP VERSION 0.8.14 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Compiler executable
add_executable(lppc
    src/main.cpp
    src/Token.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/AST.cpp
    src/Transpiler.cpp
    src/StaticAnalyzer.cpp
    src/ModuleResolver.cpp
    src/DocGenerator.cpp
    src/SourceMap.cpp
    src/MacroExpander.cpp
    src/FFI.cpp
    src/Optimizer.cpp
    src/Benchmark.cpp
    src/PackageManager.cpp
)

# REPL executable
add_executable(lpprepl
    src/repl.cpp
    src/Token.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/AST.cpp
    src/Transpiler.cpp
    src/StaticAnalyzer.cpp
)

# Tests (optional, can be added later)
# enable_testing()
# add_subdirectory(tests)

# Install
install(TARGETS lppc DESTINATION bin)
