#include "../stk.sub.h"
#include <stdio.h>

void out(stkhdl p1, stkhdl _) {
	printf("%d", stk_eval(p1));
}

void in(stkhdl _, stkhdl __) {
	stkval d;
	scanf("%d", &d);
	stk_push(d);
}

void outs(stkhdl p1, stkhdl _) {
	printf("%s", stk_tostr(p1));
}

static proc_node base_procs[] = {
	{
		"out", out
	}, {
		"in", in
	}, {
		"outs", outs
	}
};

const proc_node* stk_init(unsigned* cnt, const char** name) {
	*cnt = 3;
	*name = "io";
	return base_procs;
}
