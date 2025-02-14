#include "ast.h"

Subexpr::Subexpr(int number) : num(number), boolean(false), op(""), type(SubexprType::INTEGER) {;}
Subexpr::Subexpr(bool val) : num(-1), boolean(val), op(""), type(SubexprType::BOOLEAN) {;}
Subexpr::Subexpr(std::string s) : num(-1), boolean(false), op(s), type(SubexprType::OPERATOR) {;}

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


std::string Subexpr::toString()
{
    switch (type)
    {
        case SubexprType::BOOLEAN:
            return std::to_string(boolean);
        case SubexprType::INTEGER:
            return std::to_string(num);
        case SubexprType::OPERATOR:
            return op;
    }
}

IntExpr::IntExpr(long val) : value(val) {;}
std::string IntExpr::toString()
{
    return "(IntExpr " + std::to_string(value) + ")";
}

std::string TrueExpr::toString()
{
    return "(TrueExpr true)";
}

std::string FalseExpr::toString()
{
    return "(FalseExpr false)";
}

CharExpr::CharExpr(char c) : ch(c) {;}
std::string CharExpr::toString()
{
    return "(CharExpr " + std::to_string(ch) + ")";
}


StringLitExpr::StringLitExpr(std::string val) : value(val) {;}
std::string StringLitExpr::toString()
{
    return "(StringLitExpr " + value + ")";
}

VarExpr::VarExpr(std::string name) : name(name) {;}
std::string VarExpr::toString()
{
    return "(VarExpr " + name + ")";
}

IfExpr::IfExpr(std::vector<std::vector<Expr*> > branches) : branches(branches) {;}
IfExpr::~IfExpr()
{
    for (auto& branch : branches)
        for (Expr *e : branch)
            delete e;

}

std::string IfExpr::toString()
{
    std::string acc;
    int b = 0;
    for (auto& branch : branches)
    {
        int i = 0;
        std::string br = "(";
        for (Expr*& e : branch)
        {
            br += e->toString() + (i < branch.size()-1 ? " " : "");
            i++;
        }
        br += ")";
        acc += br + (b < branches.size() ? " " : "");
        b++;
    }
    return "(IfExpr " + acc + ")";
}

WhileExpr::WhileExpr(std::vector<Expr*> cond, std::vector<Expr*> body) : cond(cond), body(body) {;}
WhileExpr::~WhileExpr()
{
    for (Expr *e : cond)
        delete e;
    for (Expr *e : body)
        delete e;
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


AST::~AST() {}
Cmd::~Cmd() {}
Stmt::~Stmt() {}
Expr::~Expr() {}

FnSignature::FnSignature(std::vector<Type> ins, std::vector<Type> outs) : params(ins), retTypes(outs) {;}
std::string FnSignature::toString()
{
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