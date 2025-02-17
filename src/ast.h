#ifndef CPPORTH_AST_H
#define CPPORTH_AST_H

#include <string>

enum class TypeKind
{
    INT,
    BOOL,
    PTR,
    ADDR
};

enum class ASTKind
{
    INTEXPR,
    TRUEEXPR,
    FALSEEXPR,
    OPEXPR,
    VAREXPR,
    WHILEEXPR,
    IFEXPR,
    PROCCMD,
    CONSTCMD,
    MEMORYCMD,
    CHAREXPR,
    STRINGLITEXPR,
    LETSTMT,
    PEEKSTMT,
};

class AST 
{ 
public:
    virtual ~AST() = 0;
    virtual std::string toString() = 0;
    virtual ASTKind getASTKind() = 0;
};

class Type
{  
public:
    Type(TypeKind);
    TypeKind kind;
    std::string toString();
};

class Expr : public AST 
{
public:
    virtual ~Expr() = 0;
    virtual std::string toString() = 0;
    virtual ASTKind getASTKind() = 0;
};

class IntExpr : public Expr
{
    long value;
public:
    IntExpr(long);
    long getValue();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class TrueExpr : public Expr
{
public:
    std::string toString() override;
    ASTKind getASTKind() override;
};

class FalseExpr : public Expr
{
public:
    std::string toString() override;
    ASTKind getASTKind() override;
};

class CharExpr : public Expr
{
    char ch;
public:
    CharExpr(char);
    char getValue();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class StringLitExpr : public Expr
{
    std::string value;
public:
    StringLitExpr(std::string);
    std::string getValue();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class VarExpr : public Expr
{
    std::string name;
public:
    VarExpr(std::string);
    std::string getName();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class IfExpr: public Expr
{
public:
    std::vector<Expr*> then;
    std::vector<Expr*> elze;
    IfExpr *next;
    IfExpr(std::vector<Expr*>, std::vector<Expr*>, IfExpr *);
    ~IfExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class WhileExpr : public Expr
{
public:
    std::vector<Expr*> cond;
    std::vector<Expr*> body;
    WhileExpr(std::vector<Expr*>, std::vector<Expr*>);
    ~WhileExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class LetExpr : public Expr
{
    // TODO
};  

class PeekExpr : public Expr
{
    // TODO
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
};

class FnSignature
{
    std::vector<Type> params;
    std::vector<Type> retTypes;
public:
    FnSignature(std::vector<Type>, std::vector<Type>);
    std::string toString();
};

class ProcCmd : public Cmd
{
    std::string name;
    FnSignature sig;
    std::vector<Expr*> body;
public:
    ProcCmd(std::string, FnSignature, std::vector<Expr*>);
    ~ProcCmd() override;
    std::string toString() override;
    ASTKind getASTKind() override;
};

class ConstCmd : public Cmd
{
    std::string ident;
    std::vector<Expr *> body;
public:
    ~ConstCmd() override;
    ConstCmd(std::string, std::vector<Expr *>);
    std::string toString() override;
    ASTKind getASTKind() override;
};

class MemoryCmd : public Cmd
{
  std::string ident;
  std::vector<Expr *> body;
public:
    MemoryCmd(std::string, std::vector<Expr *>);
    ~MemoryCmd() override;
    std::string toString() override;
    ASTKind getASTKind() override;
};




#endif // CPPORTH_AST_H