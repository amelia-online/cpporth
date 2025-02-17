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
        std::cout << "Error: token types differ: " << std::to_string((int)t.type) << ":" << t.content << std::endl;
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
    if (peek().type == TokenType::IFSTAR)
        next = parseIf();
    else check(pop(), TokenType::END);
    return new IfExpr(body, elze, next);
}

std::vector<Expr *> Parser::parseExpr()
{
    std::vector<Expr *> subexps;
    Token t = peek();
    std::vector<TokenType> allowed = { TokenType::INTVAL,  TokenType::TRUE, 
        TokenType::FALSE, TokenType::VAR,  TokenType::OP,
        TokenType::CHAR,  TokenType::STRING,TokenType::CSTRING,
        TokenType::DROP, TokenType::SWAP, TokenType::OVER,
        TokenType::DUP, TokenType::ROT, TokenType::HERE,
        TokenType::MAX, TokenType::MIN, TokenType::PRINT,
        TokenType::SYSCALLN, TokenType::NEWLINE, 
        TokenType::WHILE, TokenType::IF, TokenType::LET
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
                subexps.push_back(new IntExpr((long)std::stoi(t.content)));
                break;
            case TokenType::TRUE:
                subexps.push_back(new TrueExpr());
                break;
            case TokenType::FALSE:
                subexps.push_back(new FalseExpr());
                break;
            case TokenType::CSTRING:
                // todo
            case TokenType::STRING:
                subexps.push_back(new StringLitExpr(t.content));
                break;
            case TokenType::WHILE:
                subexps.push_back(parseWhile());
                break;
            case TokenType::IF:
                subexps.push_back(parseIf());
                break;
            case TokenType::OP:
                subexps.push_back(new OpExpr(t.content));
                break;
            case TokenType::LET:
                subexps.push_back(parseLet());
                break;
            default:
                subexps.push_back(new VarExpr(t.content));   
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
        else if (t.type == TokenType::BIKESHEDDER) { in = false; index++; }

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
                asts.push_back(parseConst());
                break;
            
            case TokenType::MEMORY:
                asts.push_back(parseMemory());
                break;

            case TokenType::INLINE:
                index++;
            case TokenType::PROC:
                asts.push_back(parseProc());
                break;

            case TokenType::INCLUDE:
                break;

            default:
                std::cout << "Not implemented: " << token.toString() << std::endl;
                throw new std::exception();
                break;
        }

    }
endloop:

    return asts;
}

