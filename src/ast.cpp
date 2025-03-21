#include "ast.h"

Type::Type(TypeKind kind) : kind(kind) {;}
std::string Type::toString()
{
    switch (kind)
    {
        case TypeKind::INT:
            return "(IntType)";
        case TypeKind::BOOL:
            return "(BoolType)";
        case TypeKind::PTR:
            return "(PtrType)";
        case TypeKind::ADDR:
            return "(AddrType)";
        case TypeKind::VARIANT:
            return "(VariantType)";
    }
}

VariantType::VariantType(std::string name) : name(name), Type(TypeKind::VARIANT) {;}
std::string VariantType::toString()
{
    return "(VariantType " + name + ")";
}

MemoryExpr::MemoryExpr(std::string name, std::vector<Expr*> body) : ident(name), body(body) {;}

MemoryExpr::MemoryExpr(MemoryExpr *other)
{
    ident = std::string(other->ident);
    body = std::vector<Expr*>(other->body);
}

MemoryExpr::~MemoryExpr()
{
    for (auto e : body)
        delete e;
}

std::string MemoryExpr::getIdent()
{
    return ident;
}

std::string MemoryExpr::toString()
{
    if (body.size() == 0)
        return "(MemoryExpr " + ident + ")";

    std::string acc = "(";
    int idx = 0;
    for (auto e : body)
    {
        acc += e->toString() + (idx < body.size()-1 ? " " : ")");
        idx++;
    }


    return "(MemoryExpr " + ident + " " + acc + ")";
}   

ASTKind MemoryExpr::getASTKind()
{
    return ASTKind::MEMORYEXPR;
}

PrintExpr::~PrintExpr() {}
std::string PrintExpr::toString()
{
    return "(PrintExpr print)";
}

ASTKind PrintExpr::getASTKind()
{
    return ASTKind::PRINTEXPR;
}

LetExpr::LetExpr(std::vector<std::string> idents, std::vector<Expr*> body) : idents(idents), body(body) {;}
LetExpr::~LetExpr()
{
    for (auto e : body)
        delete e;
}
std::string LetExpr::toString()
{
    std::string acc = "(";
    int idx = 0;
    for (auto ident : idents)
    {
        acc += ident + (idx < idents.size()-1 ? " " : "");
        idx++;
    }
    acc += ")";

    idx = 0;
    std::string acc2 = "(";
    for (auto e : body)
    {
        acc2 += e->toString() + (idx < body.size()-1 ? " " : "");
        idx++;
    }
    acc2 += ")";

    return "(LetExpr " + acc + " " + acc2 + ")";
}
ASTKind LetExpr::getASTKind()
{
    return ASTKind::LETSTMT;
}

PeekExpr::PeekExpr(std::vector<std::string> idents, std::vector<Expr*> body) : idents(idents), body(body) {;}
PeekExpr::~PeekExpr()
{
    for (auto e : body)
        delete e;
}
std::string PeekExpr::toString()
{
    std::string acc = "(";
    int idx = 0;
    for (auto ident : idents)
    {
        acc += ident + (idx < idents.size()-1 ? " " : "");
        idx++;
    }
    acc += ")";

    idx = 0;
    std::string acc2 = "(";
    for (auto e : body)
    {
        acc2 += e->toString() + (idx < body.size()-1 ? " " : "");
        idx++;
    }
    acc2 += ")";

    return "(LetExpr " + acc + " " + acc2 + ")";
}
ASTKind PeekExpr::getASTKind()
{
    return ASTKind::PEEKSTMT;
}

OpExpr::OpExpr(std::string op) : op(op) {;}
OpExpr::~OpExpr() {}
std::string OpExpr::toString()
{
    return "(OpExpr " + op + ")";
}
ASTKind OpExpr::getASTKind()
{
    return ASTKind::OPEXPR;
}

IntExpr::IntExpr(long val) : value(val) {;}
std::string IntExpr::toString()
{
    return "(IntExpr " + std::to_string(value) + ")";
}

ASTKind IntExpr::getASTKind()
{
    return ASTKind::INTEXPR;
}

long IntExpr::getValue()
{
    return value;
}

std::string TrueExpr::toString()
{
    return "(TrueExpr true)";
}


ASTKind TrueExpr::getASTKind()
{
    return ASTKind::TRUEEXPR;
}

std::string FalseExpr::toString()
{
    return "(FalseExpr false)";
}


ASTKind FalseExpr::getASTKind()
{
    return ASTKind::FALSEEXPR;
}

CharExpr::CharExpr(char c) : ch(c) {;}
std::string CharExpr::toString()
{
    return "(CharExpr " + std::to_string(ch) + ")";
}

char CharExpr::getValue()
{
    return ch;
}

ASTKind CharExpr::getASTKind()
{
    return ASTKind::CHAREXPR;
}


StringLitExpr::StringLitExpr(std::string val, bool cstr) : value(val), cstr(cstr) {;}
std::string StringLitExpr::toString()
{
    return "(StringLitExpr " + value + ")";
}
std::string StringLitExpr::getValue()
{
    return value;
}

bool StringLitExpr::isCStr() const
{
    return cstr;
}


ASTKind StringLitExpr::getASTKind()
{
    return ASTKind::STRINGLITEXPR;
}

VarExpr::VarExpr(std::string name) : name(name) {;}
std::string VarExpr::toString()
{
    return "(VarExpr " + name + ")";
}


ASTKind VarExpr::getASTKind()
{
    return ASTKind::VAREXPR;
}



IfExpr::IfExpr(std::vector<Expr *> then, std::vector<Expr*> elze, IfExpr *next) : then(then), elze(elze), next(next) {;}
IfExpr::~IfExpr()
{
    for (auto e : then)
        delete e;
    for (auto e : elze)
        delete e;
    if (next)
        delete next;

}


ASTKind IfExpr::getASTKind()
{
    return ASTKind::IFEXPR;
}

std::string IfExpr::toString()
{
    std::string accThen = "(";
    int idx = 0;
    for (auto e : then) 
    {
        accThen += e->toString() + (idx < then.size()-1 ? " " : "");
        idx++;
    }
    accThen += ")";

    idx = 0;
    std::string accElze = "(";
    for (auto e : elze) 
    {
        accElze += e->toString() + (idx < then.size()-1 ? " " : "");
        idx++;
    }
    accElze += ")";

    
    return "[IfExpr " + accThen + " " + accElze + " " + (next ? next->toString() : "") + "]";
}

WhileExpr::WhileExpr(std::vector<Expr*> cond, std::vector<Expr*> body) : cond(cond), body(body) {;}
WhileExpr::~WhileExpr()
{
    for (Expr *e : cond)
        delete e;
    for (Expr *e : body)
        delete e;
}


ASTKind WhileExpr::getASTKind()
{
    return ASTKind::WHILEEXPR;
}

std::string WhileExpr::toString() 
{
    std::string acc;
    std::string condstr = "(";
    std::string bodystr = "(";
    int i = 0;
    for (Expr *e : cond)
    {
        condstr += e->toString() + (i < cond.size()-1 ? " " : "");
        i++;
    }
    condstr += ")";

    i = 0;
    for (Expr *e : body)
    {
        bodystr += e->toString() + (i < body.size()-1 ? " " : "");
        i++;
    }
    bodystr += ")";

    return "(WhileExpr " + condstr + " " + bodystr + ")";
}

AssertExpr::AssertExpr(std::string msg, std::vector<Expr*> body) : msg(msg), body(body) {;}
AssertExpr::~AssertExpr()
{
    for (auto e : body)
        delete e;
}

std::string AssertExpr::toString()
{
    std::string acc = "(";
    int idx = 0;
    for (auto e : body)
    {
        acc += e->toString() + (idx < body.size()-1 ? " " : ")");
        idx++;
    }
    return "(AssertExpr " + msg + " " + acc + ")";
}

AssertCmd *AssertExpr::asCmd()
{
    return new AssertCmd(msg, body);
}

ASTKind AssertExpr::getASTKind()
{
    return ASTKind::ASSERTEXPR;
}

AssertCmd::AssertCmd(std::string msg, std::vector<Expr*> body) : msg(msg), body(body) {;}
AssertCmd::~AssertCmd()
{
    for (auto e : body)
        delete e;
}

std::string AssertCmd::toString()
{
    std::string acc = "(";
    int idx = 0;
    for (auto e : body)
    {
        acc += e->toString() + (idx < body.size()-1 ? " " : ")");
        idx++;
    }
    return "(AssertExpr " + msg + " " + acc + ")";
}

ASTKind AssertCmd::getASTKind()
{
    return ASTKind::ASSERTCMD;
}

OffsetExpr::OffsetExpr() {;}
OffsetExpr::~OffsetExpr() {;}
std::string OffsetExpr::toString()
{
    return "(OffsetExpr offset)";
}
ASTKind OffsetExpr::getASTKind()
{
    return ASTKind::OFFSETEXPR;
}

ResetExpr::ResetExpr() {;}
ResetExpr::~ResetExpr() {;}
std::string ResetExpr::toString()
{
    return "(ResetExpr reset)";
}
ASTKind ResetExpr::getASTKind()
{
    return ASTKind::RESETEXPR;
}

AddrOfExpr::AddrOfExpr(VarExpr *p) : proc(p) {;}
AddrOfExpr::~AddrOfExpr()
{
    delete proc;
}
std::string AddrOfExpr::toString()
{
    return "(AddrOfExpr " + proc->name + ")";
}
ASTKind AddrOfExpr::getASTKind()
{
    return ASTKind::ADDROFEXPR;
}

CallLikeExpr::CallLikeExpr(VarExpr *p) : proc(p) {;}
CallLikeExpr::~CallLikeExpr() 
{
    delete proc;
}
std::string CallLikeExpr::toString() 
{
    return "(CallLikeExpr " + proc->name + ")";
}
ASTKind CallLikeExpr::getASTKind()
{
    return ASTKind::CALLLIKEEXPR;
}

SwapExpr::SwapExpr() {;}
SwapExpr::~SwapExpr() {;}
std::string SwapExpr::toString()
{
    return "(SwapExpr swap)";
}
ASTKind SwapExpr::getASTKind()
{
    return ASTKind::SWAPEXPR;
}

DropExpr::DropExpr() {;}
DropExpr::~DropExpr() {;}
std::string DropExpr::toString()
{
    return "(DropExpr drop)";
}
ASTKind DropExpr::getASTKind()
{
    return ASTKind::DROPEXPR;
}

DupExpr::DupExpr() {;}
DupExpr::~DupExpr() {;}
std::string DupExpr::toString()
{
    return "(DupExpr dup)";
}
ASTKind DupExpr::getASTKind()
{
    return ASTKind::DUPEXPR;
}

OverExpr::OverExpr() {;}
OverExpr::~OverExpr() {;}
std::string OverExpr::toString()
{
    return "(OverExpr over)";
}
ASTKind OverExpr::getASTKind()
{
    return ASTKind::OVEREXPR;
}

RotExpr::RotExpr() {;}
RotExpr::~RotExpr() {;}
std::string RotExpr::toString()
{
    return "(RotExpr rot)";
}
ASTKind RotExpr::getASTKind()
{
    return ASTKind::ROTEXPR;
}

HereExpr::HereExpr() {;}
HereExpr::~HereExpr() {;}
std::string HereExpr::toString()
{
    return "(HereExpr here)";
}
ASTKind HereExpr::getASTKind()
{
    return ASTKind::HEREEXPR;
}

SyscallExpr::SyscallExpr(int n) : n(n) {;}
SyscallExpr::~SyscallExpr() {;}
std::string SyscallExpr::toString()
{
    return "(SyscallExpr syscall" + std::to_string(n) + ")";
}
ASTKind SyscallExpr::getASTKind()
{
    return ASTKind::SYSCALLEXPR;
}
int SyscallExpr::getNumArgs()
{
    return n;
}

MaxExpr::MaxExpr() {;}
MaxExpr::~MaxExpr() {;}
std::string MaxExpr::toString()
{
    return "(MaxExpr max)";
}
ASTKind MaxExpr::getASTKind()
{
    return ASTKind::MAXEXPR;
}

AllocExpr::AllocExpr() {;}
AllocExpr::~AllocExpr() {;}
std::string AllocExpr::toString()
{
   return "(AllocExpr alloc)";
}
ASTKind AllocExpr::getASTKind()
{
    return ASTKind::ALLOCSTMT;
}

FreeExpr::FreeExpr() {;}
FreeExpr::~FreeExpr() {;}
std::string FreeExpr::toString()
{
    return "(FreeExpr free)";
}
ASTKind FreeExpr::getASTKind()
{
    return ASTKind::FREEEXPR;
}

VariantBinding::VariantBinding(std::vector<std::string> idents, std::vector<Expr*> body) :
    idents(idents), body(body) {;}
VariantBinding::~VariantBinding()
{
    for (auto e : body)
        delete e;
}
std::string VariantBinding::toString()
{
    return "(MatchExpr case)";
}
ASTKind VariantBinding::getASTKind()
{
    return ASTKind::VARIANTBINDING;
}

MatchExpr::MatchExpr(std::unordered_map<std::string, VariantBinding*> map, std::string supertype) :
    branches(map), supertype(supertype) {;}
MatchExpr::~MatchExpr()
{
    for (auto [name, binding] : branches)
        delete binding;
}
std::string MatchExpr::toString()
{
    std::string acc = "(MatchExpr " + supertype + " ";

    int i = 0;
    for (auto [name, binding] : branches)
    {
        int ec = 0;
        acc += "(" + name + " (";
        for (auto e : binding->body)
        {
            acc += e->toString() + (ec < binding->body.size()-1 ? " " : "");
            ec++;
        }
        acc += "))";
        if (i < branches.size()-1)
            acc += " ";
        i++;
    }
    acc += ")";
    return acc;
}
ASTKind MatchExpr::getASTKind()
{
    return ASTKind::MATCHSTMT;
}

VariantInstanceExpr::VariantInstanceExpr(std::string name, std::string parentName, std::vector<std::vector<Expr *> > args) :
    variant(name), parent(parentName), args(args) {;}
VariantInstanceExpr::~VariantInstanceExpr()
{
    for (auto a : args)
        for (auto e : a)
            delete e;
}
std::string VariantInstanceExpr::toString()
{
    std::string acc = "(Variant " + parent + "::" + variant + " ";

    int expc = 0;
    int argc = 0;
    for (auto a : args)
    {
        expc = 0;
        acc += "(";
        for (auto e : a)
        {
            acc += e->toString() + (expc < a.size()-1 ? " " : "");
            expc++;
        }
        acc += ")";
        if (argc < args.size()-1)
            acc += " ";
        argc++;
    }
    acc += ")";
    return acc;
}
ASTKind VariantInstanceExpr::getASTKind()
{
    return ASTKind::VARIANTINSTANCEEXPR;
}

ArrayLitExpr::ArrayLitExpr(std::vector<std::vector<Expr*> > items) : items(items) {;}
ArrayLitExpr::~ArrayLitExpr()
{
    for (auto i : items)
        for (auto e : i)
            delete e;
}

std::string ArrayLitExpr::toString()
{
    std::string acc = "(ArrayLitExpr ";

    int i = 0;
    int j = 0;
    for (auto item : items)
    {
        acc += "(";
        i = 0;
        for (auto e : item)
        {
            acc += e->toString() + (i < item.size()-1 ? " " : "");
            i++;
        }
        acc += ")";
        if (j < items.size()-1)
            acc += " ";
        j++;    
    }
    acc += ")";
    return acc;
}

ASTKind ArrayLitExpr::getASTKind()
{
    return ASTKind::ARRAYLITEXPR;
}

IncludeCmd::IncludeCmd(std::string path) : path(path) {;}
IncludeCmd::~IncludeCmd() {}
std::string IncludeCmd::toString()
{
    return "(IncludeCmd " + path + ")";
}
ASTKind IncludeCmd::getASTKind()
{
    return ASTKind::INCLUDECMD;
}


AST::~AST() {}
Cmd::~Cmd() {}
Stmt::~Stmt() {}
Expr::~Expr() {}

FnSignature::FnSignature(std::vector<Type> ins, std::vector<Type> outs) : params(ins), retTypes(outs) {;}
std::string FnSignature::toString()
{
    if (params.size() == 0 && retTypes.size() == 0)
        return "";


    std::string ins;
    std::string outs;

    int i, j;
    i = j = 0;

    for (Type t : params)
    {
        ins += t.toString() + (i < ins.size()-1 ? " " : "");
        i++;
    }

    for (Type t : retTypes)
    {
        outs += t.toString() + (j < retTypes.size()-1 ? " " : "");
        j++;
    }

    return "(" + ins + "-- " + outs + ")";
}

ProcCmd::ProcCmd(std::string name, FnSignature sig, std::vector<Expr*> body) : name(name), sig(sig), body(body) {;}
ProcCmd::~ProcCmd()
{
    for (AST *ast : body)
        delete ast;
}


ASTKind ProcCmd::getASTKind()
{
    return ASTKind::PROCCMD;
}

std::string ProcCmd::toString()
{
    std::string acc;
    for (int i = 0; i < body.size(); i++)
        if (i < body.size() - 1)
            acc += body[i]->toString() + " ";
        else
            acc += body[i]->toString();
    return "(ProcCmd " + name + " " + sig.toString() + " " + acc + ")";
}

ConstCmd::ConstCmd(std::string ident, std::vector<Expr *> exp) : ident(ident), body(exp) {;}

ConstCmd::~ConstCmd()
{
    for (auto e : body)
        delete e;
}


ASTKind ConstCmd::getASTKind()
{
    return ASTKind::CONSTCMD;
}

std::string ConstCmd::toString()
{
    std::string acc = "(";
    int i = 0;
    for (auto e : body)
    {
        acc += e->toString() + (i < body.size()-1 ? " " : "");
        i++;
    }
    acc += ")";

    return "(ConstCmd " + ident + " " + acc + ")";
}

MemoryCmd::MemoryCmd(std::string ident, std::vector<Expr *> expr) : ident(ident), body(expr) {;}
MemoryCmd::~MemoryCmd()
{
    for (auto e : body)
        delete e;
}


ASTKind MemoryCmd::getASTKind()
{
    return ASTKind::MEMORYCMD;
}

std::string MemoryCmd::toString()
{
    std::string acc = "(";
    int i = 0;
    for (auto e : body)
    {
        acc += e->toString() + (i < body.size()-1 ? " " : "");
        i++;
    }
    acc += ")";

    return "(MemoryCmd " + ident + " " + acc + ")";
}

MemoryExpr *MemoryCmd::toMemoryExpr()
{
    return new MemoryExpr(ident, body);
}

Field::Field(std::string name, Type type) : name(name), type(type) {;}
std::string Field::toString()
{
    return "(" + name + " :: " + type.toString() + ")";
}

Variant::Variant(std::string name, std::string parentName, std::vector<Field> fields) :
    name(name), parentName(parentName), fields(fields) {;}
std::string Variant::toString()
{
    std::string acc = "(" + parentName + "::" + name + " ";

    int i = 0;
    for (auto field : fields)
    {
        acc += field.toString() + (i < fields.size()-1 ? " " : "");
        i++;
    }
    acc += ")";
    return acc;
}

TypeCmd::TypeCmd(std::string name, std::vector<Variant> variants) :
    name(name), variants(variants) {;}
TypeCmd::~TypeCmd() {;}
std::string TypeCmd::toString()
{
    std::string acc = "(" + name + " ";

    int i = 0;
    for (auto variant : variants)
    {
        acc += variant.toString() + (i < variants.size()-1 ? " " : "");
        i++;
    }
    acc += ")";
    return acc;
}
ASTKind TypeCmd::getASTKind()
{
    return ASTKind::TYPECMD;
}

