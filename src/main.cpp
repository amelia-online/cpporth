#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "runtime.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please include file path." << std::endl;
        exit(0);
    }

    std::ifstream file;

    file.open(argv[1]);

    if (file.is_open())
    {
        std::string content;
        getline(file, content, '\0');
        Lexer lexer(content);
        std::vector<Token> tokens = lexer.lex();
        Parser parser(tokens);
        std::vector<AST*> asts = parser.parse();
        for (AST *ast : asts)
            std::cout << ast->toString() << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "Output:" << std::endl;
        Stack stack;
        Env env;
        demo(asts, stack, env);
        parser.cleanup(asts);
    } else std::cout << "Could not open file" << std::endl;

}