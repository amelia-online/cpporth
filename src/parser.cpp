#include "parser.h"
#include "helper.h"
#include <unordered_map>
#include <utility>


void todo()
{
    std::cout << "Error: not implemented." << std::endl;
    throw new std::exception();
}

void error(std::string msg)
{
    std::cout << msg << std::endl;
    throw new std::exception();
}

void check(Token t, TokenType tt)
{
    if (t.type != tt)
    {
        std::cout << "ParseError:" << t.line << ": token types differ: expected " << std::to_string((int)tt)
            << " but got " << t.content << "(" << std::to_string((int)t.type) << ")" << std::endl;
        throw new std::exception();
    }
}

Parser::Parser(std::vector<Token> input) : input(input), index(0) {;}
void Parser::cleanup(std::vector<AST*> asts)
{
    for (AST *a : asts)
        delete a;
}

Token Parser::peek()
{
    return input[index];
}

Token Parser::pop()
{
    Token res = input[index];
    index++;
    return res;
}

Token Parser::ahead()
{
    if (index + 1 < input.size())
        return input[index + 1];
    else
    {
        std::cout << "Error: ahead: out of bounds." << std::endl;
        throw new std::exception();
    }
}

Token Parser::behind()
{
    if (index > 0)
        return input[index - 1];
    else
    {
        std::cout << "Error: behind: out of bounds." << std::endl;
        throw new std::exception();
    }
}

Type Parser::parseType()
{
    Token t = pop();
    switch (t.type)
    {
        case TokenType::INT:
            return Type(TypeKind::INT);
        case TokenType::BOOL:
            return Type(TypeKind::BOOL);
        case TokenType::PTR:
            return Type(TypeKind::PTR);
        case TokenType::ADDR:
            return Type(TypeKind::ADDR);
        default:
            std::cout << "ParseError:" << t.line << ":  not a type: " << t.toString() << std::endl;
            throw new std::exception();
    }
}

LetExpr *Parser::parseLet()
{
    std::vector<std::string> idents;
    Token t = peek();

    while (index < input.size())
    {
        index++;
        t = peek();

        if (t.type == TokenType::IN)
            break;

        if (t.type == TokenType::VAR)
            idents.push_back(t.content);
        else if (t.type == TokenType::NEWLINE)
            continue;
        else error("Error: expected identifer.");
    }

    index++;
    std::vector<Expr*> body = parseExpr();
    check(pop(), TokenType::END);
    
    return new LetExpr(idents, body);
}

PeekExpr *Parser::parsePeek()
{
    std::vector<std::string> idents;
    Token t = peek();

    while (index < input.size())
    {
        index++;
        t = peek();

        if (t.type == TokenType::IN)
            break;

        if (t.type == TokenType::VAR)
            idents.push_back(t.content);
        else if (t.type == TokenType::NEWLINE)
            continue;
        else error("Error: expected identifer.");
    }

    index++;
    std::vector<Expr*> body = parseExpr();
    check(pop(), TokenType::END);
    
    return new PeekExpr(idents, body);
}

WhileExpr *Parser::parseWhile()
{
    index++;
    std::vector<Expr *> cond = parseExpr();
    //index--;
    check(pop(), TokenType::DO);
    std::vector<Expr *> body = parseExpr();
    //index--;
    check(pop(), TokenType::END);
    return new WhileExpr(cond, body);
}

IfExpr *Parser::parseIf()
{
    index++;
    std::vector<Expr*> elze;

    std::vector<Expr*> body = parseExpr();

    if (peek().type == TokenType::END)
    {
        index++;
        return new IfExpr(body, elze, nullptr);
    } else check(pop(), TokenType::ELSE);

    elze = parseExpr();
    IfExpr *next = nullptr;

    //std::cout << peek().content << std::endl;
    if (peek().type == TokenType::IFSTAR) {
        next = parseIf();
    }
    else check(pop(), TokenType::END);

    return new IfExpr(body, elze, next);
}

AddrOfExpr *Parser::parseAddrOf()
{
    index++;
    check(peek(), TokenType::VAR);
    auto v = new VarExpr(peek().content);
    index++;
    return new AddrOfExpr(v);
}

CallLikeExpr *Parser::parseCallLike()
{
    index++;
    check(peek(), TokenType::VAR);
    auto v = new VarExpr(peek().content);
    index++;
    return new CallLikeExpr(v);
}

AssertExpr *Parser::parseAssert()
{
    index++;
    check(peek(), TokenType::STRING);
    std::string msg = pop().content;
    std::vector<Expr*> body = parseExpr();
    check(pop(), TokenType::END);
    return new AssertExpr(msg, body);
}

std::vector<Expr *> Parser::parseExpr()
{
    std::vector<Expr *> subexps;
    Token t = peek();
    std::vector<TokenType> allowed = { TokenType::INTVAL,
        TokenType::VAR,  TokenType::OP,
        TokenType::CHAR,  TokenType::STRING, TokenType::CSTRING,
        TokenType::DROP, TokenType::SWAP, TokenType::OVER,
        TokenType::DUP, TokenType::ROT, TokenType::HERE,
        TokenType::MAX, TokenType::PRINT,
        TokenType::SYSCALLN, TokenType::NEWLINE, TokenType::PEEK,
        TokenType::WHILE, TokenType::IF, TokenType::LET, TokenType::OFFSET,
        TokenType::RESET, TokenType::MEMORY, TokenType::ASSERT, 
        TokenType::ADDROF, TokenType::CALLLIKE, TokenType::FREE,
        TokenType::ALLOC, TokenType::NEW, TokenType::MATCH,
    };

    while (std::find(allowed.begin(), allowed.end(), t.type) != allowed.end())
    {
        t = peek();

        if (std::find(allowed.begin(), allowed.end(), t.type) == allowed.end())
            break;

        //std::cout << "Parsing: " << t.toString() << std::endl;
        switch (t.type)
        {
            case TokenType::NEWLINE:
                break;
            case TokenType::INTVAL:
            {
                subexps.push_back(new IntExpr((long)std::stol(t.content)));
                break;
            }
            case TokenType::CHAR:
            {
                subexps.push_back(new CharExpr(realChar(t.content)));
                break;
            }
            case TokenType::NEW:
            {
                auto n = parseNew();
                n->line = t.line;
                subexps.push_back(n);
                index--;
                break;
            }
            case TokenType::ALLOC:
            {
                auto a = new AllocExpr();
                a->line = t.line;
                subexps.push_back(a);
                break;
            }
            case TokenType::MATCH:
            {
                auto m = parseMatch();
                m->line = t.line;
                subexps.push_back(m);
                index--;
                break;
            }
            case TokenType::FREE:
            {
                auto f = new FreeExpr();
                f->line = t.line;
                subexps.push_back(f);
                break;
            }
            case TokenType::OFFSET:
            {
                auto e = new OffsetExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::RESET:
            {
                auto e = new ResetExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::SWAP:
            {
                auto e = new SwapExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::ADDROF:
            {
                auto e = parseAddrOf();
                e->line = t.line;
                subexps.push_back(e);
                index--;
                break;
            }
            case TokenType::CALLLIKE:
            {
                auto e = parseCallLike();
                e->line = t.line;
                subexps.push_back(e);
                index--;
                break;
            }
            case TokenType::ASSERT:
            {
                auto e = parseAssert();
                index--;
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::DROP:
            {
                auto e = new DropExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::DUP:
            {
                auto e = new DupExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::OVER:
            {
                auto e = new OverExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::ROT:
            {
                auto e = new RotExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::HERE:
            {
                auto e = new HereExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::MAX:
            {
                auto e = new MaxExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::SYSCALLN:
            {
                std::string n = "";
                n.push_back(t.content[t.content.length()-1]);
                auto e = new SyscallExpr(std::stoi(n));
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::PRINT:
            {
                auto e = new PrintExpr();
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::MEMORY:
            {
                MemoryCmd *m = parseMemory();
                MemoryExpr *res = m->toMemoryExpr();
                res->line = t.line;
                subexps.push_back(res);
                index--;
                break;
            }
            case TokenType::CSTRING:
            {
                t.content.pop_back();
                auto e = new StringLitExpr("\"" + realString(t.content) + "\\0\"", true);
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::STRING:
            {
                auto e = new StringLitExpr(t.content, false);
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::WHILE:
            {
                auto e = parseWhile();
                e->line = t.line;
                subexps.push_back(e);
                index--;
                break;
            }
            case TokenType::IF:
            {
                auto e = parseIf();
                e->line = t.line;
                subexps.push_back(e);
                index--;
                break;
            }
            case TokenType::OP:
            {
                auto e = new OpExpr(t.content);
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::LET:
            {
                auto e = parseLet();
                e->line = t.line;
                subexps.push_back(e);
                index--;
                break;
            }
            case TokenType::PEEK:
            {
                auto e = parsePeek();
                e->line = t.line;
                subexps.push_back(e);
                index--;
                break;
            }
            default:
            {
                auto e = new VarExpr(t.content);
                e->line = t.line;
                subexps.push_back(e);   
            }
        }

        index++;
    }
   
    return subexps;
}

Field Parser::parseField()
{
    check(peek(), TokenType::VAR);      // var
    std::string name = pop().content;   // :
    check(pop(), TokenType::COLON);     // :
    check(pop(), TokenType::COLON);     // type
    auto type = parseType();            // , OR )
    return Field(name, type);
}

Variant Parser::parseVariant(std::string parent)
{
    check(peek(), TokenType::VAR);
    auto name = pop().content;
    check(peek(), TokenType::LSQUARE);
    index++;

    std::vector<Field> fields;
    bool isField = true;
    while (index < input.size())
    {
        auto token = peek();

        if (token.type == TokenType::RSQUARE)
            break;

        if (token.type == TokenType::NEWLINE)
        {
            index++;
            continue;
        }
        
        if (isField)
        {
            fields.push_back(parseField());
            isField = false;
            continue;
        }
        else if (token.type == TokenType::COMMA)
        {
            isField = true;
            index++;
            continue;
        } 
        else
        {
            std::cout << "[Error] variant\n";
            throw new std::exception();
        }

    }
    index++;
    return Variant(name, parent, fields);
}   

VariantInstanceExpr *Parser::parseNew()
{
    index++;
    check(peek(), TokenType::VAR);
    std::string parent = pop().content;
    check(pop(), TokenType::COLON);
    check(pop(), TokenType::COLON);
    check(peek(), TokenType::VAR);
    std::string name = pop().content;
    std::vector<std::vector<Expr*> > args;
    check(pop(), TokenType::LSQUARE);

    bool isArg = true;
    while (index < input.size())
    {
        auto token = peek();

        if (token.type == TokenType::RSQUARE)
            break;

        if (token.type == TokenType::NEWLINE)
        {
            index++;
            continue;
        }

        if (isArg)
        {
            args.push_back(parseExpr());
            isArg = false;
        }
        else if (token.type == TokenType::COMMA)
        {
            isArg = true;
            index++;
            continue;
        }
        else throw new std::exception();
    }
    index++;
    return new VariantInstanceExpr(name, parent, args);
}

MatchExpr *Parser::parseMatch()
{
    index++;
    std::unordered_map<std::string, VariantBinding*> map;

    bool isBranch = false;
    while (index < input.size())
    {
        auto token = peek();

        if (token.type == TokenType::END)
            break;
        if (token.type == TokenType::NEWLINE)
        {
            index++;
            continue;
        }

        if (isBranch)
        {
            // todo...
        }
        else if (token.type == TokenType::LINE)
        {
            isBranch = true;
            index++;
            continue;
        }
        else throw new std::exception();
        
    }

}

TypeCmd *Parser::parseTypeCmd()
{
    index++;
    check(peek(), TokenType::VAR);
    std::string name = pop().content;
    std::vector<Variant> variants;

    bool isVariant = false;
    while (index < input.size())
    {
        auto token = peek();
        //std::cout << token.content << "\n";
        if (token.type == TokenType::END)
            break;

        if (token.type == TokenType::NEWLINE)
        {
            index++;
            continue;
        }

        if (isVariant)
        {
            //std::cout << "Parsing variant...\n";
            auto v = parseVariant(name);
            //std::cout << "Parsed: " << v.toString() << "\n";
            variants.push_back(v);
            isVariant = false;
            //std::cout << "...done parsing variant\n";
        } 
        else if (token.type == TokenType::LINE)
        {
            isVariant = true;
            index++;
            continue;
        }
        else
        {
            std::cout << "[Error] typecmd\n";
            throw new std::exception();
        }
    }

    index++;
    return new TypeCmd(name, variants);
}

ConstCmd *Parser::parseConst()
{
    index++;
    Token identToken = pop();
    check(identToken, TokenType::VAR);
    std::string ident = identToken.content;
    std::vector<Expr *> expr = parseExpr();
    check(pop(), TokenType::END);
    return new ConstCmd(ident, expr);
}
FnSignature Parser::parseSignature()
{
    std::vector<Type> ins;
    std::vector<Type> outs;
    Token t = peek();
    bool in = true;
    while (t.type != TokenType::IN)
    {
        t = peek();
        if (t.type == TokenType::IN) break;
        else if (t.type == TokenType::BIKESHEDDER) { in = false; index++; continue; }
        else if (t.type == TokenType::NEWLINE) {index++; continue;}

        Type type = parseType();

        if (in) ins.push_back(type);
        else outs.push_back(type);
    }
    index++;
    return FnSignature(ins, outs);
}

ProcCmd *Parser::parseProc()
{
    index++;
    check(peek(), TokenType::VAR);
    std::string ident = pop().content;
    FnSignature sig = parseSignature();
    std::vector<Expr*> body = parseExpr();
    check(pop(), TokenType::END);
    return new ProcCmd(ident, sig, body);
}

MemoryCmd *Parser::parseMemory()
{
    index++;
    Token t = peek();
    check(t, TokenType::VAR);
    std::string ident = t.content;
    index++;
    std::vector<Expr *> e = parseExpr();
    //index--;
    check(pop(), TokenType::END);
    return new MemoryCmd(ident, e);
}

// TODO: fix.
IncludeCmd *Parser::parseInclude()
{
    index++;
    check(peek(), TokenType::STRING);
    auto e = new IncludeCmd(realString(pop().content));
    //std::cout << e->toString();
    return e;
}

std::vector<AST*> Parser::parse()
{
    std::vector<AST*> asts;

    while (index < input.size())
    {
        Token token = peek();

        switch (token.type)
        {
            case TokenType::NEWLINE:
                index++;
                break;

            case TokenType::END_OF_FILE:
                goto endloop;
                break;

            case TokenType::CONST:
            {
                auto e = parseConst();
                e->line = token.line;
                asts.push_back(e);
                break;
            }

            case TokenType::MEMORY:
            {
                auto e = parseMemory();
                e->line = token.line;
                asts.push_back(e);
                break;
            }

            case TokenType::INLINE:
                index++;
            case TokenType::PROC:
            {
                auto e = parseProc();
                e->line = token.line;
                asts.push_back(e);
                break;
            }
            case TokenType::ASSERT:
            {
                auto e = parseAssert()->asCmd();
                e->line = token.line;
                asts.push_back(e);
                break;
            }   
            case TokenType::TYPE:
            {
                auto t = parseTypeCmd();
                t->line = token.line;
                asts.push_back(t);
                break;
            }
            case TokenType::INCLUDE:
            {
                auto e = parseInclude();
                e->line = token.line;
                asts.push_back(e);
                break;
            }
            default:
                std::cout << "Error:" << token.line << ": Not implemented: " << token.toString() << std::endl;
                throw new std::exception();
                break;
        }

    }
endloop:

    return asts;
}

