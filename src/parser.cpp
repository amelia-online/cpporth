#include "parser.h"

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

Expr *Parser::parseExpr()
{
    std::vector<Subexpr> subexps;
    Token t = peek();
    std::vector<TokenType> allowed = {TokenType::INTVAL,  TokenType::TRUE, 
        TokenType::FALSE, TokenType::VAR,  TokenType::OP,
        TokenType::CHAR,  TokenType::STRING,TokenType::CSTRING,
        TokenType::DROP, TokenType::SWAP, TokenType::OVER,
        TokenType::DUP, TokenType::ROT, TokenType::HERE,
        TokenType::MAX, TokenType::MIN, TokenType::PRINT,
        TokenType::SYSCALLN};

    while (std::find(allowed.begin(), allowed.end(), t.type) != allowed.end())
    {
        t = peek();
        
        switch (t.type)
        {
            case TokenType::INTVAL:
                subexps.push_back(Subexpr(std::stoi(t.content)));
                break;
            case TokenType::TRUE:
                subexps.push_back(Subexpr(true));
                break;
            case TokenType::FALSE:
                subexps.push_back(Subexpr(false));
                break;
            default:
                subexps.push_back(Subexpr(t.content));   
        }

        index++;
    }
    subexps.pop_back();
    return new Expr(subexps);
}

ConstCmd *Parser::parseConst()
{
    index++;
    Token identToken = pop();
    check(identToken, TokenType::VAR);
    std::string ident = identToken.content;
    Expr *expr = parseExpr();
    index--;
    check(pop(), TokenType::END);
    return new ConstCmd(ident, expr);
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

            default:
                std::cout << "Not implemented: " << token.toString() << std::endl;
                throw new std::exception();
                break;
        }

    }
endloop:

    return asts;
}

