#include "runtime.h"
#include "helper.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>

Env::Env(int argc, char** argv)
{
    variables.insert(std::make_pair("argc", Data(argc, TypeKind::INT)));
    // todo: argv
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

void Data::assertType(TypeKind t) const
{
    if (type != t)
    {
        std::cout << "Error: Type mismatch.\n";
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

void Stack::assertMinSize(int s)
{
    if (data.size() < s)
    {
        std::cout << "Error: operation requires at least " << s << " items\n";
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
                env.variables.insert(std::make_pair(c->ident, interpExpr(c->body, s, env)));
                break;
            }
            case ASTKind::INCLUDECMD:
            {
                auto ic = (IncludeCmd *)ast;
                include(realString(ic->path), env);
                break;
            }

            case ASTKind::MEMORYCMD:
            {
                auto memcmd = (MemoryCmd *)ast;
                Stack s;
                env.variables.insert(std::make_pair(memcmd->ident, interpExpr(memcmd->body, s, env)));
                env.variables[memcmd->ident].assertType(TypeKind::PTR);
                break;
            }
            case ASTKind::ASSERTCMD:
            {
                auto ac = (AssertCmd *)ast;
                Stack s;
                auto res = interpExpr(ac->body, s, env);
                res.assertType(TypeKind::BOOL);
                if (res.isFalse())
                    std::cout << ac->msg << std::endl;
                break;
            }
            default:
                std::cout << "Error: not a command: " << ast->toString() << std::endl;
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
            case ASTKind::CONSTCMD:
            {
                Stack s;
                ConstCmd *c = (ConstCmd *)ast;
                env.variables.insert(std::make_pair(c->ident, interpExpr(c->body, s, env)));
                break;
            }
            case ASTKind::INCLUDECMD:
            {
                auto ic = (IncludeCmd *)ast;
                include(realString(ic->path), env);
                break;
            }
            case ASTKind::MEMORYCMD:
            {
                auto memcmd = (MemoryCmd *)ast;
                Stack s;
                env.variables.insert(std::make_pair(memcmd->ident, interpExpr(memcmd->body, s, env)));
                env.variables[memcmd->ident].assertType(TypeKind::PTR);
                break;
            }
            case ASTKind::ASSERTCMD:
            {
                auto ac = (AssertCmd *)ast;
                Stack s;
                auto res = interpExpr(ac->body, s, env);
                res.assertType(TypeKind::BOOL);
                if (res.isFalse())
                    std::cout << ac->msg << std::endl;
                break;
            }
            default:
                std::cout << "Error: not a command: " << ast->toString() << std::endl;
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
        //std::cout << exp->toString() << std::endl;
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
                    interpExpr(env.procs.at(v->name)->body, stack, env);
                }
                else if (env.variables.find(v->name) != env.variables.end())
                    stack.push(env.variables.at(v->name));
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
                auto str = s->getValue();
                int len = str.length();
                stack.push((long)len);
                char *ptr = new char[len];
                std::strncpy(ptr, str.data(), len);
                stack.push(ptr);
                break;
            }

            case ASTKind::IFEXPR:
            {
                auto f = (IfExpr *)exp;

                if (stack.peek().isTrue())
                    interpExpr(f->then, stack, env);
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
                stack.assertMinSize(let->idents.size());
                for (int i = let->idents.size()-1; i > 0; i--)
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
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    stack.push(lhs.getValue() + rhs.getValue());
                }

                else if (op->op == "-")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    stack.push(lhs.getValue() - rhs.getValue());
                }

                else if (op->op == "*")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    stack.push(lhs.getValue() * rhs.getValue());
                }

                else if (op->op == "divmod")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    stack.push(lhs.getValue() / rhs.getValue());
                    stack.push(lhs.getValue() % rhs.getValue());
                }

                // COMPARISON

                else if (op->op == "<")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    if (lhs.getValue() < rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == ">")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    if (lhs.getValue() > rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == "<=")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    if (lhs.getValue() <= rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == ">=")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    rhs.assertType(TypeKind::INT);
                    lhs.assertType(TypeKind::INT);
                    if (lhs.getValue() >= rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == "=")
                {
                    stack.assertMinSize(2);
                    auto rhs = stack.pop();
                    auto lhs = stack.pop();
                    if (lhs.getValue() == rhs.getValue())
                        stack.push(true);
                    else
                        stack.push(false);
                }

                else if (op->op == "!=")
                {
                    stack.assertMinSize(2);
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
                    stack.assertMinSize(2);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT);
                    a.assertType(TypeKind::INT);
                    stack.push(a.getValue() >> b.getValue());
                    break;
                }

                else if (op->op == "shl")
                {
                    stack.assertMinSize(2);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT);
                    a.assertType(TypeKind::INT);
                    stack.push(a.getValue() << b.getValue());
                    break;
                }

                else if (op->op == "or")
                {
                    stack.assertMinSize(2);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT);
                    a.assertType(TypeKind::INT);
                    stack.push(a.getValue() | b.getValue());
                    break;
                }

                else if (op->op == "and")
                {
                    stack.assertMinSize(2);
                    auto b = stack.pop();
                    auto a = stack.pop();
                    b.assertType(TypeKind::INT);
                    a.assertType(TypeKind::INT);
                    stack.push(a.getValue() & b.getValue());
                    break;
                }

                else if (op->op == "not")
                {
                    stack.assertMinSize(1);
                    auto a = stack.pop();
                    a.assertType(TypeKind::INT);
                    stack.push(~a.getValue());
                    break;
                }

                // MEMOPS

                // todo

                // CAST

                else if (op->op == "cast(bool)")
                {
                    stack.assertMinSize(1);
                    auto a = stack.pop();
                    if (a.getValue() > 0)
                        stack.push(true);
                    else
                        stack.push(false);
                    break;
                }

                else if (op->op == "cast(int)")
                {
                    stack.assertMinSize(1);
                    auto a = stack.pop();
                    stack.push(Data(a.getValue(), TypeKind::INT));
                    break;
                }

                else if (op->op == "cast(ptr)")
                {
                    stack.assertMinSize(1);
                    auto a = stack.pop();
                    stack.push(Data(a.getValue(), TypeKind::PTR));
                    break;
                }

                break;
            }

            case ASTKind::PRINTEXPR:
                stack.assertMinSize(1);
                std::cout << stack.pop().getValue() << std::endl;
                break;

            case ASTKind::DUPEXPR:
            {
                stack.assertMinSize(1);
                stack.push(stack.peek());
                break;
            }

            case ASTKind::MAXEXPR:
            {
                stack.assertMinSize(2);
                auto rhs = stack.pop();
                auto lhs = stack.pop();
                rhs.assertType(TypeKind::INT);
                lhs.assertType(TypeKind::INT);
                stack.push(std::max(lhs.getValue(), rhs.getValue()));
                break;
            }

            case ASTKind::DROPEXPR:
            {
                stack.assertMinSize(1);
                stack.pop();
                break;
            }

            case ASTKind::SWAPEXPR:
            {
                stack.assertMinSize(1);
                auto rhs = stack.pop();
                auto lhs = stack.pop();
                stack.push(rhs);
                stack.push(lhs);
                break;
            }

            case ASTKind::ROTEXPR:
            {
                stack.assertMinSize(3);
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
                stack.assertMinSize(2);
                auto b = stack.pop();
                auto a = stack.pop();
                stack.push(a);
                stack.push(b);
                stack.push(a);
                break;
            }

            default:
                std::cout << "Not implemented: " << ((int)exp->getASTKind()) << std::endl;
        }
    }

    return stack.top();
}