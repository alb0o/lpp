cmake_minimum_required(VERSION 3.15)
project(LPP VERSION 0.1.0 LANGUAGES CXX)

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
)

# Tests (optional, can be added later)
# enable_testing()
# add_subdirectory(tests)

# Install
install(TARGETS lppc DESTINATION bin)
