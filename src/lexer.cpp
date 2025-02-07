#include "lexer.h"

Lexer::Lexer(std::string input) : input(input), index(0) {;}

std::vector<Token> *Lexer::lex()
{
    std::vector<Token> *tokens = new std::vector<Token>();



    return tokens;
}

char Lexer::now()
{
    return input[index];
}