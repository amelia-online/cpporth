#include "args.h"
#include <iostream>

void usage()
{
    std::cout << "usage:\n";
    std::cout << "cpporth run <file>";
    std::cout << "cpporth run <file> -- <args>\n";
}

Args::Args(int argc, char **argv)
{
    if (argc < 2)
    {
        usage();
        exit(1);
    }

    expect("run", argv[1]);
    filepath = argv[2];
    porthArgs.push_back(filepath);
    if (argc > 3)
    {
        expect("--", argv[3]);
        for (int i = 4; i < argc; i++)
            porthArgs.push_back(argv[i]);
    }

}

void Args::expect(std::string exp, std::string act)
{
    if (exp != act)
    {
        usage();
        exit(1);
    }
}