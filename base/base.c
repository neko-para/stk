#include "../stk.sub.h"
#include <stdio.h>
#include <stdlib.h>

void push(int p1, int _) {
	stk_push(stk_eval(p1));
}

void pop(int p1, int _) {
	if (p1) {
		*stk_at(stk_eval(p1)) = *stk_at(stk_size() - 1);
	}
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

void ret(int p1, int _) {
	int n = 0;
	int p = *stk_at(stk_size() - 1);
	if (p1) {
		n = stk_eval(p1);
	}
	stk_pop();
	*stk_at(0) = p;
	while (n--) {
		stk_pop();
	}
}

static int diff;

void cmp(int p1, int p2) {
	diff = stk_eval(p1) - stk_eval(p2);
}

void je(int p1, int _) {
	if (diff == 0) {
		jmp(p1, 0);
	}
}

void jne(int p1, int _) {
	if (diff) {
		jmp(p1, 0);
	}
}

void jg(int p1, int _) {
	if (diff > 0) {
		jmp(p1, 0);
	}
}

void jge(int p1, int _) {
	if (diff >= 0) {
		jmp(p1, 0);
	}
}

void jl(int p1, int _) {
	if (diff < 0) {
		jmp(p1, 0);
	}
}

void jle(int p1, int _) {
	if (diff <= 0) {
		jmp(p1, 0);
	}
}

void Exit(int p1, int _) {
	exit(stk_eval(p1));
}

#define _PROC_(p) { \
	#p, p \
	}

static proc_node base_procs[] = {
	_PROC_(push),
	_PROC_(pop),
	_PROC_(mov),
	_PROC_(jmp),
	_PROC_(call),
	_PROC_(ret),
	_PROC_(cmp),
	_PROC_(je),
	_PROC_(jne),
	_PROC_(jg),
	_PROC_(jge),
	_PROC_(jl),
	_PROC_(jle),
	{
		"exit", Exit
	}
};

const proc_node* stk_init(unsigned* cnt, const char** name) {
	*cnt = sizeof(base_procs) / sizeof(base_procs[0]);
	*name = "base";
	return base_procs;
}
