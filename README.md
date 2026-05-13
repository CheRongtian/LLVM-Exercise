# LLVM Project
## Basic Preparation
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
### Structure
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
## Environment Test
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
## Customed Part
### For the only Frontend phase:
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

### IR Phase
```bash
cd projects
cd KaleidoscopeExercise
chmod +x run.sh
./run.sh
```

then you can do like:
```
ready> 4+5;
ready> Read top-level expression:define double @__anon_expr() {
entry:
  ret double 9.000000e+00
}

ready> def foo(a b) a*a + 2*a*b + b*b;
ready> Parsed a function defition.define double @foo(double %a, double %b) {
entry:
  %multmp = fmul double %a, %a
  %multmp1 = fmul double 2.000000e+00, %a
  %multmp2 = fmul double %multmp1, %b
  %addtmp = fadd double %multmp, %multmp2
  %multmp3 = fmul double %b, %b
  %addtmp4 = fadd double %addtmp, %multmp3
  ret double %addtmp4
}
```

### PASS Phase (Add a swicth here)
Similarly, you can get:
Here you can use command like this use turn on/off the optimization(PASS)
```bash
./run.sh # default: use PASS
# OR
./run.sh off # do not use PASS
```
#### Before PASS
```
Optimization disabled.
ready> def test(x) (1+2+x)*(x+(1+2));
ready> Parsed a function defition.define double @test(double %x) {
entry:
  %addtmp = fadd double 3.000000e+00, %x
  %addtmp1 = fadd double %x, 3.000000e+00
  %multmp = fmul double %addtmp, %addtmp1
  ret double %multmp
}
```

#### After PASS
```
ready> def test(x) (1+2+x)*(x+(1+2));
ready> Parsed a function defition.define double @test(double %x) {
entry:
  %addtmp = fadd double %x, 3.000000e+00
  %multmp = fmul double %addtmp, %addtmp
  ret double %multmp
}
```

You can see the difference here