all: parse.exe libstk.a base.so io.so

libstk.a: stk.sub.o
	ar rc libstk.a stk.sub.o

stk.sub.o: stk.sub.cpp stk.sub.h
	g++ -c -o $@ $<

parse.exe: yacc.o lex.o stk.o
	g++ -o $@ $^ -Wl,--rpath=.

lex.o: lex.c yacc.h stk.h
	gcc -c -o $@ $<

yacc.o: yacc.c yacc.h stk.h
	gcc -c -o $@ $<

stk.o: stk.cpp stk.h stk.sub.h
	g++ -c -o $@ $< -std=c++11

lex.c: parse.l
	flex -o $@ $<

yacc.c: parse.y
	bison -o $@ --defines=yacc.h $<

include */Makefile
