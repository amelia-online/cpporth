#ifndef CPPORTH_RUNTIME_H
#define CPPORTH_RUNTIME_H

#include <unordered_map>
#include "ast.h"

class Value
{
public:
    virtual ~Value() = 0;
    virtual std::string toString() = 0;
};

class IntValue : public Value
{
public:
    long value;
    IntValue(long);
    ~IntValue();

};

class Env
{
public:
    std::unordered_map<std::string, long> variables;
    std::unordered_map<std::string, ProcCmd*> procs;
};

class Stack
{
    std::vector<long> data;
public:
    void push(long);
    long pop();
    int size();
};

std::vector<AST*> toASTVec(std::vector<Expr*>);
void demo(std::vector<AST*>, Stack&, Env&);
Value *interp(std::vector<AST*>, Stack&, Env&);
void typecheck(std::vector<AST*>);

#endif // CPPORTH_RUNTIME_H