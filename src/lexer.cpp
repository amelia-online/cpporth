#include "lexer.h"
#include <format>
#include <algorithm>

Token::Token(int start, int end, TokenType type, std::string content) : start(start), end(end), type(type), content(content) {;}

Token::Token() : start(-1), end(-1), type(TokenType::VAR), content("") {;}

std::string Token::toString()
{
    if (type == TokenType::NEWLINE)
        return "NEWLINE";
    return content;
}

Lexer::Lexer(std::string input) : input(input), index(0) {;}

std::vector<Token> Lexer::lex()
{
    std::vector<Token> tokens;
    
    while (index < input.length())
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

        else if (operators.find(ch) != std::string::npos)
            tokens.push_back(lexOperator());

        else if (digits.find(ch) != std::string::npos)
        {
            Token hex;
            if (ch == '0')
            {
                if (lexHex(hex))
                    tokens.push_back(hex);
                else tokens.push_back(lexInt());
            }
            else
                tokens.push_back(lexInt());
        }

    }
    tokens.push_back(Token(index, index, TokenType::END_OF_FILE, "EOF"));
    return tokens;
}

Token Lexer::lexOperator()
{
    int idx = index;
    std::string acc;
    std::string allowed = "0123456789=<>" + operators;
    std::vector<std::string> recognized = {"<", ">", "=", "!=", ">=", "<=",
        "+", "-", "*", "--", "!8", "@8", "!16", "@16", "!32", "@32", "!64", "@64"};

    while (idx < input.length())
    {
        char c = input[idx];
        if (allowed.find(c) == std::string::npos)
            break;
        acc.push_back(c);
        idx++;
    }

    TokenType tt;

    if (std::find(recognized.begin(), recognized.end(), acc) == recognized.end())
    {
        std::cout << "Lex error: unrecognized operator: " << acc << std::endl;
        throw new std::exception();
    } 
    else if (acc == "--")
        tt = TokenType::BIKESHEDDER;
    else
        tt = TokenType::OP;

    Token t(index, idx, tt, acc);
    index = idx;
    return t;
}

Token Lexer::lexKeyword()
{
    int idx = index;
    std::string acc;
    std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-*()+.";

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
    else if (acc == "else")
        tt = TokenType::ELSE;
    else if (acc == "max")
        tt = TokenType::MAX;
    else if (acc == "min")
        tt = TokenType::MIN;
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

Token Lexer::lexInt()
{
    int idx = index;
    std::string acc;
    while (idx < input.length())
    {
        char c = input[idx];
        if (digits.find(c) == std::string::npos)
            break;
        idx++;
        acc.push_back(c);
    }
    Token t(index, idx, TokenType::INTVAL, acc);
    index = idx;
    return t;
}

bool Lexer::lexHex(Token& res)
{
    int idx = index;
    std::string acc;
    bool hasPrefix = false;
    bool hasX = false;
    int xCount = 0;
    std::string allowed = "0123456789abcdefABCDEFxX";
    while (idx < input.length())
    {
        char c = input[idx];

        if (allowed.find(c) == std::string::npos)
            break;

        if (c == 'x')
        {
            if (xCount >= 1) break;
            if (acc.length() == 1) hasPrefix = true;
            xCount++;
        }

        idx++;
        acc.push_back(c);
    }

    if (hasPrefix && acc.length() > 2)
    {
        Token t(index, idx, TokenType::INTVAL, acc);
        res = t;
        index = idx;
        return true;
    } else return false;
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