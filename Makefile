FLAGS=-g

all: parse libstk.a base.so io.so

libstk.a: stk.sub.o
	ar rc libstk.a stk.sub.o

stk.sub.o: stk.sub.cpp stk.sub.h
	g++ -c -o $@ $< -fPIC $(FLAGS)

parse: yacc.o lex.o stk.o
	g++ -o $@ $^ -Wl,--rpath=. -ldl $(FLAGS)

lex.o: lex.c yacc.h stk.h
	gcc -c -o $@ $< $(FLAGS)

yacc.o: yacc.c yacc.h stk.h
	gcc -c -o $@ $< $(FLAGS)

stk.o: stk.cpp stk.h stk.sub.h
	g++ -c -o $@ $< -std=c++11 $(FLAGS)

lex.c: parse.l
	flex -o $@ $<

yacc.c: parse.y
	bison -o $@ --defines=yacc.h $<

include */Makefile
