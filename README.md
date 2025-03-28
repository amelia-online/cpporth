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
## Unsupported Features

* `call-like`: because I have no idea what it does, and there is little to no documentation on it.
---
## Porth++ Language Extension
On top of the Porth language I am adding new features that are optional. I am calling this set of features **Porth++**

* `alloc` and `free`
  - these two features allow you to dynamically allocate memory on the heap that will live until freed.
  - ex.
  ```ruby
  include "porth/std/std.porth"

  proc make-array int -- ptr in
      let size in
          size sizeof(u64) * alloc
      end
  end

  proc main in
      12 make-array
      let pointer in
          pointer free
      end
  end
  ```
  - Notice how the pointer created in `make-array` is not automatically freed once `make-array` returns.

 * `type` and `match`
   - `type` allows you to define a type and any of its forms.
   - you create an instance of that type using `new`: `new <ParentType>::<VariantName>[<Values>, ...]`.
   - this puts a pointer to the instance on the stack.
   - `match` works with pointers to variants. It uses the variant's type to execute a certain block of code with that instance's data.
   - `type` and `match` are heavily inspired by the same constructs in [Matthew Flatt's Shplait Language](https://github.com/mflatt/shplait).
   - ex.
   ```ruby
    include "porth/std/std.porth"

    type T
    | a[n :: int, n2 :: ptr]
    | b[n :: bool]
    end

    proc main in
        new T::b[false]
        match T
        | a[b, c]:
            b print
        | b[n]:
            n true = if
                "true\n" puts
            else
                "false\n" puts
            end
        end
    end
   ```

   Output: `false`

* If your type has many different variants but you don't want to match over all of them, you can use `else`:
```ruby
include "porth/std/std.porth"

type T
| a[n :: int]
| b[bln :: bool]
| c[p :: ptr]
end

proc main in
    new T::b[true]
    match T
    | a[n]:
        n print
    | else:
        "else branch\n" puts
    end
end
```

Output: `else branch`