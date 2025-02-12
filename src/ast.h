#ifndef CPPORTH_AST_H
#define CPPORTH_AST_H

#include <string>

enum class SubexprType
{
    OPERATOR,
    INTEGER,
    BOOLEAN,
};

class AST 
{ 
public:
    virtual std::string toString() = 0;
};

class Subexpr
{
public:
    Subexpr(int);
    Subexpr(bool);
    Subexpr(std::string);
    int num;
    bool boolean;
    std::string op;
    SubexprType type;
    std::string toString();
};

class Expr : public AST 
{
    std::vector<Subexpr> subexprs;
public:
    ~Expr();
    Expr(std::vector<Subexpr>);
    virtual std::string toString();
};

class Cmd : public AST
{
};

class Stmt : public AST 
{
};

class ConstCmd : public Cmd
{
    std::string ident;
    Expr *expression;
public:
    ~ConstCmd();
    ConstCmd(std::string, Expr *);
    virtual std::string toString();
};




#endif // CPPORTH_AST_H