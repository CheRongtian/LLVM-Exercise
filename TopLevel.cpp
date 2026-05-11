#include "AST.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "TopLevel.hpp"
#include "codegen.hpp"

void InitializeModule()
{
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("My cool jit", *TheContext);
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

void HandleDefinition()
{
    if(auto FnAST = ParseDefinition()) 
    {
        if(auto *FnIR = FnAST->codegen())
        {
            fprintf(stderr, "Parsed a function defition.");
            FnIR->print(errs());
            fprintf(stderr, "\n");
        }  
    }
    else getNextToken();
}

void HandleExtern()
{
    if(auto ProtoAST = ParseExtern())
    {
        if(auto *FnIR = ProtoAST->codegen())
        {
            fprintf(stderr, "Read extern.");
            FnIR->print(errs());
            fprintf(stderr, "\n");
        }
    }
    else getNextToken();
}

void HandleTopLevelExpression()
{
    if(auto FnAST = ParseTopLevelExpr())
    {
        if(auto *FnIR = FnAST->codegen())
        {
            fprintf(stderr, "Read top-level expression:");
            FnIR->print(errs());
            fprintf(stderr, "\n");
            // remove an anonymous expression
            FnIR->eraseFromParent();
        }
    }
    else getNextToken();
}

// top ::= definition | external | expression | ';'
void MainLoop()
{
    while(true)
    {
        fprintf(stderr, "ready> ");
        switch(CurTok)
        {
            case tok_eof: return;
            case ';': getNextToken(); break;
            case tok_def: HandleDefinition(); break;
            case tok_extern: HandleExtern(); break;
            default: HandleTopLevelExpression(); break;
        }
    }    
}

