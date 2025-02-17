#ifndef CPPORTH_RUNTIME_H
#define CPPORTH_RUNTIME_H

#include <unordered_map>
#include "ast.h"

class Env
{
    std::unordered_map<std::string, Expr*> variables;
    std::unordered_map<std::string, ProcCmd*> procs;
};

class Stack
{

};

class Value
{
public:
    virtual ~Value() = 0;
    virtual std::string toString() = 0;
};


Value interp(std::vector<AST*>, Stack&, Env&);
void typecheck(std::vector<AST*>);

#endif // CPPORTH_RUNTIME_H