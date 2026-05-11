#include "AST.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;
std::map<std::string, Value*> NamedValues;

Value *LogErrorV(const char *Str)
{
    LogError(Str);
    return nullptr;
}

Value *NumberExprAST::codegen()
{
    return ConstantFP::get(*TheContext, APFloat(Val));
}

Value *VariableExprAST::codegen()
{
    Value *V = NamedValues[Name];
    if(!V) return LogErrorV("Unknown variable name");
    return V;
}

Value *BinaryExprAST::codegen()
{
    Value *L = LHS->codegen();
    Value *R = RHS->codegen();
    if(!L||!R) return nullptr;

    switch(Op)
    {
        case '+': return Builder->CreateFAdd(L, R, "addtmp");
        case '-': return Builder->CreateFSub(L, R, "subtmp");
        case '*': return Builder->CreateFMul(L, R, "multmp");
        case '<': 
            L = Builder->CreateFCmpULT(L, R, "cmptmp");
            return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
        default: return LogErrorV("Invalid binary operator");
    }
}

Value *CallExprAST::codegen()
{
    Function *CalleeF = TheModule->getFunction(Callee);
    if(!CalleeF) return LogErrorV("Unknown function referenced");

    if(CalleeF->arg_size()!=Args.size()) return LogErrorV("Incorrect # arguements passed");

    std::vector<Value *> ArgsV;
    for(unsigned i=0, e=Args.size(); i!=e; i++)
    {
        ArgsV.push_back(Args[i]->codegen());
        if(!ArgsV.back()) return nullptr;
    }

    return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen()
{
    std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*TheContext));
    FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

    unsigned Idx = 0;
    for(auto &Arg: F->args()) Arg.setName(Args[Idx++]);

    return F;
}

Function *FunctionAST::codegen()
{
    Function *TheFunction = TheModule->getFunction(Proto->getName());

    if(!TheFunction) TheFunction = Proto->codegen();
    if(!TheFunction) return nullptr;

    BasicBlock *BB = BasicBlock::Create(*TheContext,  "entry",  TheFunction);
    Builder->SetInsertPoint(BB);

    NamedValues.clear();
    for(auto &Arg:TheFunction->args()) NamedValues[std::string(Arg.getName())] = &Arg;
    if(Value *RetVal = Body->codegen())
    {
        Builder->CreateRet(RetVal);
        verifyFunction(*TheFunction);
        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return nullptr;
}