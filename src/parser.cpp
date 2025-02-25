#include "parser.h"

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
        std::cout << "Error:" << t.line << ": token types differ: " << std::to_string((int)tt)
            << ":" << t.content << "(" << std::to_string((int)t.type) << ")" << std::endl;
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
            std::cout << "Error: not a type: " << t.toString() << std::endl;
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
        TokenType::RESET, TokenType::MEMORY, TokenType::ASSERT, TokenType::ADDROF
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
                subexps.push_back(new CharExpr(t.content[1]));
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
                auto e = new AddrOfExpr();
                e->line = t.line;
                subexps.push_back(e);
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
                auto e = new StringLitExpr(t.content + "\\0");
                e->line = t.line;
                subexps.push_back(e);
                break;
            }
            case TokenType::STRING:
            {
                auto e = new StringLitExpr(t.content);
                e->line = t.line;
                subexps.push_back(new StringLitExpr(t.content));
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

ConstCmd *Parser::parseConst()
{
    index++;
    Token identToken = pop();
    check(identToken, TokenType::VAR);
    std::string ident = identToken.content;
    std::vector<Expr *> expr = parseExpr();
    //index--;
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
    std::vector<Expr*> path = parseExpr(); // maybe dont use this.
    if (path.size() != 1 || path[0]->getASTKind() != ASTKind::STRINGLITEXPR) {

        for (auto a : path)
            std::cout << a->toString() << std::endl;

        error("Error: expected string.");
    }

    //check(peek(), TokenType::NEWLINE);
    
    std::string p = ((StringLitExpr *)path[0])->getValue();
    delete path[0];

    return new IncludeCmd(p);
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

