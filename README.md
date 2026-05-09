After git clone llvm-project:
```bash
# build
rm -rf build/*
cmake -S llvm -B build -G Ninja -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release
cmake --build build
# test
cmake --build build --target check-all
# install (optional)
cmake --install build
```
Then you can get:
```css
Projects/
├── llvm-project/               
│   ├── build/                  
│   └── llvm/
│
└── KaleidoscopeExercise/                 
    ├── Environment Test/ # put these files into a folder to do the environment test
    │   ├── CMakeLists.txt   
    │   ├── test.sh       
    │   └── test.cpp            
    ├── AST.cpp
    └── ...         
```
For the Environment Test/
do:
```bash
cd projects
cd KaleidoscopeExercise
cd "Environment Test"
chmod +x test.sh
./test.sh
```

you should see the output like:
```
LLVM Context created successfully! Environment is ready.
```

For the rest components:
do
```bash
cd projects
cd KaleidoscopeExercise
mkdir build
cd build
cmake ..
make
./KaleidoscopeExercise
```
and you can get output like:
```
ready> def foo(x y) x+y y
ready> Parsed a function defition.
ready> def foo(x y) x+y );
Parsed a top-level expr.
ready> Parsed a function defition.
ready> Error: unknown token when expecting an expression
ready> extern sin(a);
ready> Parsed an extern.
```