#ifndef CPPORTH_HELPER_H
#define CPPORTH_HELPER_H

#include <string>
#include "runtime.h"

std::string openFile(std::string);
std::string realString(std::string);
char realChar(std::string);
void dumpStack(const Stack&);
std::vector<AST*> toASTs(std::string);

#endif // CPPORTH_HELPER_H