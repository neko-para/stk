#include "stk.sub.h"

void* stk_procs[STK_TOOL_CNT];

extern "C" const proc_node* __stk_init(void* procs[], unsigned* cnt, const char** name) {
	for (long i = 0; i < STK_TOOL_CNT; ++i) {
		stk_procs[i] = procs[i];
	}
	return stk_init(cnt, name);
}
