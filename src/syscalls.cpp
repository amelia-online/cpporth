#include "syscalls.h"
#include <iostream>
#include <unistd.h>

long syscall(int syscallnum, std::vector<long> args) 
{
    switch (syscallnum)
    {
        case 1: // sys_write
        {
            unsigned int fd = (unsigned int)(args[0] & 0xFFFFFFFF);
            const char *buf = (const char *)(args[1]);
            size_t count = (size_t)args[2];
            auto res = write(fd, buf, count);
            return (long)res;
        }

        case 60: // exit
        {
            int code = (int)(args[0] & 0xFFFFFFFF);
            exit(code);
            break;
        } 

        default:
            std::cout << "Error: Syscall not implemented: " << syscallnum << std::endl;
            return 0;
    }
}