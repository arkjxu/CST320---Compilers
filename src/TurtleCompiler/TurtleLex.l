
%{

/* 
Name: Jiawei Xu
Date: January 09, 2020
File: TurtleLex.l
Assignment: Assignment 1
*/

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "TurtleYacc.tab.h"
#include "symtable.h"

typedef enum
{
	SCOPE_STATE_GLOBAL = 0,
	SCOPE_STATE_TO = 1,
	SCOPE_STATE_PARAM = 2,
	SCOPE_STATE_LOCAL
} SCOPE_STATE;
SCOPE_STATE scope_state = SCOPE_STATE_GLOBAL;
int bracket_count = 0;
STORAGE_TYPE storageForNextDecl = ST_GLOBAL;

void yyerror(const char * msg);

%}

%option noyywrap

%%

fd							{ return FD; }
bk							{ return BK; }
rt							{ return RT; }
lt							{ return LT; }
setc						{ return SETC; }
sety						{ return SETY; }
setxy						{ return SETXY; }
setx						{ return SETX; }
home						{ return HOME; }
seth						{ return SETH; }
pd							{ return PD; }
pu							{ return PU; }
ht							{ return HT; }
st							{ return ST; }
repeat						{ return REPEAT; }
if							{ return IF; }
ifelse						{ return IFELSE; }
to							{
								scope_state = SCOPE_STATE_TO;
								return TO;
							}

return						{ return RETURN; }


color						{ return COLOR; }
xcor						{ return XCOR; }
ycor						{ return YCOR; }
heading						{ return HEADING; }
random						{ return RANDOM; }

black						{ yylval.color_type = BLACK; return COLOR_NAME; }
white						{ yylval.color_type = WHITE; return COLOR_NAME; }
orange						{ yylval.color_type = ORANGE; return COLOR_NAME; }
yellow						{ yylval.color_type = YELLOW; return COLOR_NAME; }
lime						{ yylval.color_type = LIME; return COLOR_NAME; }
cyan						{ yylval.color_type = CYAN; return COLOR_NAME; }
blue						{ yylval.color_type = BLUE; return COLOR_NAME; }
magenta						{ yylval.color_type = MAGENTA; return COLOR_NAME; }
red							{ yylval.color_type = RED; return COLOR_NAME; }
brown						{ yylval.color_type = BROWN; return COLOR_NAME; }
green						{ yylval.color_type = GREEN; return COLOR_NAME; }
turquoise					{ yylval.color_type = TURQUOISE; return COLOR_NAME; }
sky							{ yylval.color_type = SKY; return COLOR_NAME; }
violet						{ yylval.color_type = VIOLET; return COLOR_NAME; }
pink						{ yylval.color_type = PINK; return COLOR_NAME; }

int							{ return INT; }

\-?[0-9]+					{ 
								yylval.value = atoi(yytext);
								return NUMBER; 
							}

"+"							{ return yytext[0]; }
"-"							{ return yytext[0]; }
"*"							{ return yytext[0]; }
"/"							{ return yytext[0]; }

"="							{ return yytext[0]; }
"<"							{ return yytext[0]; }
">"							{ return yytext[0]; }

"["							{
								bracket_count++;
								return yytext[0]; 
							}
"]"							{ 
								bracket_count--;
								if (scope_state == SCOPE_STATE_LOCAL && bracket_count == 0)
								{
									scope_state = SCOPE_STATE_GLOBAL;
									storageForNextDecl = ST_GLOBAL;
									SymbolTable::GetInstance()->ExitLocalScope();
								}
								return yytext[0]; 
							}
"("							{
								if (scope_state == SCOPE_STATE_TO) 
								{
									scope_state = SCOPE_STATE_PARAM;
									storageForNextDecl = ST_PARAM;
									SymbolTable::GetInstance()->NewLocalScope();
								}
								return yytext[0]; 
							}
")"							{
								if (scope_state == SCOPE_STATE_PARAM)
								{
									scope_state = SCOPE_STATE_LOCAL;
									storageForNextDecl = ST_LOCAL;
								}	
								return yytext[0]; 
							}

[a-zA-Z][a-zA-Z0-9]*		{ 
								yylval.symentry = SymbolTable::GetInstance()->Insert(yytext, ID);
								return ID; 
							}

\/\/.*\n					{ yylineno++; }
\/\*([^*]|(\*+[^*/]))*\*+\/	{
								for (int i = 0; yytext[i] != '\0'; ++i) {
									if (yytext[i] == '\n')
										yylineno++;
								}
							}

(\t|" ")+					{ /* ignore whitespace */ }

\n							{ yylineno++; }

[0-9][a-zA-Z0-9]*			{
								yyerror("illegal token");
								exit(-1);
							}

[^ \+\-\*\/\(\)\[\]\=\<\>\_\t\n]*	{
										yyerror("illegal token");
										exit(-1);
									}

\n							{ yylineno++; }

%%

void yyerror(const char * msg)
{
	fprintf(stderr, "ERROR! Line #%d: %s, near '%s'\n", yylineno, msg, yytext);
}

