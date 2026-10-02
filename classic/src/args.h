#ifndef CPPORTH_ARGS_H
#define CPPORTH_ARGS_H

// usage:
// ./cpporth run <file> -- <porth args>
#include <string>
#include <vector>

class Args
{
public:
    std::string filepath;
    std::vector<std::string> porthArgs;
    Args(int, char**);
    void expect(std::string, std::string);
};

void usage();

#endif // CPPORTH_ARGS_H