#include "lexer.h"
#include <format>
#include <algorithm>

Token::Token(int start, int end, int line, TokenType type, std::string content) : start(start), end(end), line(line), type(type), content(content) {;}

Token::Token() : start(-1), end(-1), line(-1), type(TokenType::VAR), content("") {;}

void Lexer::debug()
{
    if (index > 1 && index + 1 < input.length())
        std::cout << input[index-1] << input[index] << input[index+1] << std::endl;
}

std::string Token::toString()
{
    if (type == TokenType::NEWLINE)
        return "NEWLINE";
    return content;
}

Lexer::Lexer(std::string input) : input(input), line(1), index(0) {;}

std::vector<Token> Lexer::lex()
{
    std::vector<Token> tokens;
    
    while (index < input.length())
    {
        char ch = peek();

        //debug();
        //for (Token t : tokens)
        //    std::cout << t.toString() << std::endl;

        if (ch == '\n')
        {
            if (tokens.size() == 0 || tokens.back().type != TokenType::NEWLINE)
            {
                Token t(index, index+1, line, TokenType::NEWLINE, "\n");
                tokens.push_back(t);
            }
            line++;
            index++;
            continue;
        }

        if (ch == ' ')
        {
            index++;
            continue;
        }

        if (ch == '/')
            if (isComment())
                continue;

        if (operators.find(ch) != std::string::npos)
        {
            Token t;
            if (lexOperator(t))
                tokens.push_back(t);
            else if (ch == '-')
                tokens.push_back(lexInt());
            else tokens.push_back(lexKeyword());
        }
        
        else if (ch == '"')
            tokens.push_back(lexString());

        else if (ch == '\'') {
            tokens.push_back(lexChar());
        }

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

        else if (alphabet.find(ch) != std::string::npos)    
            tokens.push_back(lexKeyword());

    }
    tokens.push_back(Token(index, index, line, TokenType::END_OF_FILE, "EOF"));
    return tokens;
}

bool Lexer::isComment()
{
    if (peek() == '/' && input[index + 1] == '/')
    {
        index += 2;
        while (peek() != '\n' && index < input.length())
            index++;
        return true;
    } else return false;
}

Token Lexer::lexString()
{
    int idx = index;
    std::string acc;
    int qCount = 0;
    bool escape = false;
    while (idx < input.length())
    {
        char c = input[idx];

        if (escape)
        {
            acc.push_back(c);
            idx++;
            escape = false;
            continue;
        }

        if (c == '\\')
            escape = true;

        if (c == '\"')
        {
            qCount++;
            if (qCount >= 2)
            {
                acc.push_back(c);
                break;
            }
        }

        acc.push_back(c);
        idx++;
    }
    
    TokenType tt = TokenType::STRING;
    idx++;
    if (input[idx] == 'c')
    {
        acc.push_back('c');
        tt = TokenType::CSTRING;
        idx++;
    }

    Token t(index, idx, line, tt, acc);
    index = idx;
    return t;
}

Token Lexer::lexChar()
{
    int idx = index;
    int i = 0;
    std::string acc;
    int qCount = 0;
    bool escape = false;
    while (i < 4)
    {
        char c = input[idx];

        if (escape)
        {
            acc.push_back(c);
            i++;
            idx++;
            escape = false;
            continue;
        }

        if (c == '\\')
            escape = true;

        if (c == '\'')
        {
            ++qCount;
            if (qCount >= 2)
            {
                acc.push_back(c);
                i++;
                idx++;
                break;
            }
        }

        idx++;
        i++;
        acc.push_back(c);
    }
    Token t(index, idx, line, TokenType::CHAR, acc);
    index = idx;
    return t;
}

bool Lexer::lexOperator(Token& res)
{
    int idx = index;
    std::string acc;
    std::string allowed = "0123456789=<>/@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" + operators;
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
        //std::cout << "Lex error: unrecognized operator: " << acc << std::endl;
        return false;
    } 
    else if (acc == "--")
        tt = TokenType::BIKESHEDDER;
    else
        tt = TokenType::OP;

    Token t(index, idx, line, tt, acc);
    index = idx;
    res = t;
    return true;
}

Token Lexer::lexKeyword()
{
    int idx = index;
    std::string acc;
    std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-*().@!?+_/%1234567890=<>";

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
    else if (acc == "print")
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
    else if (acc == "addr-of")
        tt = TokenType::ADDROF;
    else if (acc == "addr")
        tt = TokenType::ADDR;
    else if (acc == "max")
        tt = TokenType::MAX;
    else if (acc == "cast(int)" || acc == "cast(bool)" || acc == "cast(ptr)" 
        || acc == "and" || acc == "or" || acc == "not" || acc == "shr" || acc == "shl" 
        || acc == "idivmod" || acc == "call-like" || acc == "divmod")
    {
        tt = TokenType::OP;
    }
    else if (acc == "syscall0" || acc == "syscall1" || acc == "syscall2" || acc == "syscall3" || acc == "syscall4" 
        || acc == "syscall5" || acc == "syscall6")
        tt = TokenType::SYSCALLN;
    else
        tt = TokenType::VAR;

    //std::cout << "Lexed " << acc << " as " << std::to_string((int)tt) << std::endl;

    Token res(index, idx, line, tt, acc);
    index = idx;
    return res;
}

Token Lexer::lexInt()
{
    int idx = index;
    std::string acc;
    std::string allowed = digits + alphabet;
    while (idx < input.length())
    {
        char c = input[idx];
        if (allowed.find(c) == std::string::npos)
            break;
        idx++;
        acc.push_back(c);
    }

    for (char c : acc)
        if (digits.find(c) == std::string::npos)
        {
            Token t(index, idx, line, TokenType::VAR, acc);
            index = idx;
            return t;
        }

    Token t(index, idx, line, TokenType::INTVAL, acc);
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
        Token t(index, idx, line, TokenType::INTVAL, acc);
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
    else throw new std::exception();
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