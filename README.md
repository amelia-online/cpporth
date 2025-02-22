CPPORTH
===

"It's like Forth but in Python" *but in C++ \(and it's interpreted!\)*

This is an interpreter for [Tsoding's Porth language.](https://gitlab.com/tsoding/porth) The original Porth language uses [fasm](https://flatassembler.net/) to compile, which as far as
I know, only supports Linux. This interpreter is intended to work on any operating system with a C++ compiler; I recommend g++. C++20 is required to compile this project.
To compile, `cd` into the top-level directory and use `make`.

---
This project is a work-in-progress and is not complete. There may be some slight differences between the original language and this interpreted version, for example,
in error messages and Porth's `here`. Overall, however, the language should be mostly the same.



