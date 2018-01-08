#include "../stk.sub.h"
#include <stdio.h>

void push(int p1, int _) {
	stk_push(stk_eval(p1));
}

void pop(int _, int __) {
	stk_pop();
}

void mov(int p1, int p2) {
	*stk_at(stk_eval(p1)) = stk_eval(p2);
}

void jmp(int p1, int _) {
	*stk_at(0) = stk_where(p1);
}

static proc_node base_procs[] = {
	{
		"push", push
	}, {
		"pop", pop
	}, {
		"mov", mov
	}, {
		"jmp", jmp
	}
};

const proc_node* stk_init(unsigned* cnt, const char** name) {
	*cnt = 4;
	*name = "base";
	return base_procs;
}
