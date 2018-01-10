%{
#include "stk.h"
#include "yacc.h"
#include <stdlib.h>
%}
%%
[a-zA-Z_][a-zA-Z0-9_]* {
	yylval = getWord(yytext);
	return WORD;
}
\"([^\S\\"]|[ \t]|\\([\n"\\abefnrtv]|[0-7]{1,3}|x[0-9a-fA-F]{1,2}))*\" {
	yylval = parseWord(yytext);
	return WORD;
}
[0-9]+ {
	yylval = getImm(atoi(yytext));
	return VALUE;	
}
[-+*/%\[\]\(\)\{\}:;,.#] {
	return *yytext;
}
[ \t\n\r]* ;
%%

int yyerror(char* error) {
	fprintf(stderr, "%s at line %d", error, yylineno);
}
