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
    WhileExpr *parseWhile();
    IfExpr *parseIf();
    ConstCmd *parseConst();
    MemoryCmd *parseMemory();
    ProcCmd *parseProc();
    LetExpr *parseLet();
    PeekExpr *parsePeek();
    AssertExpr *parseAssert();
    AddrOfExpr *parseAddrOf();
    CallLikeExpr *parseCallLike();
    IncludeCmd *parseInclude();
    TypeCmd *parseTypeCmd();
    Variant parseVariant(std::string);
    Field parseField();
    VariantBinding *parseMatchBranch(std::unordered_map<std::string, VariantBinding*>&);
    ArrayLitExpr *parseArrayLit();
    MatchExpr *parseMatch();
    VariantInstanceExpr *parseNew();
    std::vector<Expr *> parseExpr();
    Type parseType();
    FnSignature parseSignature();
};

#endif // PARSER_H