#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

class Parser
{
    int index;  
    std::vector<Token> input;
public:
    Parser(std::vector<Token>);
    std::vector<AST*> parse();
    void cleanup(std::vector<AST*>);
    Token peek();
    Token pop();
    Token ahead();
    Token behind();
    AST parseWhile();
    AST parseIf();
    ConstCmd *parseConst();
    AST parseProc();
    AST parseInclude();
    Expr *parseExpr();
};

#endif // PARSER_H