#include "lexer.h"

Token::Token(int start, int end, TokenType type, std::string content) : start(start), end(end), 
    type(type), content(content) {;}

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

        if (alphabet.find(ch) != std::string::npos)
        {

        }

        else if (digits.find(ch) != std::string::npos)
        {

        }
    }

    return tokens;
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