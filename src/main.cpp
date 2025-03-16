#include <iostream>
#include <fstream>
#include "lexer.h"
#include "helper.h"
#include "parser.h"
#include "runtime.h"
#include "args.h"
//#include "typechecker.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please include file path." << std::endl;
        exit(0);
    }
    
    Args args(argc, argv);
    auto txt = openFile(args.filepath);

    Lexer lexer(txt);
    std::vector<Token> tokens = lexer.lex();

        //for (auto t : tokens)
        //    std::cout << t.toString() << std::endl;

    Parser parser(tokens);
    std::vector<AST*> asts = parser.parse();
        //for (AST *ast : asts)
        //    std::cout << ast->toString() << std::endl;
    auto pargs = new char*[args.porthArgs.size()];
    for (int i = 0; i < args.porthArgs.size(); i++) {
        pargs[i] = new char[args.porthArgs[i].size()+1];
        std::strncpy(pargs[i], args.porthArgs[i].c_str(), args.porthArgs[i].size()+1);
    }

    // TypeEnv tenv;
    // TypeStack tstack;
    // typecheck(asts, tstack, tenv);
    
    Stack s;
    Env e(args.porthArgs.size(), pargs);
    interp(asts, s, e);

    for (int i = 0; i < args.porthArgs.size(); i++)
        delete[] pargs[i];
    delete[] pargs;

    parser.cleanup(asts);


}