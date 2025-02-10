CC = g++
FLAGS = -g -fsanitize=address -fsanitize=undefined -std=c++20
TEST=src/test.txt

all: cpporth

run: cpporth
	./cpporth $(TEST)

clean:
	rm *.o ./cpporth

cpporth: main.o lexer.o
	$(CC) $(FLAGS) main.o lexer.o -o cpporth

main.o: src/main.cpp
	$(CC) $(FLAGS) -c src/main.cpp

lexer.o: src/lexer.h src/lexer.cpp
	$(CC) $(FLAGS) -c src/lexer.cpp