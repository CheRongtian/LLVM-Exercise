cmake_minimum_required(VERSION 3.17)
project(KaleidoscopeExercise)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(KaleidoscopeExercise
                AST.cpp
                lexer.cpp
                main.cpp
                parser.cpp
                TopLevel.cpp
                AST.hpp
                parser.hpp
                lexer.hpp
                TopLevel.hpp
                )