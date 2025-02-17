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
    INCLUDECMD,
    CHAREXPR,
    STRINGLITEXPR,
    LETSTMT,
    PEEKSTMT,
    PRINTEXPR
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
public:
    long value;
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
public:
    std::string name;
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

class PrintExpr : public Expr
{
public:
    ~PrintExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class OpExpr : public Expr
{
public:
    std::string op;
    OpExpr(std::string);
    ~OpExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class LetExpr : public Expr
{
public:
    std::vector<std::string> idents;
    std::vector<Expr*> body;
    LetExpr(std::vector<std::string>, std::vector<Expr*>);
    ~LetExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};  

class PeekExpr : public Expr
{
public:
    std::vector<std::string> idents;
    std::vector<Expr*> body;
    PeekExpr(std::vector<std::string>, std::vector<Expr*>);
    ~PeekExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
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
    FnSignature sig;
public:
    std::vector<Expr*> body;    
    std::string name;           
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

class IncludeCmd : public Cmd
{
public:
    std::string path;
    IncludeCmd(std::string);
    ~IncludeCmd();
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