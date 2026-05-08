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
    ├── CMakeLists.txt          
    └── test.cpp                
```

do:
```bash
chmod +x test.sh
./test.sh
```

you should see the output like:
```
LLVM Context created successfully! Environment is ready.
```