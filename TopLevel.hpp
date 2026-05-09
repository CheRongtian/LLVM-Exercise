#ifndef TOPLEVEL_HPP
#define TOPLEVEL_HPP

#include "AST.hpp"
#include "lexer.hpp"
#include "parser.hpp"

void HandleDefinition();

void HandleExtern();

void HandleTopLevelExpression();

void MainLoop();

#endif