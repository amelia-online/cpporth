#ifndef CPPORTH_RUNTIME_H
#define CPPORTH_RUNTIME_H

#include <unordered_map>
#include "ast.h"
typedef unsigned char uchar;

class Value
{
    Type type;
    
public:
    // todo
};

class Data
{
    Type type;
    void *ptr;
public:
    Data(void *);
    Data(long);
    Data(bool);
    ~Data();
    Type getType();
    bool getBoolVal();
    long getIntVal();
    uchar *getPtrVal();
};

class Env
{
public:
    std::unordered_map<std::string, Data> variables;
    std::unordered_map<std::string, ProcCmd*> procs;
};


class Stack
{
    std::vector<Data> data;
public:
    void push(long);
    void push(bool);
    void push(void *);
    void push(Data);
    Data pop();
    Data peek();
    int size();
};

std::vector<AST*> toAstVec(std::vector<Expr*>);
Data interp(std::vector<AST*>, Stack&, Env&);
Data interpExpr(std::vector<Expr*>, Stack&, Env&);
void typecheck(std::vector<AST*>);

#endif // CPPORTH_RUNTIME_H