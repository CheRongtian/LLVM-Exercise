#include "AST.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;
std::map<std::string, Value*> NamedValues;

bool EnableOptimization = true;

std::unique_ptr<FunctionPassManager> TheFPM;
std::unique_ptr<LoopAnalysisManager> TheLAM;
std::unique_ptr<FunctionAnalysisManager> TheFAM;
std::unique_ptr<CGSCCAnalysisManager> TheCGAM;
std::unique_ptr<ModuleAnalysisManager> TheMAM;
std::unique_ptr<PassInstrumentationCallbacks> ThePIC;
std::unique_ptr<StandardInstrumentations> TheSI;

std::unique_ptr<KaleidoscopeJIT> TheJIT;
ExitOnError ExitOnErr;
std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

Value *LogErrorV(const char *Str)
{
    LogError(Str);
    return nullptr;
}

Function *getFunction(std::string Name)
{
    if(auto *F = TheModule->getFunction(Name)) return F;
    auto FI = FunctionProtos.find(Name);
    if(FI != FunctionProtos.end()) return FI->second->codegen();
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
    Function *CalleeF = getFunction(Callee);
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

Value *IfExprAST::codegen()
{
    Value *CondV = Cond->codegen();
    if(!CondV) return nullptr;

    CondV = Builder->CreateFCmpONE(CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "ifcond");
    Function *TheFunction = Builder->GetInsertBlock()->getParent();

    BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
    BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "ifcont");

    Builder->CreateCondBr(CondV, ThenBB, ElseBB);

    Builder->SetInsertPoint(ThenBB);
    Value *ThenV = Then->codegen();
    if(!ThenV) return nullptr;

    Builder->CreateBr(MergeBB);
    ThenBB = Builder->GetInsertBlock();

    TheFunction->insert(TheFunction->end(), ElseBB);
    Builder->SetInsertPoint(ElseBB);
    Value *ElseV = Else->codegen();
    if(!ElseV) return nullptr;

    Builder->CreateBr(MergeBB);
    ElseBB = Builder->GetInsertBlock();

    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder->SetInsertPoint(MergeBB);
    PHINode *PN = Builder->CreatePHI(Type::getDoubleTy(*TheContext), 2, "iftmp");

    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
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
    auto &P = *Proto;
    FunctionProtos[Proto->getName()] = std::move(Proto);
    Function *TheFunction = getFunction(P.getName());

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

        if(EnableOptimization)
            TheFPM->run(*TheFunction, *TheFAM);
        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return nullptr;
}