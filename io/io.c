#include "../stk.sub.h"
#include <stdio.h>

void out(int p1, int _) {
	printf("%d", stk_eval(p1));
}

void in(int _, int __) {
	int d;
	scanf("%d", &d);
	stk_push(d);
}

void outs(int p1, int _) {
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
