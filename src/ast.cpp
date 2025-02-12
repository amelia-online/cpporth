#include "ast.h"

Subexpr::Subexpr(int number) : num(number), boolean(false), op(""), type(SubexprType::INTEGER) {;}
Subexpr::Subexpr(bool val) : num(-1), boolean(val), op(""), type(SubexprType::BOOLEAN) {;}
Subexpr::Subexpr(std::string s) : num(-1), boolean(false), op(s), type(SubexprType::OPERATOR) {;}


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

AtomExpr::AtomExpr(std::vector<Subexpr> subexprs) : subexprs(subexprs) {;}
AtomExpr::~AtomExpr() {;}

AST::~AST() {}
Cmd::~Cmd() {}
Stmt::~Stmt() {}
Expr::~Expr() {}

std::string AtomExpr::toString()
{
    std::string acc;
    int i = 0;
    for (Subexpr& e : subexprs)
    {
        acc += e.toString() + ((i < subexprs.size() - 1) ? " " : "");
        i++;
    }
    return "(" + acc + ")";
}

ProcCmd::ProcCmd(std::string name, std::vector<AST*> body) : name(name), body(body) {;}
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
    return "(ProcCmd " + name + " " + acc + ")";
}

ConstCmd::ConstCmd(std::string ident, Expr *exp) : ident(ident), expression(exp) {;}

ConstCmd::~ConstCmd()
{
    delete expression;
}

std::string ConstCmd::toString()
{
    return "(ConstCmd " + ident + " " + expression->toString() + ")";
}

MemoryCmd::MemoryCmd(std::string ident, Expr *expr) : ident(ident), sizeExpr(expr) {;}
MemoryCmd::~MemoryCmd()
{
    delete sizeExpr;
}

std::string MemoryCmd::toString()
{
    return "(MemoryCmd " + ident + " " + sizeExpr->toString() + ")";
}