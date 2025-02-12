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
    INTVAL, // 8
    INCLUDE,
    STRING,
    CSTRING,
    DROP,
    SWAP,
    DUP,
    ROT,
    OVER, // 16
    OP,
    MAX,
    MIN,
    MEMORY,
    ELSE,
    DO,
    INT,
    INLINE, // 24
    PTR,
    CONST,
    LET,
    PEEK,
    OFFSET,
    RESET,
    BOOL,
    ADDR, // 32
    BIKESHEDDER,
    ASSERT,
    IFSTAR,
    HERE,
    END_OF_FILE,
    SYSCALLN,
    CHAR,
    TRUE, // 40
    FALSE,
    ADDROF
};

class Token
{
public:
    int start;
    int end;
    TokenType type;
    std::string content;
    Token(int, int, TokenType, std::string);
    Token();
    std::string toString();
};

class Lexer
{
   std::string input;
   size_t index;
   const std::string alphabet = "abcdefghjiklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/%";
   const std::string digits = "0123456789";
   const std::string operators = "+-*!@=?><_";
public:
    Lexer(std::string);
    std::vector<Token> lex();
    char peek();
    char pop();
    char offset(size_t);
    char ahead();
    Token lexKeyword();
    bool lexHex(Token&);
    Token lexInt();
    Token lexString();
    Token lexChar();
    bool isComment();
    bool lexOperator(Token&);
    void debug();
};

#endif //LEXER_H