CC = g++
FLAGS = -g -fsanitize=address -fsanitize=undefined -std=c++20
TEST=src/test.txt
OBJS=lexer.o main.o parser.o ast.o runtime.o helper.o syscalls.o

all: cpporth

run: cpporth
	./cpporth $(TEST)

clean:
	rm *.o ./cpporth

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