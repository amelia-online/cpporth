#ifndef CPPORTH_HELPER_H
#define CPPORTH_HELPER_H

#include <string>
#include "runtime.h"

std::string openFile(std::string);
std::string realString(std::string);
void dumpStack(const Stack&);

#endif // CPPORTH_HELPER_H