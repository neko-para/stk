#ifndef _STK_SUB_H_
#define _STK_SUB_H_

#define STK_TOOL_CNT 7

typedef void (*stkPushProc)(long v);
typedef void (*stkPopProc)();
typedef long (*stkSizeProc)();
typedef long* (*stkAtProc)(long x);
typedef long (*stkEvalProc)(long x);
typedef long (*stkWhereProc)(long s);
typedef const char* (*stkToStrProc)(long s);
typedef void (*stkProc)(long p1, long p2);

#ifdef __cplusplus
extern "C" {
#endif

extern void* stk_procs[STK_TOOL_CNT];

#define stk_push ((stkPushProc)stk_procs[0])
#define stk_pop ((stkPopProc)stk_procs[1])
#define stk_size ((stkSizeProc)stk_procs[2])
#define stk_at ((stkAtProc)stk_procs[3])
#define stk_eval ((stkEvalProc)stk_procs[4])
#define stk_where ((stkWhereProc)stk_procs[5])
#define stk_tostr ((stkToStrProc)stk_procs[6])

typedef struct {
	const char* name;
	stkProc proc;
} proc_node;

typedef const proc_node* (*stkInitProc)(void* procs[], unsigned* cnt, const char** name);
const proc_node* stk_init(unsigned* cnt, const char** name);

#ifdef __cplusplus
}
#endif

#endif
