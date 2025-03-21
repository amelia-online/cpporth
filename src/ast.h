#ifndef CPPORTH_AST_H
#define CPPORTH_AST_H

#include <string>
#include <unordered_map>

enum class TypeKind
{
    INT,
    BOOL,
    PTR,
    ADDR,
    VARIANT,
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
    PRINTEXPR,
    MEMORYEXPR,
    OFFSETEXPR,
    RESETEXPR,
    SWAPEXPR,
    DROPEXPR,
    DUPEXPR,
    OVEREXPR,
    ROTEXPR,
    HEREEXPR,
    SYSCALLEXPR,
    MAXEXPR,
    ASSERTEXPR,
    ADDROFEXPR,
    ASSERTCMD,
    CALLLIKEEXPR,
    ALLOCSTMT,
    FREEEXPR,
    TYPECMD,
    MATCHSTMT,
    VARIANTINSTANCEEXPR,
    VARIANTBINDING,
    ARRAYLITEXPR
};

class AST 
{ 
public:
    int line;
    virtual ~AST() = 0;
    virtual std::string toString() = 0;
    virtual ASTKind getASTKind() = 0;
};

class Cmd : public AST
{
public:
    virtual ~Cmd() = 0;
    //virtual std::string toString() = 0;
};


class Type
{  
public:
    Type(TypeKind);
    TypeKind kind;
    std::string toString();
};

class VariantType : public Type
{
public:
    std::string name;
    VariantType(std::string);
    std::string toString();
};

class Expr : public AST 
{
public:
    virtual ~Expr() = 0;
    virtual std::string toString() = 0;
    virtual ASTKind getASTKind() = 0;
};

class AssertCmd : public Cmd
{
public:
    std::string msg;
    std::vector<Expr*> body;
    AssertCmd(std::string, std::vector<Expr*>);
    ~AssertCmd();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class Field
{   
public:
    std::string name;
    Type type;
    Field(std::string, Type);
    std::string toString();
};

class Variant
{
public:
    std::string name;
    std::string parentName;
    std::vector<Field> fields;
    Variant(std::string, std::string, std::vector<Field>);
    std::string toString();
};

class TypeCmd : public Cmd
{
public:
    std::string name;
    std::vector<Variant> variants;
    TypeCmd(std::string, std::vector<Variant>);
    ~TypeCmd();
    std::string toString() override;
    ASTKind getASTKind() override;
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
    bool cstr;
public:
    StringLitExpr(std::string, bool);
    std::string getValue();
    std::string toString() override;
    bool isCStr() const;
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

class AllocExpr: public Expr
{
public:
    AllocExpr();
    ~AllocExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class FreeExpr: public Expr
{
public:
    FreeExpr();
    ~FreeExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class VariantInstanceExpr : public Expr
{
public:
    std::string parent;
    std::string variant;
    std::vector<std::vector<Expr *> > args;
    VariantInstanceExpr(std::string, std::string, std::vector<std::vector<Expr *> >);
    ~VariantInstanceExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class VariantBinding : public Expr
{
public:
    std::vector<std::string> idents;
    std::vector<Expr *> body;
    VariantBinding(std::vector<std::string>, std::vector<Expr*>);
    ~VariantBinding();
    std::string toString() override;
    ASTKind getASTKind() override;
};  

class MatchExpr : public Expr
{
public:
    std::unordered_map<std::string, VariantBinding*> branches;
    std::string supertype;
    MatchExpr(std::unordered_map<std::string, VariantBinding*>, std::string);
    ~MatchExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class ArrayLitExpr : public Expr
{
public:
    std::vector<std::vector<Expr*> > items;
    ArrayLitExpr(std::vector<std::vector<Expr*> >);
    ~ArrayLitExpr();
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

class MemoryExpr : public Expr
{
    std::string ident;
public:
    std::vector<Expr*> body;
    MemoryExpr(std::string, std::vector<Expr*>);
    MemoryExpr(MemoryExpr *);
    ~MemoryExpr();
    std::string getIdent();
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

class AssertExpr : public Expr
{
public:
    std::string msg;
    std::vector<Expr*> body;
    AssertExpr(std::string, std::vector<Expr*>);
    ~AssertExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
    AssertCmd *asCmd();
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

class OffsetExpr : public Expr
{
public:
    OffsetExpr();
    ~OffsetExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class ResetExpr : public Expr
{
public:
    ResetExpr();
    ~ResetExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class AddrOfExpr : public Expr
{
public:
    VarExpr *proc;
    AddrOfExpr(VarExpr *);
    ~AddrOfExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class CallLikeExpr : public Expr
{
public:
    VarExpr *proc;
    CallLikeExpr(VarExpr *);
    ~CallLikeExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class SwapExpr : public Expr
{
public:
    SwapExpr();
    ~SwapExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class DropExpr : public Expr
{
public:
    DropExpr();
    ~DropExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class DupExpr : public Expr
{
public:
    DupExpr();
    ~DupExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class OverExpr : public Expr
{
public:
    OverExpr();
    ~OverExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class RotExpr : public Expr
{
public:
    RotExpr();
    ~RotExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class HereExpr : public Expr
{
public:
    HereExpr();
    ~HereExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class SyscallExpr : public Expr
{
    int n;
public:
    SyscallExpr(int);
    ~SyscallExpr();
    int getNumArgs();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class MaxExpr : public Expr
{
public:
    MaxExpr();
    ~MaxExpr();
    std::string toString() override;
    ASTKind getASTKind() override;
};

class Stmt : public AST 
{
public:
    virtual ~Stmt() = 0;
};

class FnSignature
{
public:
    std::vector<Type> params;
    std::vector<Type> retTypes;
    FnSignature(std::vector<Type>, std::vector<Type>);
    std::string toString();
};

class ProcCmd : public Cmd
{
public:
    FnSignature sig;
    std::vector<Expr*> body;    
    std::string name;           
    ProcCmd(std::string, FnSignature, std::vector<Expr*>);
    ~ProcCmd() override;
    std::string toString() override;
    ASTKind getASTKind() override;
};

class ConstCmd : public Cmd
{
public:
    std::string ident;
    std::vector<Expr *> body;
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
public:
    std::string ident;
    std::vector<Expr *> body;
    MemoryCmd(std::string, std::vector<Expr *>);
    ~MemoryCmd() override;
    std::string toString() override;
    ASTKind getASTKind() override;
    MemoryExpr *toMemoryExpr();
};





#endif // CPPORTH_AST_H