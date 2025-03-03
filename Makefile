CC = g++
FLAGS = -g -fsanitize=address -std=c++20
TEST=src/test.txt
OBJS=lexer.o main.o parser.o ast.o runtime.o helper.o syscalls.o
OBJSNOMAIN= lexer.o parser.o ast.o runtime.o helper.o syscalls.o
GTEST=./googletest

all: cpporth

run: cpporth
	./cpporth $(TEST)

clean:
	rm *.o ./cpporth ./cpporthtests

tests: cpporthtests

cpporthtests: tests/tests.cpp $(OBJSNOMAIN)
	$(CC) $(FLAGS) -I$(GTEST)/googletest/include -L$(GTEST)/build/lib -lgtest $(OBJSNOMAIN) tests/tests.cpp -o cpporthtests
	./cpporthtests

cpporth: main.o lexer.o parser.o ast.o runtime.o helper.o syscalls.o
	$(CC) $(FLAGS) $(OBJS) -o cpporth

syscalls.o: src/syscalls.h src/syscalls.cpp
	$(CC) $(FLAGS) -c src/syscalls.cpp

helper.o: src/helper.cpp src/helper.h
	$(CC) $(FLAGS) -c src/helper.cpp

runtime.o: src/runtime.cpp src/runtime.h
	$(CC) $(FLAGS) -c src/runtime.cpp

parser.o: src/parser.cpp src/parser.h
	$(CC) $(FLAGS) -c src/parser.cpp

ast.o: src/ast.h src/ast.cpp
	$(CC) $(FLAGS) -c src/ast.cpp

main.o: src/main.cpp
	$(CC) $(FLAGS) -c src/main.cpp

lexer.o: src/lexer.h src/lexer.cpp
	$(CC) $(FLAGS) -c src/lexer.cpp

typechecker.o: src/typechecker.cpp src/typechecker.h
	$(CC) $(FLAGS) -c src/typechecker.cpp