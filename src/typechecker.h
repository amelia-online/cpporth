#ifndef CPPORTH_TYPECHECKER_H
#define CPPORTH_TYPECHECKER_H

#include <vector>
#include <unordered_map>
#include "ast.h"

enum class Datatype
{
    INT,
    PTR,
    BOOL,
    ADDR
};

#define Int Datatype::INT
#define Ptr Datatype::PTR
#define Bool Datatype::BOOL
#define Addr Datatype::ADDR
#define TypeVec std::vector<Datatype>

Datatype typeToDatatype(Type);

class TypeStack
{
    std::vector<Datatype> stack;
public:
    bool isEmpty();
    size_t size();
    void clear();
    Datatype pop();
    void push(Datatype);
    Datatype peek();
    TypeVec slice(size_t, size_t);
};

class NameInfo
{
public:
    NameInfo(bool);
    bool isFunction;
};

class VarInfo : public NameInfo
{
    Datatype type;
public:
    VarInfo(Datatype);
    Datatype getType() const;
};

class FunInfo : public NameInfo
{
    std::vector<Datatype> ins;
    std::vector<Datatype> outs;
    ProcCmd *proc;
public:
    FunInfo(ProcCmd*);
    std::vector<Datatype> getIns() const;
    std::vector<Datatype> getOuts() const;
    ProcCmd *getProcCmd() const;
};

class TypeEnv
{
    std::unordered_map<std::string, NameInfo> map;
public:
    TypeEnv();
    TypeEnv(TypeEnv const&);
    bool isVar(std::string);
    bool isFunc(std::string);
    bool containsKey(std::string);
    void putVar(std::string, Datatype);
    void putFunc(std::string, ProcCmd *);
    void remove(std::string);
    NameInfo get(std::string);
};

class Typechecker
{
    int line;
    std::string filepath;
    std::vector<AST*> prog;
public:
    Typechecker(std::vector<AST*>, std::string);
    void typecheck();
    void typecheckExpr(std::vector<Expr*>, 
        TypeStack&,
        TypeEnv&
    );
    void typeError(std::string);
    void typecheckContract(ProcCmd *, TypeStack&);
    void setPath(std::string);
};


#endif // CPPORTH_TYPECHECKER_H