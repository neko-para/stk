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
	*stk_at(0) = stk_eval(p1);
}

void call(int p1, int _) {
	int p = stk_eval(p1);
	stk_push(*stk_at(0));
	*stk_at(0) = p;
}

void ret(int _, int __) {
	int p = *stk_at(stk_size() - 1);
	stk_pop();
	*stk_at(0) = p;
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
	}, {
		"call", call
	}, {
		"ret", ret
	}
};

const proc_node* stk_init(unsigned* cnt, const char** name) {
	*cnt = sizeof(base_procs) / sizeof(base_procs[0]);
	*name = "base";
	return base_procs;
}
