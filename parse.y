%{
#include "stk.h"
#include <stdio.h>
#define YYDEBUG 0
int yylex();
int yyerror(char* err);
extern FILE* yyin;
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
	| WORD { $$ = $1; }
	| value '+' value { $$ = getAdd($1, $3); }
	| value '-' value { $$ = getSub($1, $3); }
	| value '*' value { $$ = getMul($1, $3); }
	| value '/' value { $$ = getDiv($1, $3); }
	| value '%' value { $$ = getMod($1, $3); }
	| '-' value %prec NEG { $$ = getNeg($2); }
	| '[' value ']' { $$ = getDrf($2); }
	| '{' value '}' { $$ = getDlb($2); }
	| '(' value ')' { $$ = $2; }
	;

cmd: ';'
	| WORD ':' { Label($1); }
	| WORD ';' { Inst(0, $1); }
	| WORD '.' WORD ';' { Inst($1, $3); }
	| WORD value ';' { Inst1(0, $1, $2); }
	| WORD '.' WORD value ';' { Inst1($1, $3, $4); }
	| WORD value ',' value { Inst2(0, $1, $2, $4); }
	| WORD '.' WORD value ',' value { Inst2($1, $3, $4, $6); }
	| '#' WORD ';' {
		pushFile(fopen(tostr($2), "r"));
	}
	;

%%
int yywrap() {
	return popFile(&yyin);
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
