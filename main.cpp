#include "lexer.hpp"
#include "AST.hpp"
#include "parser.hpp"
#include "TopLevel.hpp"
#include "codegen.hpp"

int main()
{
    /*
    Just for lexer part:
    int Tok;
    do
    {
        Tok = gettok();
        std::cout << Tok << std::endl;
    }while(Tok!=tok_eof);
    return 0;
    */
   BinopPrecedence['<'] = 10;
   BinopPrecedence['+'] = 20;
   BinopPrecedence['-'] = 30;
   BinopPrecedence['*'] = 40;

   fprintf(stderr, "ready> ");
   getNextToken();

   InitializeModuleAndManagers();
   
   MainLoop();
   
   TheModule->print(errs(), nullptr);

   return 0; 
}