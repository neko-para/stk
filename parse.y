%{
#include "stk.h"
#include <stdio.h>
#define YYDEBUG 0
%}
%token WORD VALUE
%left '+' '-'
%left '*' '/' '%'
%left NEG
%%
cmds:
	| cmds cmd
	;

value:
	VALUE { $$ = $1; }
	| value '+' value { $$ = getAdd($1, $3); }
	| value '-' value { $$ = getSub($1, $3); }
	| value '*' value { $$ = getMul($1, $3); }
	| value '/' value { $$ = getDiv($1, $3); }
	| value '%' value { $$ = getMod($1, $3); }
	| '-' value %prec NEG { $$ = getNeg($2); }
	| '[' value ']' { $$ = getDrf($2); }
	| '{' WORD '}' { $$ = getDlb($2); }
	| '(' value ')' { $$ = $2; }
	;

cmd: ';'
	| WORD ':' { Label($1); }
	| WORD ';' { Inst($1); }
	| WORD WORD ';' { Inst1($1, $2); }
	| WORD value ';' { Inst1($1, $2); }
	| WORD value ',' value { Inst2($1, $2, $4); }
	;

%%
int yywrap() {
	return 1;
}

#if YYDEBUG
extern int yydebug;
#endif

int main(int argc, char* argv[]) {
	freopen(argv[1], "r", stdin);
	init();
#if YYDEBUG
	yydebug = 1;
#endif
	yyparse();
	run();
}
