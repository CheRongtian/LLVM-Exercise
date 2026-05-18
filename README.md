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

### JIT Part

Similarly, you can get:
```
ready> def inc(x) x+1;
ready> Parsed a function defition.define double @inc(double %x) {
entry:
  %addtmp = fadd double %x, 1.000000e+00
  ret double %addtmp
}

ready> inc (2);   
ready> Evaluated to 3.000000
ready> extern sin(x);
ready> Read extern.declare double @sin(double)

ready> sin(1);
ready> Evaluated to 0.841471
ready> extern cos(x);
ready> Read extern.declare double @cos(double)

ready> cos(1);
ready> Evaluated to 0.540302
ready> def foo(x) sin(x)*sin(x)+cos(x)*cos(x);
ready> Parsed a function defition.define double @foo(double %x) {
entry:
  %calltmp = call double @sin(double %x)
  %calltmp1 = call double @sin(double %x)
  %multmp = fmul double %calltmp, %calltmp1
  %calltmp2 = call double @cos(double %x)
  %calltmp3 = call double @cos(double %x)
  %multmp4 = fmul double %calltmp2, %calltmp3
  %addtmp = fadd double %multmp, %multmp4
  ret double %addtmp
}

ready> foo(4);
ready> Evaluated to 1.000000
```
```
ready> extern putchard(char);
ready> Read extern.declare double @putchard(double)

ready> putchard(45);
ready> -Evaluated to 0.000000
ready> putchard(65);
ready> AEvaluated to 0.000000
ready> putchard(75);
ready> KEvaluated to 0.000000
```

The process:
```
lexer -> parser -> AST -> IR -> PASS -> JIT Machine Code
```
has been completed.

### If

```
ready> def fib(x) if x<3 then 1 else fib(x-1) + fib(x-2);
ready> Parsed a function defition.define double @fib(double %x) {
entry:
  %cmptmp = fcmp ult double %x, 3.000000e+00
  br i1 %cmptmp, label %ifcont, label %else

else:                                             ; preds = %entry
  %subtmp = fadd double %x, -1.000000e+00
  %calltmp = call double @fib(double %subtmp)
  %subtmp1 = fadd double %x, -2.000000e+00
  %calltmp2 = call double @fib(double %subtmp1)
  %addtmp = fadd double %calltmp, %calltmp2
  br label %ifcont

ifcont:                                           ; preds = %entry, %else
  %iftmp = phi double [ %addtmp, %else ], [ 1.000000e+00, %entry ]
  ret double %iftmp
}

ready> fib(1);
ready> Evaluated to 1.000000
ready> fib(2);
ready> Evaluated to 1.000000
ready> fib(3);
ready> Evaluated to 2.000000
ready> fib(4);
ready> Evaluated to 3.000000
ready> fib(5);
ready> Evaluated to 5.000000
```

### Loop
```
ready> extern putchard(char);
ready> Read extern.declare double @putchard(double)

ready> def printstar(n) for i=1, i<n,1.0 in putchard(42);
ready> Parsed a function defition.define double @printstar(double %n) {
entry:
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i = phi double [ 1.000000e+00, %entry ], [ %nextvar, %loop ]
  %calltmp = call double @putchard(double 4.200000e+01)
  %nextvar = fadd double %i, 1.000000e+00
  %cmptmp = fcmp ult double %i, %n
  br i1 %cmptmp, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  ret double 0.000000e+00
}

ready> printstar(36);
ready> ************************************Evaluated to 0.000000
ready> extern printd(val);
ready> Read extern.declare double @printd(double)

ready> def fib(x) if x<3 then 1 else fib(x-1) + fib(x-2);
ready> Parsed a function defition.define double @fib(double %x) {
entry:
  %cmptmp = fcmp ult double %x, 3.000000e+00
  br i1 %cmptmp, label %ifcont, label %else

else:                                             ; preds = %entry
  %subtmp = fadd double %x, -1.000000e+00
  %calltmp = call double @fib(double %subtmp)
  %subtmp1 = fadd double %x, -2.000000e+00
  %calltmp2 = call double @fib(double %subtmp1)
  %addtmp = fadd double %calltmp, %calltmp2
  br label %ifcont

ifcont:                                           ; preds = %entry, %else
  %iftmp = phi double [ %addtmp, %else ], [ 1.000000e+00, %entry ]
  ret double %iftmp
}

ready> def printfib(n) for i=1, i<n+1 in printd(fib(i));
ready> Parsed a function defition.define double @printfib(double %n) {
entry:
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i = phi double [ 1.000000e+00, %entry ], [ %nextvar, %loop ]
  %calltmp = call double @fib(double %i)
  %calltmp1 = call double @printd(double %calltmp)
  %nextvar = fadd double %i, 1.000000e+00
  %addtmp = fadd double %n, 1.000000e+00
  %cmptmp = fcmp ult double %i, %addtmp
  br i1 %cmptmp, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  ret double 0.000000e+00
}

ready> printfib(10);
ready> 1.000000
1.000000
2.000000
3.000000
5.000000
8.000000
13.000000
21.000000
34.000000
55.000000
89.000000
Evaluated to 0.000000
```

### Self-Defined Operator

run code like:
```bash
./run.sh Mandelbrot.kale
```

You can get like:

```
ready> ready> Parsed a function defition.define double @"unary!"(double %v) {
entry:
  %ifcond = fcmp ueq double %v, 0.000000e+00
  %. = select i1 %ifcond, double 1.000000e+00, double 0.000000e+00
  ret double %.
}

ready> ready> Parsed a function defition.define double @unary-(double %v) {
entry:
  %subtmp = fsub double 0.000000e+00, %v
  ret double %subtmp
}

ready> ready> Parsed a function defition.define double @"binary>"(double %LHS, double %RHS) {
entry:
  %cmptmp = fcmp ult double %RHS, %LHS
  %booltmp = uitofp i1 %cmptmp to double
  ret double %booltmp
}

ready> ready> Parsed a function defition.define double @"binary|"(double %LHS, double %RHS) {
entry:
  %ifcond = fcmp ueq double %LHS, 0.000000e+00
  %ifcond1 = fcmp ueq double %RHS, 0.000000e+00
  %. = select i1 %ifcond1, double 0.000000e+00, double 1.000000e+00
  %iftmp5 = select i1 %ifcond, double %., double 1.000000e+00
  ret double %iftmp5
}

ready> ready> Parsed a function defition.define double @"binary&"(double %LHS, double %RHS) {
entry:
  %unop = call double @"unary!"(double %LHS)
  %ifcond = fcmp ueq double %unop, 0.000000e+00
  br i1 %ifcond, label %else, label %ifcont

else:                                             ; preds = %entry
  %unop1 = call double @"unary!"(double %RHS)
  %unop2 = call double @"unary!"(double %unop1)
  br label %ifcont

ifcont:                                           ; preds = %entry, %else
  %iftmp = phi double [ %unop2, %else ], [ 0.000000e+00, %entry ]
  ret double %iftmp
}

ready> ready> Parsed a function defition.define double @"binary="(double %LHS, double %RHS) {
entry:
  %cmptmp = fcmp ult double %LHS, %RHS
  %booltmp = uitofp i1 %cmptmp to double
  %binop = call double @"binary>"(double %LHS, double %RHS)
  %binop1 = call double @"binary|"(double %booltmp, double %binop)
  %unop = call double @"unary!"(double %binop1)
  ret double %unop
}

ready> ready> Parsed a function defition.define double @"binary:"(double %x, double %y) {
entry:
  ret double %y
}

ready> ready> Read extern.declare double @putchard(double)

ready> ready> Parsed a function defition.define double @printdensity(double %d) {
entry:
  %binop = call double @"binary>"(double %d, double 8.000000e+00)
  %ifcond = fcmp ueq double %binop, 0.000000e+00
  br i1 %ifcond, label %else, label %then

then:                                             ; preds = %entry
  %calltmp = call double @putchard(double 3.200000e+01)
  br label %ifcont14

else:                                             ; preds = %entry
  %binop1 = call double @"binary>"(double %d, double 4.000000e+00)
  %ifcond2 = fcmp ueq double %binop1, 0.000000e+00
  br i1 %ifcond2, label %else5, label %then3

then3:                                            ; preds = %else
  %calltmp4 = call double @putchard(double 4.600000e+01)
  br label %ifcont14

else5:                                            ; preds = %else
  %binop6 = call double @"binary>"(double %d, double 2.000000e+00)
  %ifcond7 = fcmp ueq double %binop6, 0.000000e+00
  br i1 %ifcond7, label %else10, label %then8

then8:                                            ; preds = %else5
  %calltmp9 = call double @putchard(double 4.300000e+01)
  br label %ifcont14

else10:                                           ; preds = %else5
  %calltmp11 = call double @putchard(double 4.200000e+01)
  br label %ifcont14

ifcont14:                                         ; preds = %then3, %else10, %then8, %then
  %iftmp15 = phi double [ %calltmp, %then ], [ %calltmp4, %then3 ], [ %calltmp9, %then8 ], [ %calltmp11, %else10 ]
  ret double %iftmp15
}

ready> ready> Parsed a function defition.define double @mandelconverger(double %real, double %imag, double %iters, double %creal, double %cimag) {
entry:
  %binop = call double @"binary>"(double %iters, double 2.550000e+02)
  %multmp = fmul double %real, %real
  %multmp1 = fmul double %imag, %imag
  %addtmp = fadd double %multmp, %multmp1
  %binop2 = call double @"binary>"(double %addtmp, double 4.000000e+00)
  %binop3 = call double @"binary|"(double %binop, double %binop2)
  %ifcond = fcmp ueq double %binop3, 0.000000e+00
  br i1 %ifcond, label %else, label %ifcont

else:                                             ; preds = %entry
  %subtmp = fsub double %multmp, %multmp1
  %addtmp6 = fadd double %subtmp, %creal
  %multmp7 = fmul double %real, 2.000000e+00
  %multmp8 = fmul double %multmp7, %imag
  %addtmp9 = fadd double %multmp8, %cimag
  %addtmp10 = fadd double %iters, 1.000000e+00
  %calltmp = call double @mandelconverger(double %addtmp6, double %addtmp9, double %addtmp10, double %creal, double %cimag)
  br label %ifcont

ifcont:                                           ; preds = %entry, %else
  %iftmp = phi double [ %calltmp, %else ], [ %iters, %entry ]
  ret double %iftmp
}

ready> ready> Parsed a function defition.define double @mandelconverge(double %real, double %imag) {
entry:
  %calltmp = call double @mandelconverger(double %real, double %imag, double 0.000000e+00, double %real, double %imag)
  ret double %calltmp
}

ready> ready> Parsed a function defition.define double @mandelhelp(double %xmin, double %xmax, double %xstep, double %ymin, double %ymax, double %ystep) {
entry:
  br label %loop

loop:                                             ; preds = %afterloop, %entry
  %y = phi double [ %ymin, %entry ], [ %nextvar4, %afterloop ]
  br label %loop1

loop1:                                            ; preds = %loop1, %loop
  %x = phi double [ %xmin, %loop ], [ %nextvar, %loop1 ]
  %calltmp = call double @mandelconverge(double %x, double %y)
  %calltmp2 = call double @printdensity(double %calltmp)
  %nextvar = fadd double %xstep, %x
  %cmptmp = fcmp ult double %x, %xmax
  br i1 %cmptmp, label %loop1, label %afterloop

afterloop:                                        ; preds = %loop1
  %calltmp3 = call double @putchard(double 1.000000e+01)
  %binop = call double @"binary:"(double 0.000000e+00, double %calltmp3)
  %nextvar4 = fadd double %ystep, %y
  %cmptmp5 = fcmp ult double %y, %ymax
  br i1 %cmptmp5, label %loop, label %afterloop8

afterloop8:                                       ; preds = %afterloop
  ret double 0.000000e+00
}

ready> Parsed a function defition.define double @mandel(double %realstart, double %imagstart, double %realmag, double %imagmag) {
entry:
  %multmp = fmul double %realmag, 7.800000e+01
  %addtmp = fadd double %realstart, %multmp
  %multmp1 = fmul double %imagmag, 4.000000e+01
  %addtmp2 = fadd double %imagstart, %multmp1
  %calltmp = call double @mandelhelp(double %realstart, double %addtmp, double %realmag, double %imagstart, double %addtmp2, double %imagmag)
  ret double %calltmp
}

ready> ready> *******************************************************************************
*******************************************************************************
****************************************++++++*********************************
************************************+++++...++++++*****************************
*********************************++++++++.. ...+++++***************************
*******************************++++++++++..   ..+++++**************************
******************************++++++++++.     ..++++++*************************
****************************+++++++++....      ..++++++************************
**************************++++++++.......      .....++++***********************
*************************++++++++.   .            ... .++**********************
***********************++++++++...                     ++**********************
*********************+++++++++....                    .+++*********************
******************+++..+++++....                      ..+++********************
**************++++++. ..........                        +++********************
***********++++++++..        ..                         .++********************
*********++++++++++...                                 .++++*******************
********++++++++++..                                   .++++*******************
*******++++++.....                                    ..++++*******************
*******+........                                     ...++++*******************
*******+... ....                                     ...++++*******************
*******+++++......                                    ..++++*******************
*******++++++++++...                                   .++++*******************
*********++++++++++...                                  ++++*******************
**********+++++++++..        ..                        ..++********************
*************++++++.. ..........                        +++********************
******************+++...+++.....                      ..+++********************
*********************+++++++++....                    ..++*********************
***********************++++++++...                     +++*********************
*************************+++++++..   .            ... .++**********************
**************************++++++++.......      ......+++***********************
****************************+++++++++....      ..++++++************************
*****************************++++++++++..     ..++++++*************************
*******************************++++++++++..  ...+++++**************************
*********************************++++++++.. ...+++++***************************
***********************************++++++....+++++*****************************
***************************************++++++++********************************
*******************************************************************************
*******************************************************************************
*******************************************************************************
*******************************************************************************
*******************************************************************************
Evaluated to 0.000000
ready> ready> ; ModuleID = 'Kaleidoscope'
source_filename = "Kaleidoscope"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
```

In the Kale code part, the last line:
```kale
mandel(-2.3, -1.3, 0.05, 0.07);
```
can set the parameter for the mandelbrot graph.