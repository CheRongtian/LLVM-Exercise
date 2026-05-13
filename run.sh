cmake -S . -B build -DLLVM_DIR=../llvm-project/build/lib/cmake/llvm
cmake --build build
./build/KaleidoscopeExercise "$@"