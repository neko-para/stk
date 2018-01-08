%{
#include "stk.h"
#include "yacc.h"
#include <stdlib.h>
%}
%%
[a-zA-Z_][a-zA-Z0-9_.]* {
	yylval = getWord(yytext);
	return WORD;
}
[0-9]+ {
	yylval = getImm(atoi(yytext));
	return VALUE;	
}
[-+*/%\[\]\(\):;,] {
	return *yytext;
}
[ \t\n\r]* ;
%%

