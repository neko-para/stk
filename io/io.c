#include "../stk.sub.h"
#include <stdio.h>

void out(int p1, int _) {
	printf("%d ", stk_eval(p1));
}

void in(int _, int __) {
	int d;
	scanf("%d", &d);
	stk_push(d);
}

static proc_node base_procs[] = {
	{
		"out", out
	}, {
		"in", in
	}
};

const proc_node* stk_init(unsigned* cnt, const char** name) {
	*cnt = 2;
	*name = "io";
	return base_procs;
}
