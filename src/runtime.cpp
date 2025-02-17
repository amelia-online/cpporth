#include "runtime.h"
#include <iostream>

void Stack::push(long l)
{
    data.push_back(l);
}

long Stack::pop()
{
    long l = data[data.size() - 1];
    data.pop_back();
    return l;
}

int Stack::size()
{
    return data.size();
}

std::vector<AST*> toASTVec(std::vector<Expr*> exprs)
{
    std::vector<AST*> res;

    for (auto e : exprs)
        res.push_back(e);

    return res;
}

void demo(std::vector<AST*> prog, Stack& stack, Env& env)
{
    for (AST *ast : prog)
    {
        switch (ast->getASTKind())
        {
            case ASTKind::PROCCMD:
            {
                ProcCmd *p = (ProcCmd *)ast;
                env.procs.insert(std::make_pair(p->name, p));
                demo(toASTVec(p->body), stack, env);
                break;
            }
            case ASTKind::VAREXPR:
            {
                stack.push(env.variables.at(((VarExpr *)ast)->name));
                break;
            }
            case ASTKind::INTEXPR:
            {
                IntExpr *e = (IntExpr*)ast;
                stack.push(e->value);
                break;
            }
            case ASTKind::PRINTEXPR:
            {
                long l = stack.pop();
                std::cout << l << std::endl;
                break;
            }
            case ASTKind::LETSTMT:
            {
                LetExpr *let = (LetExpr *)ast;
                for (auto i : let->idents)
                {
                    long l = stack.pop();
                    env.variables.insert(std::make_pair(i, l));
                }
                demo(toASTVec(let->body), stack, env);
                break;
            }
            case ASTKind::OPEXPR:
            {
                OpExpr *o = (OpExpr *)ast;
                if (o->op == "+")
                {
                    long rhs = stack.pop();
                    long lhs = stack.pop();
                    stack.push(lhs + rhs);
                }
                break;
            }
            default:
                std::cout << "Not implemented" << std::endl;
                break;
        }
    }
}