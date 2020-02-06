
%{

/* 
Name: Jiawei Xu
Date: January 09, 2020
File: TurtleLex.l
Assignment: Assignment 1
*/

#include <stdio.h>
#include <stdlib.h>
#include "TurtleYacc.tab.h"

void yyerror(const char * msg);

void PrintKeyword(const char * lexeme);
void PrintIdentifier(const char * lexeme);
void PrintNumber(const char * lexeme);
void PrintIllegal(const char * lexeme);

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
to							{ return TO; }
return						{ return RETURN; }


color						{ return COLOR; }
xcor						{ return XCOR; }
ycor						{ return YCOR; }
heading						{ return HEADING; }
random						{ return RANDOM; }

black						{ return COLOR_NAME; }
white						{ return COLOR_NAME; }
orange						{ return COLOR_NAME; }
yellow						{ return COLOR_NAME; }
lime						{ return COLOR_NAME; }
cyan						{ return COLOR_NAME; }
blue						{ return COLOR_NAME; }
magenta						{ return COLOR_NAME; }
red							{ return COLOR_NAME; }
brown						{ return COLOR_NAME; }
green						{ return COLOR_NAME; }
turquoise					{ return COLOR_NAME; }
sky							{ return COLOR_NAME; }
violet						{ return COLOR_NAME; }
pink						{ return COLOR_NAME; }

int							{ return INT; }

\-?[0-9]+					{ return NUMBER; }

"+"							{ return yytext[0]; }
"-"							{ return yytext[0]; }
"*"							{ return yytext[0]; }
"/"							{ return yytext[0]; }

"="							{ return yytext[0]; }
"<"							{ return yytext[0]; }
">"							{ return yytext[0]; }

"["							{ return yytext[0]; }
"]"							{ return yytext[0]; }
"("							{ return yytext[0]; }
")"							{ return yytext[0]; }

[a-zA-Z][a-zA-Z0-9]*		{ return ID; }

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

void PrintKeyword(const char * lexeme)
{
	printf("Keyword: %s\n", lexeme);
}

void PrintIdentifier(const char * lexeme)
{
	printf("Id: %s\n", lexeme);
}

void PrintNumber(const char * lexeme)
{
	printf("Number: %s\n", lexeme);
}

void PrintIllegal(const char * lexeme)
{
	printf("Illegal: %s\n", lexeme);
}

void yyerror(const char * msg)
{
	fprintf(stderr, "ERROR! Line #%d: %s, near '%s'\n", yylineno, msg, yytext);
}

