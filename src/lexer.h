#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>

enum class TokenType
{
    NEWLINE = 0,
    PROC,
    IN,
    VAR,
    WHILE,
    END,
    IF,
    PRINT,
    INTVAL,
    INCLUDE,
    STRING,
    CSTRING,
    DROP,
    SWAP,
    DUP,
    ROT,
    OVER,
    OP,
    MAX,
    MEMORY,
    ELSE,
    DO,
    INT,
    INLINE,
    PTR,
    CONST,
    MEMOP,
    LET,
    PEEK,
    OFFSET,
    RESET,
    BOOL,
    ADDR,
    BIKESHEDDER,
    ASSERT,
    IFSTAR,
    HERE,
    END_OF_FILE,
    SYSCALLN,
};

class Token
{
public:
    int start;
    int end;
    TokenType type;
    std::string content;
    Token(int, int, TokenType, std::string);
    std::string toString();
};

class Lexer
{
   std::string input;
   size_t index;
   const std::string alphabet = "abcdefghjiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   const std::string digits = "0123456789";
public:
    Lexer(std::string);
    std::vector<Token> lex();
    char peek();
    char pop();
    char offset(size_t);
    char ahead();
    Token lexKeyword();
    Token lexHex();
    Token lexFloat();
    Token lexString();
    bool isComment();
    Token lexOperator();
    Token lexBikeshedder();
};

#endif //LEXER_H