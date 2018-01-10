#ifndef _STK_H_
#define _STK_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void init();
void push(long v);
void pop();
long size();
long* at(long x);
long eval(long x);
long where(long s);
const char* tostr(long s);

void pushFile(FILE* file);
long popFile(FILE** file);
long getWord(const char* str);
long parseWord(const char* str);
long getImm(long num);
long getAdd(long p1, long p2);
long getSub(long p1, long p2);
long getMul(long p1, long p2);
long getDiv(long p1, long p2);
long getMod(long p1, long p2);
long getNeg(long p1);
long getAnd(long p1, long p2);
long getOr(long p1, long p2);
long getXor(long p1, long p2);
long getShl(long p1, long p2);
long getShr(long p1, long p2);
long getNot(long p1);
long getDrf(long p1);
long getDlb(long p1);
const char* nextFile();

void Label(long l);
void Inst(long lb, long is);
void Inst1(long lb, long is, long p1);
void Inst2(long lb, long is, long p1, long p2);

void run();

#ifdef __cplusplus
}
#endif

#endif
