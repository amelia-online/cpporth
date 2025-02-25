#include "runtime.h"
#include "helper.h"
#include "lexer.h"
#include "parser.h"
#include "syscalls.h"
#include <iostream>
#include <algorithm>

Env::Env(int argc, char** argv)
{
    variables.insert(std::make_pair("argc", Data(argc, TypeKind::INT)));

    variables.insert(std::make_pair("argv", Data((long)argv, TypeKind::PTR)));
    filepath = argv[1];
    
}

Env::Env(const Env& other)
{
    variables = std::unordered_map<std::string, Data>(other.variables);
    procs = std::unordered_map<std::string, ProcCmd*>(other.procs);
    included = std::vector<std::string>(other.included);
    offset = other.offset;
    filepath = other.filepath;
}

Env::~Env()
{
    for (auto a : toClean)
        delete[] a;
}

Env& Env::operator=(Env other)
{
    std::swap(variables, other.variables);
    std::swap(procs, other.procs);
    std::swap(offset, other.offset);
    std::swap(filepath, other.filepath);
    return *this;
}

Env& Env::operator+=(Env other)
{
    std::swap(variables, other.variables);
    std::swap(procs, other.procs);
    std::swap(included, other.included);
    return *this;
}

Data::Data() : value(-1), type(TypeKind::ADDR), isNone_(true) {;}
Data::Data(long val, TypeKind t) : value(val), type(t), isNone_(false) {;}

TypeKind Data::getType() const
{
    return type;
}

bool Data::isNone()
{
    return isNone_;
}

bool Data::isTrue()
{
    return type == TypeKind::BOOL && value == 1;
}

bool Data::isPtr()
{
    return type == TypeKind::PTR;
}

bool Data::isInt()
{
    return type == TypeKind::INT;
}

bool Data::isFalse()
{
    return type == TypeKind::BOOL && value == 0;
}

void Data::assertType(TypeKind t, int line) const
{
    if (type != t)
    {
        std::cout << "Error:" << line << ": Type mismatch.\n";
        throw new std::exception();
    }
}

long Data::getValue() const
{
    return value;
}

void Stack::push(long l)
{
    Data d(l, TypeKind::INT);
    data.push_back(d);
}

Stack Stack::scope(const ProcCmd *pcmd)
{
    Stack s;
    int incount = pcmd->sig.params.size();
    return s;
}

std::string Stack::toString()
{
    std::string acc = "[";
    int idx = 0;
    for (auto d : data)
    {
        acc += std::to_string(d.getValue()) + (idx < data.size()-1 ? " " : "");
        idx++;
    }
    acc += "]";

    return acc;
}

void Stack::clear()
{
    data.clear();
}

std::vector<Data> Stack::toVector() const
{
    std::vector<Data> res;
    for (auto i : data)
        res.push_back(i);
    return res;
}

void Stack::append(const Stack& other)
{
    std::vector<Data> items = other.toVector();
    for (auto i : items)
        data.push_back(i);
}

bool Stack::isEmpty()
{
    return data.size() == 0;
}

void Stack::assertMinSize(int s, int line)
{
    if (data.size() < s)
    {
        std::cout << "Error:" << line << ": operation requires at least " << s << " items\n";
        throw new std::exception();
    }
}

Data Stack::top()
{
    if (data.size() == 0)
        return Data();
    return peek();
}

void Stack::push(Data d)
{
    data.push_back(d);
}

void Stack::push(bool b)
{
    data.push_back(Data((long)b, TypeKind::BOOL));
}

void Stack::push(void *ptr)
{
    data.push_back(Data((long)ptr, TypeKind::PTR));
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

void include(std::string path, Env& env)
{
    std::string contents = openFile(path);
    Lexer lexer(contents);
    Parser parser(lexer.lex());

    std::vector<AST*> prog = parser.parse();

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
                long offs = (long)env.offset;
                auto res = interpExpr(c->body, s, env);
                Data d(res.getValue() + offs, res.getType());
                env.variables.insert(std::make_pair(c->ident, d));
                break;
            }

            case ASTKind::INCLUDECMD:
            {
                auto ic = (IncludeCmd *)ast;

                if (std::find(env.included.begin(), env.included.end(), realString(ic->path)) == env.included.end())
                    env.included.push_back(realString(ic->path));
                else
                    break;

                Env e2(env);
                e2.filepath = realString(ic->path);
                include(realString(ic->path), e2);
                env += e2;
                break;
            }

            case ASTKind::MEMORYCMD:
            {
                auto memcmd = (MemoryCmd *)ast;
                Stack s;
                long size = interpExpr(memcmd->body, s, env).getValue();
                unsigned char *m = new unsigned char[size]();
                env.variables.insert(std::make_pair(memcmd->ident, Data((long)m, TypeKind::PTR)));
                break;
            }
            case ASTKind::ASSERTCMD:
            {
                auto ac = (AssertCmd *)ast;
                Stack s;
                auto res = interpExpr(ac->body, s, env);
                res.assertType(TypeKind::BOOL, ac->line);
                if (res.isFalse())
                    std::cout << ac->msg << std::endl;
                break;
            }
            default:
                std::cout << "Error:" << ast->line << ": not a command: " << ast->toString() << std::endl;
                throw new std::exception();
        }
    }
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
            case ASTKind::CONSTCMD: // fix offset stuff
            {
                Stack s;
                ConstCmd *c = (ConstCmd *)ast;
                long offs = (long)env.offset;
                auto res = interpExpr(c->body, s, env);
                Data d((!res.isNone() ? res.getValue() : 0) + offs, res.getType());
                env.variables.insert(std::make_pair(c->ident, d));
                break;
            }
            case ASTKind::INCLUDECMD:
            {
                auto ic = (IncludeCmd *)ast;

                if (std::find(env.included.begin(), env.included.end(), realString(ic->path)) == env.included.end())
                    env.included.push_back(realString(ic->path));
                else
                    break;


                Env e2(env);
                e2.filepath = realString(ic->path);
                include(realString(ic->path), e2);
                env += e2;
                break;
            }
            case ASTKind::MEMORYCMD:
            {
                auto memcmd = (MemoryCmd *)ast;
                Stack s;
                long size = interpExpr(memcmd->body, s, env).getValue();
               //\ std::cout << size << ast->line <<  std::endl;
                unsigned char *m = new unsigned char[size]();
                env.variables.insert(std::make_pair(memcmd->ident, Data((long)m, TypeKind::PTR)));
                break;
            }
            case ASTKind::ASSERTCMD:
            {
                auto ac = (AssertCmd *)ast;
                Stack s;
                auto res = interpExpr(ac->body, s, env);
                res.assertType(TypeKind::BOOL, ac->line);
                if (res.isFalse())
                    std::cout << ac->msg << std::endl;
                break;
            }
            default:
                std::cout << "Error:" << ast->line << ": not a command: " << ast->toString() << std::endl;
                throw new std::exception();
        }
    }

    if (env.procs.find("main") == env.procs.end())
    {
        std::cout << "Error: Main function not found.\n";
        throw new std::exception();
    }

    return interpExpr(env.procs.at("main")->body, stack, env);
}

Data interpExpr(std::vector<Expr*> exps, Stack& stack, Env& env)
{
    for (auto exp : exps)
    {
        //std::cout << stack.toString() << " " << exp->toString() << std::endl;

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
                {
                    Env e2(env);
                    interpExpr(env.procs.at(v->name)->body, stack, e2);
                }
                else if (env.variables.find(v->name) != env.variables.end())
                    stack.push(env.variables.at(v->name));
                else
                {
                    std::cout << "Error:" << exp->line << ": Unknown identifier encountered: '" << v->name << "'\n";
                    throw new std::exception();
                }
                break;
            }

            case ASTKind::WHILEEXPR:
            {
                WhileExpr *w = (WhileExpr *)exp;
                while (interpExpr(w->cond, stack, env).isTrue()) 
                {
                    stack.pop();
                    interpExpr(w->body, stack, env);
                }

                break;
            }

            case ASTKind::STRINGLITEXPR:
            {
                StringLitExpr *s = (StringLitExpr *)exp;
                auto str = realString(s->getValue());
                int len = str.length();
                stack.push((long)len);
                char *ptr = new char[len];
                std::strncpy(ptr, str.data(), len);
                stack.push(ptr);
                break;
            }

            case ASTKind::CHAREXPR:
            {
                auto e = (CharExpr *)exp;
                stack.push((long)e->getValue());
                break;
            }

            case ASTKind::SYSCALLEXPR:
            {
                auto e = (SyscallExpr *)exp;
                
                stack.assertMinSize(e->getNumArgs()+1, exp->line);
                stack.peek().assertType(TypeKind::INT, exp->line);
                int sysnum = stack.pop().getValue() & 0xFFFFFF;

                std::vector<long> args;

                for (int i = 0; i < e->getNumArgs(); i++)
                    args.push_back(stack.pop().getValue());
                
                stack.push(syscall(sysnum, args));
                break;
            }

            case ASTKind::MEMORYEXPR:
            {
                auto ex = (MemoryExpr *)exp;
                Stack sta;
                auto s = interpExpr(ex->body, sta, env);                
                auto ptr = new unsigned char[s.getValue()];
                //std::cout << ex->getIdent() << std::endl;
                env.variables.insert(std::make_pair(ex->getIdent(), Data((long)ptr, TypeKind::PTR)));
                env.toClean.push_back(ptr);
                break;
            }

            case ASTKind::IFEXPR:
            {
                auto f = (IfExpr *)exp;

                if (stack.pop().isTrue()) 
                {
                    //stack.pop();
                    interpExpr(f->then, stack, env);
                    break;
                }
                else if (f->elze.size() != 0)
                    interpExpr(f->elze, stack, env);

                if (f->next && f->elze.size() > 0)
                {
                    std::vector<Expr*> expr = {f->next};
                    interpExpr(expr, stack, env);
                }
            
                break;
            }

            case ASTKind::LETSTMT:
            {
                auto let = (LetExpr *)exp;
                stack.assertMinSize(let->idents.size(), let->line);
                for (int i = let->idents.size()-1; i >= 0; i--)
                    env.variables.insert(std::make_pair(let->idents[i], stack.pop()));
                
                interpExpr(let->body, stack, env);

                for (auto ident : let->idents)
                    env.variables.erase(ident);

                break;
            }

            case ASTKind::PEEKSTMT:
            {
                // TODO: change to not modify stack.
                auto let = (LetExpr *)exp;
                stack.assertMinSize(let->idents.size(), let->line);
                for (int i = let->idents.size()-1; i >= 0; i--)
                    env.variables.insert(std::make_pair(let->idents[i], stack.pop()));
                
                interpExpr(let->body, stack, env);

                for (auto ident : let->idents)
                    env.variables.erase(ident);

                break;
            }

            case ASTKind::OPEXPR:
            {
                auto op = (OpExpr *)exp;

                // ARITHMETIC
                
                if (op->op == "+")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    stack.push(lhs.getValue() + rhs.getValue());
                }

                else if (op->op == "-")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    stack.push(lhs.getValue() - rhs.getValue());
                }

                else if (op->op == "*")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    stack.push(lhs.getValue() * rhs.getValue());
                }

                else if (op->op == "divmod")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    stack.push(lhs.getValue() / rhs.getValue());
                    stack.push(lhs.getValue() % rhs.getValue());
                }

                // COMPARISON

                else if (op->op == "<")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    if (lhs.getValue() < rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == ">")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    if (lhs.getValue() > rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == "<=")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    if (lhs.getValue() <= rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == ">=")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT, op->line);
                    lhs.assertType(TypeKind::INT, op->line);
                    if (lhs.getValue() >= rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == "=")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    if (lhs.getValue() == rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == "!=")
                {
                    stack.assertMinSize(2, op->line);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    if (lhs.getValue() != rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                // BITWISE

                else if (op->op == "shr")
                {
                    stack.assertMinSize(2, op->line);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT, op->line);
                    a.assertType(TypeKind::INT, op->line);
                    stack.push(a.getValue() >> b.getValue());
                    break;
                }

                else if (op->op == "shl")
                {
                    stack.assertMinSize(2, op->line);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT, op->line);
                    a.assertType(TypeKind::INT, op->line);
                    stack.push(a.getValue() << b.getValue());
                    break;
                }

                else if (op->op == "or")
                {
                    stack.assertMinSize(2, op->line);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT, op->line);
                    a.assertType(TypeKind::INT, op->line);
                    stack.push(a.getValue() | b.getValue());
                    break;
                }

                else if (op->op == "and")
                {
                    stack.assertMinSize(2, op->line);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT, op->line);
                    a.assertType(TypeKind::INT, op->line);
                    stack.push(a.getValue() & b.getValue());
                    break;
                }

                else if (op->op == "not")
                {
                    stack.assertMinSize(1, op->line);
                    auto a = stack.pop();
                    a.assertType(TypeKind::INT, op->line);
                    stack.push(~a.getValue());
                    break;
                }

                // MEMOPS

                else if (op->op == "!8")
                {
                    stack.assertMinSize(2, op->line);
                    auto ptr = stack.pop();
                    auto byte = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    byte.assertType(TypeKind::INT, op->line);
                    *((unsigned char *)ptr.getValue()) = byte.getValue() & 0xFF;
                    break;
                }

                else if (op->op == "@8")
                {
                    stack.assertMinSize(1, op->line);
                    auto ptr = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    long byte = (long)*((unsigned char *)ptr.getValue());
                    stack.push(byte);
                    break;
                }

                else if (op->op == "!16")
                {
                    stack.assertMinSize(2, op->line);
                    auto ptr = stack.pop();
                    auto byte = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    byte.assertType(TypeKind::INT, op->line);
                    *((unsigned short *)ptr.getValue()) = byte.getValue() & 0xFF;
                    break;
                }

                else if (op->op == "@16")
                {
                    stack.assertMinSize(1, op->line);
                    auto ptr = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    long byte = (long)*((unsigned short *)ptr.getValue());
                    stack.push(byte);
                    break;
                }


                else if (op->op == "!32")
                {
                    stack.assertMinSize(2, op->line);
                    auto ptr = stack.pop();
                    auto byte = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    byte.assertType(TypeKind::INT, op->line);
                    *((unsigned int *)ptr.getValue()) = byte.getValue() & 0xFF;
                    break;
                }

                else if (op->op == "@32")
                {
                    stack.assertMinSize(1, op->line);
                    auto ptr = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    long byte = (long)*((unsigned int *)ptr.getValue());
                    stack.push(byte);
                    break;
                }

                else if (op->op == "!64")
                {
                    stack.assertMinSize(2, op->line);
                    auto ptr = stack.pop();
                    auto byte = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    byte.assertType(TypeKind::INT, op->line);
                    *((unsigned long *)ptr.getValue()) = byte.getValue() & 0xFF;
                    break;
                }

                else if (op->op == "@64")
                {
                    stack.assertMinSize(1, op->line);
                    auto ptr = stack.pop();
                    ptr.assertType(TypeKind::PTR, op->line);
                    long byte = (long)*((unsigned long *)ptr.getValue());
                    stack.push(byte);
                    break;
                }

                // CAST

                else if (op->op == "cast(bool)")
                {
                    stack.assertMinSize(1, op->line);
                    auto a = stack.pop();
                    if (a.getValue() > 0)
                        stack.push(true);
                    else
                        stack.push(false);
                    break;
                }

                else if (op->op == "cast(int)")
                {
                    stack.assertMinSize(1, op->line);
                    auto a = stack.pop();
                    stack.push(Data(a.getValue(), TypeKind::INT));
                    break;
                }

                else if (op->op == "cast(ptr)")
                {
                    stack.assertMinSize(1, op->line);
                    auto a = stack.pop();
                    stack.push(Data(a.getValue(), TypeKind::PTR));
                    break;
                }

                break;
            }

            case ASTKind::PRINTEXPR:
                stack.assertMinSize(1, exp->line);
                std::cout << stack.pop().getValue() << std::endl;
                break;

            case ASTKind::DUPEXPR:
            {
                stack.assertMinSize(1, exp->line);
                stack.push(stack.peek());
                break;
            }

            case ASTKind::HEREEXPR:
            {
                std::string s = env.filepath;
                s += ":";
                s += std::to_string(exp->line);
                char* here = new char[s.length()];
                std::strncpy(here, s.data(), s.length());
                stack.push((long)s.length());
                stack.push(here);
                break;
            }

            case ASTKind::OFFSETEXPR:
            {
                stack.assertMinSize(1, exp->line);
                auto i = stack.pop();
                env.offset += i.getValue();
                break;
            }

            case ASTKind::RESETEXPR:
            {
                env.offset = 0;
                break;
            }

            case ASTKind::MAXEXPR:
            {
                stack.assertMinSize(2, exp->line);
                auto rhs = stack.pop();
                auto lhs = stack.pop();
                rhs.assertType(TypeKind::INT, exp->line);
                lhs.assertType(TypeKind::INT, exp->line);
                stack.push(std::max(lhs.getValue(), rhs.getValue()));
                break;
            }

            case ASTKind::DROPEXPR:
            {
                stack.assertMinSize(1, exp->line);
                stack.pop();
                break;
            }

            case ASTKind::SWAPEXPR:
            {
                stack.assertMinSize(1, exp->line);
                auto rhs = stack.pop();
                auto lhs = stack.pop();
                stack.push(rhs);
                stack.push(lhs);
                break;
            }

            case ASTKind::ROTEXPR:
            {
                stack.assertMinSize(3, exp->line);
                auto c = stack.pop();
                auto b = stack.pop();
                auto a = stack.pop();
                stack.push(b);
                stack.push(c);
                stack.push(a);
                break;
            }

            case ASTKind::OVEREXPR:
            {
                stack.assertMinSize(2, exp->line);
                auto b = stack.pop();
                auto a = stack.pop();
                stack.push(a);
                stack.push(b);
                stack.push(a);
                break;
            }

            default:
                std::cout << "Not implemented:" << exp->line <<  ": " << ((int)exp->getASTKind()) << std::endl;
                exit(1);
                break;
        }
    }

    return stack.top();
}