#include "typechecker.h"
#include "helper.h"
#include <iostream>

Datatype typeToDatatype(Type t)
{
    switch (t.kind)
    {
        case TypeKind::ADDR:
            return Addr;
        case TypeKind::BOOL:
            return Bool;
        case TypeKind::INT:
            return Int;
        case TypeKind::PTR:
            return Ptr;
    }
}

bool TypeStack::isEmpty()
{
    return stack.size() == 0;
}

size_t TypeStack::size()
{
    return stack.size();
}

void TypeStack::clear()
{
    stack.clear();
}

Datatype TypeStack::pop()
{
    auto res = stack.back();
    stack.pop_back();
    return res;
}

Datatype TypeStack::peek()
{
    return stack.back();
}

void TypeStack::push(Datatype t)
{
    stack.push_back(t);
}

TypeVec TypeStack::slice(size_t start, size_t end)
{
    TypeVec res;
    
    for (size_t i = start; i < end; i++)
        res.push_back(stack[i]);

    return res;
}

NameInfo::NameInfo(bool b) : isFunction(b) {;}
VarInfo::VarInfo(Datatype t) : type(t), NameInfo(false) {;}
Datatype VarInfo::getType() const
{
    return type;
}

FunInfo::FunInfo(ProcCmd *p) : proc(p), NameInfo(true) 
{
    for (auto t : p->sig.params)
        ins.push_back(typeToDatatype(t));
    
    for (auto t : p->sig.retTypes)
        outs.push_back(typeToDatatype(t));
}

TypeVec FunInfo::getIns() const
{
    return ins;
}

TypeVec FunInfo::getOuts() const
{
    return outs;
}

ProcCmd *FunInfo::getProcCmd() const
{
    return proc;
}

TypeEnv::TypeEnv() {;}

TypeEnv::TypeEnv(const TypeEnv& other)
{
    map = std::unordered_map(other.map);
}

bool TypeEnv::containsKey(std::string key)
{
    return map.find(key) != map.end();
}

bool TypeEnv::isFunc(std::string key)
{
    return containsKey(key) && map.at(key).isFunction;
}

bool TypeEnv::isVar(std::string key)
{
    return containsKey(key) && !map.at(key).isFunction;
}

void TypeEnv::putFunc(std::string name, ProcCmd *p)
{
    map.insert(std::make_pair(name, FunInfo(p)));
}

void TypeEnv::putVar(std::string key, Datatype t)
{
    map.insert(std::make_pair(key, VarInfo(t)));
}

NameInfo TypeEnv::get(std::string name)
{
    return map.at(name);
}

void TypeEnv::remove(std::string name)
{
    map.erase(name);
}

Typechecker::Typechecker(std::vector<AST*> prog, std::string fp) : prog(prog), line(1), filepath(fp) {;}

void Typechecker::typecheck()
{
    TypeEnv e;
    for (auto ast : prog)
    {
        switch (ast->getASTKind())
        {
            case ASTKind::PROCCMD:
            {
                auto p = (ProcCmd *)ast;
                e.putFunc(p->name, p);
                break;  
            }

            case ASTKind::CONSTCMD:
            {
                auto c = (ConstCmd*)ast;
                TypeStack t;
                typecheckExpr(c->body, t, e);
                if (t.size() != 1)
                    typeError("Constant '" + c->ident + "': stack has more than one or zero item(s) remaining");

                auto type = t.pop();
                e.putVar(c->ident, type);

                break;
            }

            case ASTKind::ASSERTCMD:
            {
                auto a = (AssertCmd *)ast;
                TypeStack t;
                typecheckExpr(a->body, t, e);

                if (t.size() != 1)
                    typeError("assertion: stack has more than one or zero item(s) remaining");
                if (t.pop() != Bool)
                    typeError("Assertion type must be a boolean");

                break;
            }

            case ASTKind::INCLUDECMD:
            {
                // TODO: make better.
                auto i = (IncludeCmd *)ast;
                Typechecker tch(toASTs(i->path), realString(i->path));

                tch.typecheck();

                for (auto as : tch.prog)
                    delete as;

                break;
            }

            case ASTKind::MEMORYCMD:
            {
                auto m = (MemoryCmd *)ast;
                
                TypeStack s;
                typecheckExpr(m->body, s, e);
                if (s.size() != 1)
                    typeError("memory: stack has more than one or zero item(s) remaining");
                if (s.pop() != Int)
                    typeError("memory: body must evaluate to int");
                break;
            }
        }
    }
    if (!e.containsKey("main") || !e.isFunc("main"))
        typeError("main function is undefined");

    auto main = ((FunInfo *)&(e.get("main")))->getProcCmd();
    TypeStack s;
    typecheckExpr(main->body, s, e);
}

void Typechecker::typecheckExpr(std::vector<Expr*> exprs, TypeStack& s, TypeEnv& e)
{
    for (auto exp : exprs)
    {
        //std::cout << stack.toString() << " " << exp->toString() << std::endl;

        switch (exp->getASTKind())
        {
            case ASTKind::INTEXPR:
            {
                IntExpr *i = (IntExpr *)exp;
                s.push(Int);
                break;
            }

            case ASTKind::ALLOCSTMT:
            {
                auto a = (AllocExpr *)exp;
                Datatype size = s.pop();
                if (size != Int)
                    typeError("Alloc requires int");
                s.push(Ptr);
                break;
            }

            case ASTKind::MATCHSTMT:
            {
                auto m = (MatchExpr *)exp;
                // VariantData *v = (VariantData *)stack.pop().getValue();

                // auto branch = m->branches.find(v->name) != m->branches.end() ? 
                //     m->branches.at(v->name) : m->branches.at("else");

                // for (int i = 0; i < branch->idents.size(); i++)
                //     env.variables.insert(std::make_pair(branch->idents[i], v->values[i]));
                
                // interpExpr(branch->body, stack, env);

                // for (auto ident : branch->idents)
                //     env.variables.erase(ident);

                break;
            }

            case ASTKind::VARIANTINSTANCEEXPR:
            {
                // auto n = (VariantInstanceExpr *)exp;
                // Stack s;
            
                // std::vector<Data> data;
                // for (auto arg : n->args)
                // {
                //     data.push_back(interpExpr(arg, s, env));
                //     s.clear();
                // }

                // auto res = new VariantData(n->variant, n->parent, data);
                // stack.push(res);

                break;
            }

            case ASTKind::FREEEXPR:
            {
                auto top = s.pop();
                if (top != Ptr)
                    typeError("Free requires type ptr");
                break;
            }

            case ASTKind::VAREXPR:
            {
                VarExpr *v = (VarExpr *)exp;
                
                if (!e.containsKey(v->getName()))
                    typeError("Identifier '" + v->getName() + "' is undefined");

                if (e.isVar(v->getName()))
                {
                    NameInfo *namei = &(e.get(v->getName()));
                    VarInfo *var = (VarInfo *)namei;
                    s.push(var->getType());
                }
                else
                {
                    FunInfo *f = (FunInfo *)&(e.get(v->getName()));
                    ProcCmd *p = f->getProcCmd();
                    typecheckContract(p, s);
                    typecheckExpr(p->body, s, e);
                }

                break;
            }

            case ASTKind::ADDROFEXPR: 
            {
                auto a = (AddrOfExpr *)exp;
                auto v = a->proc;
                
                if (!e.containsKey(v->getName()) || (e.containsKey(v->getName()) && !e.isFunc(v->getName())))
                    typeError("Procedure " + v->getName() + " does not exist");

                s.push(Addr);
                break;
            }

            case ASTKind::CALLLIKEEXPR:
            {
                typeError("`call-like` statement is not supported in this version of cpporth");
                break;
            }

            case ASTKind::WHILEEXPR:
            {
                WhileExpr *w = (WhileExpr *)exp;
                
                TypeStack stack;
                typecheckExpr(w->cond, stack, e);
                auto t = stack.pop();

                if (t != Bool)
                    typeError("While loop condition must result in type bool");

                typecheckExpr(w->body, s, e);
                
                break;
            }

            case ASTKind::STRINGLITEXPR:
            {
                StringLitExpr *str = (StringLitExpr *)exp;
                if (!str->isCStr())
                    s.push(Int);
                s.push(Ptr);
                break;
            }

            case ASTKind::CHAREXPR: // broken
            {
                s.push(Int);
                break;
            }

            case ASTKind::ASSERTEXPR:
            {
                auto ass = (AssertExpr *)exp;

                TypeStack stack;
                typecheckExpr(ass->body, stack, e);
                auto res = s.pop();

                if (res != Bool)
                    typeError("Assert expression must be of type bool");

                break;
            }

            case ASTKind::SYSCALLEXPR:
            {
                auto e = (SyscallExpr *)exp;
                
                //stack.assertMinSize(e->getNumArgs()+1, exp->line);
                //stack.peek().assertType(TypeKind::INT, exp->line);

                break;
            }

            case ASTKind::MEMORYEXPR:
            {
                auto ex = (MemoryExpr *)exp;
                TypeStack sta;
                typecheckExpr(ex->body, sta, e);                
                if (sta.pop() != Int)
                    typeError("Memory expression must be of type int");
                break;
            }

            case ASTKind::IFEXPR:
            {
                auto f = (IfExpr *)exp;

                auto r = s.pop();

                if (r != Bool)
                    typeError("If statement requires a bool on the stack");

                typecheckExpr(f->then, s, e);

                if (f->elze.size() > 0)
                {
                    typecheckExpr(f->elze, s, e);

                    if (f->next)
                    {
                        r = s.pop();
                        if (r != Bool)
                            typeError("If* statement requires a bool on the stack.");
                        std::vector<Expr*> fnext = {f->next};
                        typecheckExpr(fnext, s, e);
                    }
                }
            
                break;
            }

            case ASTKind::LETSTMT:
            {
                auto let = (LetExpr *)exp;
                
                if (s.size() < let->idents.size())
                    typeError("Insufficient stack elements for amount of bindings");

                for (std::string ident : let->idents)
                {
                    auto elem = s.pop();

                    if (ident == "_")
                        continue;

                    e.putVar(ident, elem);
                }

                typecheckExpr(let->body, s, e);

                for (std::string ident : let->idents)
                    if (ident != "_")
                        e.remove(ident);

                break;
            }

            case ASTKind::PEEKSTMT:
            {
                auto peek = (PeekExpr *)exp;
                
                if (s.size() < peek->idents.size())
                    typeError("Insufficient stack elements for amount of bindings");

                TypeVec slice = s.slice(s.size()-1-peek->idents.size(), s.size()-1);
                for (std::string ident : peek->idents)
                {
                   auto elem = slice.back();
                   slice.pop_back();

                    if (ident == "_")
                        continue;

                    e.putVar(ident, elem);
                }

                typecheckExpr(peek->body, s, e);

                for (std::string ident : peek->idents)
                    if (ident != "_")
                        e.remove(ident);

                break;
            }

            case ASTKind::OPEXPR:
            {
                auto op = (OpExpr *)exp;

                // ARITHMETIC

                if (
                    op->op == "*" 
                    || op->op == "+" 
                    || op->op == "-"
                    || op->op == "shl"
                    || op->op == "shr"
                    || op->op == "or"
                    || op->op == "and"
                )
                {
                    if (s.size() < 2)
                        typeError(op->op + " requires at least two stack elements");
                    auto rhs = s.pop();
                    auto lhs = s.pop();

                    if (rhs != Int || lhs != Int)
                        typeError(op->op + ": both operands must be of type int");
                    
                    s.push(Int);
                }

                else if (op->op == "divmod")
                {
                    if (s.size() < 2)
                        typeError(op->op + " requires at least two stack elements");
                    auto rhs = s.pop();
                    auto lhs = s.pop();

                    if (rhs != Int || lhs != Int)
                        typeError(op->op + ": both operands must be of type int");
                    
                    s.push(Int);
                    s.push(Int);
                }

                // COMPARISON

                else if (
                        op->op == "<"
                        || op->op == "<="
                        || op->op == ">"
                        || op->op == ">="
                    )
                {
                    if (s.size() < 2)
                        typeError(op->op + " requires at least two stack elements");

                    auto rhs = s.pop();
                    auto lhs = s.pop();

                    if (rhs != Int || lhs != Int)
                        typeError(op->op + ": both operands must be of type int");

                    s.push(Bool);
                }

                else if (op->op == "=" || op->op == "!=")
                {
                    if (s.size() < 2)
                        typeError(op->op + " requires at least two stack elements");

                    auto rhs = s.pop();
                    auto lhs = s.pop();

                    if (rhs != Int && rhs != Bool || lhs != Int && lhs != Bool || lhs != rhs)
                        typeError("Only ints and bools can be compared, and lhs and rhs must be of the same type");
                    
                    s.push(Bool);
                }

                // BITWISE

                else if (op->op == "not")
                {
                    if (s.size() < 1)
                        typeError("not requires at least one stack element");

                    auto a = s.pop();

                    if (a != Int)
                        typeError("not only works on ints");

                    s.push(Int);
                }

                // MEMOPS

                // Storing
                else if (
                    op->op == "!8"
                    || op->op == "!16"
                    || op->op == "!32"
                    || op->op == "!64"
                )
                {
                    if (s.size() < 2)
                        typeError(op->op + " requires at least two stack elements");

                    auto ptr = s.pop();
                    auto byte = s.pop();

                    if (ptr != Ptr)
                        typeError(op->op + ": Top of stack must be of type ptr");

                    if (byte != Int)
                        typeError(op->op + ": Only ints can be stored");

                    break;
                }


                else if (
                    op->op == "@8"
                    || op->op == "@16"
                    || op->op == "@32"
                    || op->op == "@64"
                )
                {
                    if (s.size() < 1)
                        typeError(op->op + ": Insufficient stack elements");

                    auto ptr = s.pop();

                    if (ptr != Ptr)
                        typeError(op->op + ": Argument must be of type ptr");
                   
                    s.push(Int);
                    break;
                }

                // CAST

                else if (op->op == "cast(bool)")
                {
                    if (s.size() < 1)
                        typeError(op->op + ": Insufficient stack elements");

                    auto a = s.pop();

                    if (a == Ptr)
                        typeError(op->op + ": Cannot cast ptr to type bool");

                    s.push(Bool);
                    break;
                }

                else if (op->op == "cast(int)")
                {
                    if (s.size() < 1)
                        typeError(op->op + ": Insufficient stack elements");

                    auto a = s.pop();
                    s.push(Int);
                    break;
                }

                else if (op->op == "cast(ptr)")
                {
                    if (s.size() < 1)
                        typeError(op->op + ": Insufficient stack elements");

                    auto a = s.pop();

                    if (a == Bool)
                        typeError(op->op + ": Cannot cast bool to type ptr");

                    s.push(Ptr);
                    break;
                }

                break;
            }

            case ASTKind::PRINTEXPR:
                if (s.size() < 1)
                    typeError("print: Insufficient stack elements");

                s.pop();

                break;

            case ASTKind::DUPEXPR:
            {
                if (s.size() < 1)
                    typeError("dup: Insufficient stack elements");

                s.push(s.peek());
                break;
            }

            case ASTKind::HEREEXPR:
            {
                s.push(Int);
                s.push(Ptr);
                break;
            }

            case ASTKind::OFFSETEXPR:
            {
                if (s.size() < 1)
                     typeError("offset: Insufficient stack elements");

                s.pop();
                break;
            }

            case ASTKind::RESETEXPR:
            {
                break;
            }

            case ASTKind::MAXEXPR:
            {
                if (s.size() < 2)
                    typeError("max: Insufficient stack elements");

                auto rhs = s.pop();
                auto lhs = s.pop();
                
                if (rhs != Int || lhs != Int)
                    typeError("max: both elements must be of type int");

                s.push(Int);
                break;
            }

            case ASTKind::DROPEXPR:
            {
                if (s.size() < 1)
                    typeError("drop: Insufficient stack elements");

                s.pop();
                break;
            }

            case ASTKind::SWAPEXPR:
            {
                if (s.size() < 2)
                    typeError("swap: Insufficient stack elements");

                auto rhs = s.pop();
                auto lhs = s.pop();
                s.push(rhs);
                s.push(lhs);
                break;
            }

            case ASTKind::ROTEXPR:
            {
                //stack.assertMinSize(3, exp->line);
                if (s.size() < 3)
                    typeError("rot requires 3 stack elements.");

                auto c = s.pop();
                auto b = s.pop();
                auto a = s.pop();
                s.push(b);
                s.push(c);
                s.push(a);
                break;
            }

            case ASTKind::OVEREXPR:
            {
                //stack.assertMinSize(2, exp->line);

                if (s.size() < 2)
                    typeError("over requires 2 stack elements");

                auto b = s.pop();
                auto a = s.pop();
                s.push(a);
                s.push(b);
                s.push(a);
                break;
            }

            default:
                std::cout << "Not implemented:" << exp->line <<  ": " << ((int)exp->getASTKind()) << std::endl;
                exit(1);
                break;
        }
    }
}   

void Typechecker::typeError(std::string msg)
{
    std::cout << "TypeError: " << msg << "at " << filepath << ":" << line << std::endl;
    throw new std::exception();
}

void Typechecker::setPath(std::string fp)
{
    filepath = fp;
}

void Typechecker::typecheckContract(ProcCmd *p, TypeStack& s)
{

}

