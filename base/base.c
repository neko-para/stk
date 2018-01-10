#include "../stk.sub.h"
#include <stdio.h>
#include <stdlib.h>

void push(stkhdl p1, stkhdl _) {
	stk_push(stk_eval(p1));
}

void pop(stkhdl p1, stkhdl _) {
	if (p1) {
		*stk_at(stk_eval(p1)) = *stk_at(stk_size() - 1);
	}
	stk_pop();
}

void mov(stkhdl p1, stkhdl p2) {
	*stk_at(stk_eval(p1)) = stk_eval(p2);
}

void jmp(stkhdl p1, stkhdl _) {
	*stk_at(0) = stk_eval(p1);
}

void call(stkhdl p1, stkhdl _) {
	stkhdl p = stk_eval(p1);
	stk_push(*stk_at(0));
	*stk_at(0) = p;
}

void ret(stkhdl p1, stkhdl _) {
	stkhdl n = 0;
	stkhdl p = *stk_at(stk_size() - 1);
	if (p1) {
		n = stk_eval(p1);
	}
	stk_pop();
	*stk_at(0) = p;
	while (n--) {
		stk_pop();
	}
}

static long diff;

void cmp(stkhdl p1, stkhdl p2) {
	stkval a = stk_eval(p1);
	stkval b = stk_eval(p2);
	diff = ((a < b) ? -1 : (a > b));
}

void je(stkhdl p1, stkhdl _) {
	if (diff == 0) {
		jmp(p1, 0);
	}
}

void jne(stkhdl p1, stkhdl _) {
	if (diff) {
		jmp(p1, 0);
	}
}

void jg(stkhdl p1, stkhdl _) {
	if (diff > 0) {
		jmp(p1, 0);
	}
}

void jge(stkhdl p1, stkhdl _) {
	if (diff >= 0) {
		jmp(p1, 0);
	}
}

void jl(stkhdl p1, stkhdl _) {
	if (diff < 0) {
		jmp(p1, 0);
	}
}

void jle(stkhdl p1, stkhdl _) {
	if (diff <= 0) {
		jmp(p1, 0);
	}
}

void loop(stkhdl p1, stkhdl _) {
	if ((*stk_at(-1))--) {
		jmp(p1, 0);
	}
}

void Exit(stkhdl p1, stkhdl _) {
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
	_PROC_(loop),
	{
		"exit", Exit
	}
};

const proc_node* stk_init(unsigned* cnt, const char** name) {
	*cnt = sizeof(base_procs) / sizeof(base_procs[0]);
	*name = "base";
	return base_procs;
}
