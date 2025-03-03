CPPORTH
===

"It's like Forth but in Python" *but in C++ \(and it's interpreted!\)*

This is an interpreter for [Tsoding's Porth language.](https://gitlab.com/tsoding/porth) The original Porth language only works on Linux as it relies heavily on Linux syscalls. This interpreter is intended to work on any operating system with a C++ compiler; I recommend g++. C++20 is required to compile this project.
To compile, `cd` into the top-level directory and use `make`.

---
This project is a work-in-progress and is not complete. There may be some slight differences between the original language and this interpreted version, for example,
in error messages and Porth's `here`. Overall, however, the language should be mostly the same.

---
Testing
==

This project uses googletest for testing, which is included as a submodule to this project.
To run the tests, you must first build libgtest.a. To do this:

```bash
$ cd googletest

$ mkdir build

$ cd build

$ cmake ..

$ make
```

This will create a Makefile in the build directory of googletest and build the googletest.
Once this is done, do the following:

```bash
$ cd ../..

$ make tests
```

This will bring us back to the main project directory and build and run the tests.
---
## Unsuppported Features

* `call-like`: because I have no idea what it does, and there is little to no documentation on it.

