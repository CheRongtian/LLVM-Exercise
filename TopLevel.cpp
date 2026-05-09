#include "TopLevel.hpp"

void HandleDefinition()
{
    if(ParseDefinition()) fprintf(stderr, "Parsed a function defition.\n");
    else getNextToken();
}

void HandleExtern()
{
    if(ParseExtern()) fprintf(stderr, "Parsed an extern.\n");
    else getNextToken();
}

void HandleTopLevelExpression()
{
    if(ParseTopLevelExpr()) fprintf(stderr, "Parsed a top-level expr.\n");
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

