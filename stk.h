#ifndef _STK_H_
#define _STK_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void init();
void push(int v);
void pop();
int size();
int* at(int x);
int eval(int x);
int where(int s);
const char* tostr(int s);

void pushFile(FILE* file);
int popFile(FILE** file);
int getWord(const char* str);
int parseWord(const char* str);
int getImm(int num);
int getAdd(int p1, int p2);
int getSub(int p1, int p2);
int getMul(int p1, int p2);
int getDiv(int p1, int p2);
int getMod(int p1, int p2);
int getNeg(int p1);
int getDrf(int p1);
int getDlb(int p1);
const char* nextFile();

void Label(int l);
void Inst(int lb, int is);
void Inst1(int lb, int is, int p1);
void Inst2(int lb, int is, int p1, int p2);

void run();

#ifdef __cplusplus
}
#endif

#endif
