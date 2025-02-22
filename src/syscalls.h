#ifndef CPPORTH_SYSCALLS_H
#define CPPORTH_SYSCALLS_H

#include <vector>

long syscall(int, std::vector<long>);

#endif // CPPORTH_SYSCALLS_H