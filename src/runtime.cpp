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

std::vector<AST*> toASTVec(std::vector<Expr*> exprs)
{
    std::vector<AST*> res;

    for (auto e : exprs)
        res.push_back(e);

    return res;
}

// interp

void interp(std::vector<AST*> prog, Stack& stack, Env& env)
{
    for (auto ast : prog)
    {
        // todo


    }
}