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

Expr::Expr(std::vector<Subexpr> subexprs) : subexprs(subexprs) {;}
Expr::~Expr() {;}

std::string Expr::toString()
{
    std::string acc;
    for (Subexpr& e : subexprs)
        acc += e.toString() + " ";
    return "(" + acc + ")";
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