#include "ast.h"

Type::Type(TypeKind kind) : kind(kind) {;}
std::string Type::toString()
{
    switch (kind)
    {
        case TypeKind::INT:
            return "(IntType)";
        case TypeKind::BOOL:
            return "(BoolType)";
        case TypeKind::PTR:
            return "(PtrType)";
        case TypeKind::ADDR:
            return "(AddrType)";
    }
}

PrintExpr::~PrintExpr() {}
std::string PrintExpr::toString()
{
    return "(PrintExpr print)";
}

ASTKind PrintExpr::getASTKind()
{
    return ASTKind::PRINTEXPR;
}

LetExpr::LetExpr(std::vector<std::string> idents, std::vector<Expr*> body) : idents(idents), body(body) {;}
LetExpr::~LetExpr()
{
    for (auto e : body)
        delete e;
}
std::string LetExpr::toString()
{
    std::string acc = "(";
    int idx = 0;
    for (auto ident : idents)
    {
        acc += ident + (idx < idents.size()-1 ? " " : "");
        idx++;
    }
    acc += ")";

    idx = 0;
    std::string acc2 = "(";
    for (auto e : body)
    {
        acc2 += e->toString() + (idx < body.size()-1 ? " " : "");
        idx++;
    }
    acc2 += ")";

    return "(LetExpr " + acc + " " + acc2 + ")";
}
ASTKind LetExpr::getASTKind()
{
    return ASTKind::LETSTMT;
}

PeekExpr::PeekExpr(std::vector<std::string> idents, std::vector<Expr*> body) : idents(idents), body(body) {;}
PeekExpr::~PeekExpr()
{
    for (auto e : body)
        delete e;
}
std::string PeekExpr::toString()
{
    std::string acc = "(";
    int idx = 0;
    for (auto ident : idents)
    {
        acc += ident + (idx < idents.size()-1 ? " " : "");
        idx++;
    }
    acc += ")";

    idx = 0;
    std::string acc2 = "(";
    for (auto e : body)
    {
        acc2 += e->toString() + (idx < body.size()-1 ? " " : "");
        idx++;
    }
    acc2 += ")";

    return "(LetExpr " + acc + " " + acc2 + ")";
}
ASTKind PeekExpr::getASTKind()
{
    return ASTKind::LETSTMT;
}

OpExpr::OpExpr(std::string op) : op(op) {;}
OpExpr::~OpExpr() {}
std::string OpExpr::toString()
{
    return "(OpExpr " + op + ")";
}
ASTKind OpExpr::getASTKind()
{
    return ASTKind::OPEXPR;
}

IntExpr::IntExpr(long val) : value(val) {;}
std::string IntExpr::toString()
{
    return "(IntExpr " + std::to_string(value) + ")";
}

ASTKind IntExpr::getASTKind()
{
    return ASTKind::INTEXPR;
}

std::string TrueExpr::toString()
{
    return "(TrueExpr true)";
}


ASTKind TrueExpr::getASTKind()
{
    return ASTKind::TRUEEXPR;
}

std::string FalseExpr::toString()
{
    return "(FalseExpr false)";
}


ASTKind FalseExpr::getASTKind()
{
    return ASTKind::FALSEEXPR;
}

CharExpr::CharExpr(char c) : ch(c) {;}
std::string CharExpr::toString()
{
    return "(CharExpr " + std::to_string(ch) + ")";
}


ASTKind CharExpr::getASTKind()
{
    return ASTKind::CHAREXPR;
}


StringLitExpr::StringLitExpr(std::string val) : value(val) {;}
std::string StringLitExpr::toString()
{
    return "(StringLitExpr " + value + ")";
}
std::string StringLitExpr::getValue()
{
    return value;
}


ASTKind StringLitExpr::getASTKind()
{
    return ASTKind::STRINGLITEXPR;
}

VarExpr::VarExpr(std::string name) : name(name) {;}
std::string VarExpr::toString()
{
    return "(VarExpr " + name + ")";
}


ASTKind VarExpr::getASTKind()
{
    return ASTKind::VAREXPR;
}

IfExpr::IfExpr(std::vector<Expr *> then, std::vector<Expr*> elze, IfExpr *next) : then(then), elze(elze), next(next) {;}
IfExpr::~IfExpr()
{
    for (auto e : then)
        delete e;
    for (auto e : elze)
        delete e;
    if (next)
        delete next;

}


ASTKind IfExpr::getASTKind()
{
    return ASTKind::IFEXPR;
}

std::string IfExpr::toString()
{
    std::string accThen = "(";
    int idx = 0;
    for (auto e : then) 
    {
        accThen += e->toString() + (idx < then.size()-1 ? " " : "");
        idx++;
    }
    accThen += ")";

    idx = 0;
    std::string accElze = "(";
    for (auto e : elze) 
    {
        accElze += e->toString() + (idx < then.size()-1 ? " " : "");
        idx++;
    }
    accElze += ")";

    
    return "[IfExpr " + accThen + " " + accElze + " " + (next ? next->toString() : "") + "]";
}

WhileExpr::WhileExpr(std::vector<Expr*> cond, std::vector<Expr*> body) : cond(cond), body(body) {;}
WhileExpr::~WhileExpr()
{
    for (Expr *e : cond)
        delete e;
    for (Expr *e : body)
        delete e;
}


ASTKind WhileExpr::getASTKind()
{
    return ASTKind::WHILEEXPR;
}

std::string WhileExpr::toString() 
{
    std::string acc;
    std::string condstr = "(";
    std::string bodystr = "(";
    int i = 0;
    for (Expr *e : cond)
    {
        condstr += e->toString() + (i < cond.size()-1 ? " " : "");
        i++;
    }
    condstr += ")";

    i = 0;
    for (Expr *e : body)
    {
        bodystr += e->toString() + (i < body.size()-1 ? " " : "");
        i++;
    }

    return "(WhileExpr " + condstr + " " + bodystr + ")";
}

IncludeCmd::IncludeCmd(std::string path) : path(path) {;}
IncludeCmd::~IncludeCmd() {}
std::string IncludeCmd::toString()
{
    return "(IncludeCmd " + path + ")";
}
ASTKind IncludeCmd::getASTKind()
{
    return ASTKind::INCLUDECMD;
}


AST::~AST() {}
Cmd::~Cmd() {}
Stmt::~Stmt() {}
Expr::~Expr() {}

FnSignature::FnSignature(std::vector<Type> ins, std::vector<Type> outs) : params(ins), retTypes(outs) {;}
std::string FnSignature::toString()
{
    if (params.size() == 0 && retTypes.size() == 0)
        return "";


    std::string ins;
    std::string outs;

    int i, j;
    i = j = 0;

    for (Type t : params)
    {
        ins += t.toString() + (i < ins.size()-1 ? " " : "");
        i++;
    }

    for (Type t : retTypes)
    {
        outs += t.toString() + (j < retTypes.size()-1 ? " " : "");
        j++;
    }

    return "(" + ins + " -- " + outs + ")";
}

ProcCmd::ProcCmd(std::string name, FnSignature sig, std::vector<Expr*> body) : name(name), sig(sig), body(body) {;}
ProcCmd::~ProcCmd()
{
    for (AST *ast : body)
        delete ast;
}


ASTKind ProcCmd::getASTKind()
{
    return ASTKind::PROCCMD;
}

std::string ProcCmd::toString()
{
    std::string acc;
    for (int i = 0; i < body.size(); i++)
        if (i < body.size() - 1)
            acc += body[i]->toString() + " ";
        else
            acc += body[i]->toString();
    return "(ProcCmd " + name + " " + sig.toString() + " " + acc + ")";
}

ConstCmd::ConstCmd(std::string ident, std::vector<Expr *> exp) : ident(ident), body(exp) {;}

ConstCmd::~ConstCmd()
{
    for (auto e : body)
        delete e;
}


ASTKind ConstCmd::getASTKind()
{
    return ASTKind::CONSTCMD;
}

std::string ConstCmd::toString()
{
    std::string acc = "(";
    int i = 0;
    for (auto e : body)
    {
        acc += e->toString() + (i < body.size()-1 ? " " : "");
        i++;
    }
    acc += ")";

    return "(ConstCmd " + ident + " " + acc + ")";
}

MemoryCmd::MemoryCmd(std::string ident, std::vector<Expr *> expr) : ident(ident), body(expr) {;}
MemoryCmd::~MemoryCmd()
{
    for (auto e : body)
        delete e;
}


ASTKind MemoryCmd::getASTKind()
{
    return ASTKind::MEMORYCMD;
}

std::string MemoryCmd::toString()
{
    std::string acc = "(";
    int i = 0;
    for (auto e : body)
    {
        acc += e->toString() + (i < body.size()-1 ? " " : "");
        i++;
    }
    acc += ")";

    return "(MemoryCmd " + ident + " " + acc + ")";
}