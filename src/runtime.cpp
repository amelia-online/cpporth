#include "runtime.h"
#include <iostream>

Data::Data(long l) : ptr(new long(l)), type(Type(TypeKind::INT)) {;}

Data::Data(bool b) : ptr(new bool(b)), type(Type(TypeKind::BOOL)) {;}

Data::Data(void *ptr) : ptr(ptr), type(Type(TypeKind::PTR)) {;}

Data::~Data()
{
    switch (type.kind)
    {
        case TypeKind::INT:
            delete ((long *)ptr);
            break;
        case TypeKind::BOOL:
            delete ((bool *)ptr);
            break;
        case TypeKind::PTR:
            delete ((uchar *)ptr);
            break;
        default:
            break;
    }
}

Type Data::getType()
{
    return type;
}

long Data::getIntVal()
{
    return *((long *)ptr);
}

bool Data::getBoolVal()
{
    return *((bool *)ptr);
}

uchar *Data::getPtrVal()
{
    return ((uchar *)ptr);
}


void Stack::push(long l)
{
    Data d(l);
    data.push_back(d);
}

void Stack::push(bool b)
{
    data.push_back(Data(b));
}

void Stack::push(void *ptr)
{
    data.push_back(Data(ptr));
}

Data Stack::peek()
{
    if (size() < 1)
    {
        std::cout << "Error: stack is empty." << std::endl;
        throw new std::exception();
    }
    return data[data.size()-1];
}

Data Stack::pop()
{
    if (size() < 1)
    {
        std::cout << "Error: stack is empty." << std::endl;
        throw new std::exception();
    }

    Data l = data[data.size() - 1];
    data.pop_back();
    return l;
}

int Stack::size()
{
    return data.size();
}

std::vector<AST*> toAstVec(std::vector<Expr*> exprs)
{
    std::vector<AST*> res;

    for (auto e : exprs)
        res.push_back(e);

    return res;
}

// interp

Data interp(std::vector<AST*> prog, Stack& stack, Env& env)
{
    for (auto ast : prog)
    {
        switch (ast->getASTKind())
        {
            case ASTKind::PROCCMD:
                env.procs.insert(std::make_pair(((ProcCmd *)ast)->name, (ProcCmd *)ast));
                break;
            case ASTKind::CONSTCMD:
            {
                Stack s;
                ConstCmd *c = (ConstCmd *)ast;
                env.variables.insert(std::make_pair(c->ident, interpExpr(c->body, s, env)));
            }
            case ASTKind::INCLUDECMD:
                break;
            case ASTKind::MEMORYCMD:
                break;
            case ASTKind::ASSERTCMD:
                break;
            default:
                std::cout << "Error: not a command: " << ast->toString() << std::endl;
                throw new std::exception();
        }
    }

    return interpExpr(env.procs.at("main")->body, stack, env);
}

Data interpExpr(std::vector<Expr*> exps, Stack& stack, Env& env)
{
    for (auto exp : exps)
    {
        switch (exp->getASTKind())
        {
            case ASTKind::INTEXPR:
            {
                IntExpr *i = (IntExpr *)exp;
                stack.push(i->getValue());
                break;
            }

            case ASTKind::VAREXPR:
            {
                VarExpr *v = (VarExpr *)exp;
                
                if (env.procs.find(v->name) != env.procs.end())
                    interpExpr(env.procs.at(v->name)->body, stack, env);
                else if (env.variables.find(v->name) != env.variables.end())
                    stack.push(env.variables.at(v->name));


            }
        }
    }

    return stack.pop();
}