#include "../stk.sub.h"
#include <stdio.h>

void out(long p1, long _) {
	printf("%d", stk_eval(p1));
}

void in(long _, long __) {
	long d;
	scanf("%d", &d);
	stk_push(d);
}

void outs(long p1, long _) {
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
