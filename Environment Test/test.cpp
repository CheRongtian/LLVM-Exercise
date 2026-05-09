#include "llvm/IR/LLVMContext.h"
#include <iostream>

int main() {
    llvm::LLVMContext context;
    std::cout << "LLVM Context created successfully! Environment is ready." << std::endl;
    return 0;
}