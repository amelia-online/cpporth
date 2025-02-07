CC = g++
FLAGS = -g -fsanitize=address -fsanitize=undefined -std=c++20

lexer.o: src/lexer.h src/lexer.cpp
	$(CC) $(FLAGS) -c src/lexer.cpp