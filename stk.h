#ifndef _STK_H_
#define _STK_H_

#include <stdio.h>

#include "stk.sub.h"

#ifdef __cplusplus
extern "C" {
#endif

void init();
void push(stkval v);
void pop();
stkval size();
stkval* at(stkval x);
stkval eval(stkhdl x);
stkval where(stkhdl s);
const char* tostr(stkhdl s);

void pushFile(FILE* file);
int popFile(FILE** file);
stkhdl getWord(const char* str);
stkhdl parseWord(const char* str);
stkhdl getImm(stkval num);
stkhdl getAdd(stkhdl p1, stkhdl p2);
stkhdl getSub(stkhdl p1, stkhdl p2);
stkhdl getMul(stkhdl p1, stkhdl p2);
stkhdl getDiv(stkhdl p1, stkhdl p2);
stkhdl getMod(stkhdl p1, stkhdl p2);
stkhdl getNeg(stkhdl p1);
stkhdl getAnd(stkhdl p1, stkhdl p2);
stkhdl getOr(stkhdl p1, stkhdl p2);
stkhdl getXor(stkhdl p1, stkhdl p2);
stkhdl getShl(stkhdl p1, stkhdl p2);
stkhdl getShr(stkhdl p1, stkhdl p2);
stkhdl getNot(stkhdl p1);
stkhdl getDrf(stkhdl p1);
stkhdl getDlb(stkhdl p1);
const char* nextFile();

void Label(stkhdl l);
void Inst(stkhdl lb, stkhdl is);
void Inst1(stkhdl lb, stkhdl is, stkhdl p1);
void Inst2(stkhdl lb, stkhdl is, stkhdl p1, stkhdl p2);

void run();

#ifdef __cplusplus
}
#endif

#endif
