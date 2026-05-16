cmake_minimum_required(VERSION 3.17)
project(KaleidoscopeExercise)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs 
                                native
                                Core 
                                Support 
                                Passes 
                                InstCombine 
                                ScalarOpts 
                                TransformUtils 
                                Analysis 
                                ExecutionEngine 
                                OrcJIT 
                                Object 
                                )

list(REMOVE_DUPLICATES llvm_libs)

add_executable(KaleidoscopeExercise
                AST.cpp
                lexer.cpp
                main.cpp
                parser.cpp
                TopLevel.cpp
                codegen.cpp
                JITLib.cpp
                AST.hpp
                parser.hpp
                lexer.hpp
                TopLevel.hpp
                codegen.hpp
                JITLib.hpp
                KaleidoscopeJIT.hpp
                )

target_link_libraries(KaleidoscopeExercise ${llvm_libs})

target_link_options(KaleidoscopeExercise PRIVATE "-Wl,-no_warn_duplicate_libraries")