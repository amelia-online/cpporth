#ifndef CPPORTH_RUNTIME_H
#define CPPORTH_RUNTIME_H

#include <unordered_map>
#include "ast.h"

class Data
{
    long value;
    TypeKind type;
    bool isNone_;
public:
    Data(long, TypeKind);
    Data();
    TypeKind getType() const;
    void assertType(TypeKind, int) const;
    long getValue() const;
    bool isTrue();
    bool isFalse();
    bool isPtr();
    bool isInt();
    bool isNone();
};

class Env
{
public:
    std::unordered_map<std::string, Data> variables;
    std::unordered_map<std::string, ProcCmd*> procs;
    int offset = 0;
    Env(int, char**);
};


class Stack
{
    std::vector<Data> data;
public:
    void push(long);
    void push(bool);
    void push(void *);
    void push(Data);
    void assertMinSize(int, int);
    void append(const Stack&);
    void clear();
    bool isEmpty();
    std::vector<Data> toVector() const;
    Data pop();
    Data peek();
    Data top();
    Stack scope(const ProcCmd*);
    int size();
    std::string toString();
};

std::vector<AST*> toAstVec(std::vector<Expr*>);
Data interp(std::vector<AST*>, Stack&, Env&);
Data interpExpr(std::vector<Expr*>, Stack&, Env&);
void typecheck(std::vector<AST*>);
void include(std::string, Env&);
#endif // CPPORTH_RUNTIME_H