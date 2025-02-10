#include "lexer.h"
#include <format>

Token::Token(int start, int end, TokenType type, std::string content) : start(start), end(end), 
    type(type), content(content) {;}

std::string Token::toString()
{
    return content;
}

Lexer::Lexer(std::string input) : input(input), index(0) {;}

std::vector<Token> Lexer::lex()
{
    std::vector<Token> tokens;

    while (index < input.size())
    {
        char ch = peek();

        if (ch == '\n')
        {
            if (tokens.size() == 0 || tokens.back().type != TokenType::NEWLINE)
            {
                Token t(index, index+1, TokenType::NEWLINE, "\n");
                tokens.push_back(t);
            }
            index++;
            continue;
        }

        if (ch == ' ')
        {
            index++;
            continue;
        }

        if (alphabet.find(ch) != std::string::npos)    
            tokens.push_back(lexKeyword());
    
        else if (digits.find(ch) != std::string::npos)
        {

        }

    }
    tokens.push_back(Token(index, index, TokenType::END_OF_FILE, "EOF"));
    return tokens;
}

Token Lexer::lexKeyword()
{
    int idx = index;
    std::string acc;
    std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-*()";

    while (idx < input.length())
    {
        char c = input[idx];
        if (allowed.find(c) == std::string::npos)
            break;

        acc.push_back(c);
        idx++;
    }

    TokenType tt;
    // Forgive me.
    if (acc == "proc")
        tt = TokenType::PROC;
    else if (acc == "in")
        tt = TokenType::IN;
    else if (acc == "while")
        tt = TokenType::WHILE;
    else if (acc == "end")
        tt = TokenType::END;
    else if (acc == "if")
        tt = TokenType::IF;
    else if (acc == "if*")
        tt = TokenType::IFSTAR;
    else if (acc == "PRINT")
        tt = TokenType::PRINT;
    else if (acc == "include")
        tt = TokenType::INCLUDE;
    else if (acc == "let")
        tt = TokenType::LET;
    else if (acc == "const")
        tt = TokenType::CONST;
    else if (acc == "memory")
        tt = TokenType::MEMORY;
    else if (acc == "offset")
        tt = TokenType::OFFSET;
    else if (acc == "reset")
        tt = TokenType::RESET;
    else if (acc == "int")
        tt = TokenType::INT;
    else if (acc == "bool")
        tt = TokenType::BOOL;
    else if (acc == "do")
        tt = TokenType::DO;
    else if (acc == "drop")
        tt = TokenType::DROP;
    else if (acc == "swap")
        tt = TokenType::SWAP;
    else if (acc == "over")
        tt = TokenType::OVER;
    else if (acc == "dup")
        tt = TokenType::DUP;
    else if (acc == "rot")
        tt = TokenType::ROT;
    else if (acc == "peek")
        tt = TokenType::PEEK;
    else if (acc == "inline")
        tt = TokenType::INLINE;
    else if (acc == "assert")
        tt = TokenType::ASSERT;
    else if (acc == "here")
        tt = TokenType::HERE;
    else if (acc == "ptr")
        tt = TokenType::PTR;
    else if (acc == "cast(int)" || acc == "cast(bool)" || acc == "cast(ptr)" 
        || acc == "and" || acc == "or" || acc == "not" || acc == "shr" || acc == "shl")
        tt = TokenType::OP;
    else if (acc == "syscall1" || acc == "syscall2" || acc == "syscall3" || acc == "syscall4" 
        || acc == "syscall5" || acc == "syscall6")
        tt = TokenType::SYSCALLN;
    else
        tt = TokenType::VAR;

    Token res(index, idx, tt, acc);
    index = idx;
    return res;
}

char Lexer::ahead()
{
    return offset(1);
}

char Lexer::offset(size_t offs)
{
    if (index + offs < input.length() && index + offs > -1)
        return input[index + offs];
    throw new std::exception();
}

char Lexer::pop()
{
    char res = peek();
    index++;
    return res;
}


char Lexer::peek()
{
    return input[index];
}