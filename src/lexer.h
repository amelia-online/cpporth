#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>

class Lexer
{
   std::string input;
   size_t index;
   const std::string alphabet = "abcdefghjiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   const std::string digits = "0123456789";
public:
    Lexer(std::string);
    std::vector<Token> *lex();
    char now();
};

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
};

class Token
{
public:
    int start;
    int end;
    TokenType type;
    std::string content;
};

#endif //LEXER_H