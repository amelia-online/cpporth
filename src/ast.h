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
    virtual ~AST() = 0;
    virtual std::string toString() = 0;
};

class Expr : public AST 
{
public:
    virtual ~Expr() = 0;
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

class AtomExpr : public Expr 
{
    std::vector<Subexpr> subexprs;
public:
    ~AtomExpr() override;
    AtomExpr(std::vector<Subexpr>);
    std::string toString() override;
};

class Cmd : public AST
{
public:
    virtual ~Cmd() = 0;
    //virtual std::string toString() = 0;
};

class Stmt : public AST 
{
public:
    virtual ~Stmt() = 0;
    //virtual std::string toString() = 0;
};

class ProcCmd : public Cmd
{
    std::string name;
    std::vector<AST*> body;
public:
    ProcCmd(std::string, std::vector<AST*>);
    ~ProcCmd() override;
    std::string toString() override;
};

class ConstCmd : public Cmd
{
    std::string ident;
    Expr *expression;
public:
    ~ConstCmd() override;
    ConstCmd(std::string, Expr *);
    std::string toString() override;
};

class MemoryCmd : public Cmd
{
  std::string ident;
  Expr *sizeExpr;
public:
    MemoryCmd(std::string, Expr *);
    ~MemoryCmd() override;
    std::string toString() override;
};




#endif // CPPORTH_AST_H