#include <iostream>
#include <fstream>
#include "lexer.h"
#include "helper.h"
#include "parser.h"
#include "runtime.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please include file path." << std::endl;
        exit(0);
    }
    

    auto txt = openFile(argv[1]);

    Lexer lexer(txt);
    std::vector<Token> tokens = lexer.lex();

        //for (auto t : tokens)
        //    std::cout << t.toString() << std::endl;

    Parser parser(tokens);
    std::vector<AST*> asts = parser.parse();
        //for (AST *ast : asts)
        //    std::cout << ast->toString() << std::endl;
    auto args = new char*[argc];
    for (int i = 0; i < argc; i++) {
        args[i] = new char[std::strlen(argv[i])+1];
        std::strncpy(args[i], argv[i], std::strlen(argv[i])+1);
    }
    
    Stack s;
    Env e(argc, args);
    interp(asts, s, e);

    for (int i = 0; i < argc; i++)
        delete[] args[i];
    delete[] args;

    parser.cleanup(asts);


}