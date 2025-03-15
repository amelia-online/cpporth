#include "helper.h"
#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <iostream>

std::string openFile(std::string path)
{
    std::string buf;
    std::ifstream file;

    file.open(path);

    if (!file.is_open())
    {
        std::cout << "Could not open file " << path << std::endl;
        throw new std::exception();
    }
  
    getline(file, buf, '\0');

    return buf;
}

std::string realString(std::string rep)
{
    std::string res;
    bool escape = false;
    for (char c : rep)
    {

        if (escape)
        {

            switch (c)
            {
                case 'n':
                    res.push_back('\n');
                    break;
                case '0':
                    res.push_back('\0');
                    break;
                case '\\':
                    res.push_back('\\');
                    break;
                case 'r':
                    res.push_back('\r');
                    break;
                case '"':
                    res.push_back('"');
                    break;
                case '\'':
                    res.push_back('\'');
                    break;
                default:
                    std::cout << "Error: unrecognized string escape \\" << c << std::endl;
                    throw new std::exception();
            }
            escape = false;
            continue;
        }

        if (c == '\\')
        {
            escape = true;
            continue;
        }

        if (c == '"')
            continue;

        res.push_back(c);

    }

    return res;
}

char realChar(std::string s)
{
    if (s == "'\\n'")
        return '\n';
    if (s == "'\\r'")
        return '\r';
    if (s == "'\\0'")
        return '\0';
    if (s == "'\\\\'")
        return '\\';
    if (s == "\\\"")
        return '\"';
    if (s == "'\\''")
        return '\'';
    return s[1];
}

std::vector<AST*> toASTs(std::string path)
{
    Lexer l(openFile(realString(path)));
    Parser p(l.lex());
    return p.parse();
}