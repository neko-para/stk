#ifndef _STK_H_
#define _STK_H_

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

int getWord(const char* str);
int getImm(int num);
int getAdd(int p1, int p2);
int getSub(int p1, int p2);
int getMul(int p1, int p2);
int getDiv(int p1, int p2);
int getMod(int p1, int p2);
int getNeg(int p1);
int getDrf(int p1);

void Label(int l);
void Inst(int is);
void Inst1(int is, int p1);
void Inst2(int is, int p1, int p2);

void run();

#ifdef __cplusplus
}
#endif

#endif