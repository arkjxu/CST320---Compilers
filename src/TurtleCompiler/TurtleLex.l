
%{

/* 
Name: Jiawei Xu
Date: January 09, 2020
File: TurtleLex.l
Assignment: Assignment 1
*/

#include <stdio.h>
#include <stdlib.h>

void yyerror(const char * msg);

void PrintKeyword(const char * lexeme);
void PrintIdentifier(const char * lexeme);
void PrintNumber(const char * lexeme);
void PrintIllegal(const char * lexeme);

%}

%option noyywrap

%%

\/\/[^\n]*					{  }
\/\*([^*]|\*+[^*\/])*\*\/	{  }

fd							{ PrintKeyword(yytext); }
bk							{ PrintKeyword(yytext); }
rt							{ PrintKeyword(yytext); }
lt							{ PrintKeyword(yytext); }
setc						{ PrintKeyword(yytext); }
sety						{ PrintKeyword(yytext); }
setxy						{ PrintKeyword(yytext); }
setx						{ PrintKeyword(yytext); }
home						{ PrintKeyword(yytext); }
seth						{ PrintKeyword(yytext); }
pd							{ PrintKeyword(yytext); }
pu							{ PrintKeyword(yytext); }
ht							{ PrintKeyword(yytext); }
st							{ PrintKeyword(yytext); }
repeat						{ PrintKeyword(yytext); }
if							{ PrintKeyword(yytext); }
ifelse						{ PrintKeyword(yytext); }
to							{ PrintKeyword(yytext); }
return						{ PrintKeyword(yytext); }


color						{ PrintKeyword(yytext); }
xcor						{ PrintKeyword(yytext); }
ycor						{ PrintKeyword(yytext); }
heading						{ PrintKeyword(yytext); }
random						{ PrintKeyword(yytext); }

black						{ PrintKeyword(yytext); }
white						{ PrintKeyword(yytext); }
orange						{ PrintKeyword(yytext); }
yellow						{ PrintKeyword(yytext); }
lime						{ PrintKeyword(yytext); }
cyan						{ PrintKeyword(yytext); }
blue						{ PrintKeyword(yytext); }
magenta						{ PrintKeyword(yytext); }
red							{ PrintKeyword(yytext); }
brown						{ PrintKeyword(yytext); }
green						{ PrintKeyword(yytext); }
turquoise					{ PrintKeyword(yytext); }
sky							{ PrintKeyword(yytext); }
violet						{ PrintKeyword(yytext); }
pink						{ PrintKeyword(yytext); }

int							{ PrintKeyword(yytext); }
[a-z][a-zA-Z0-9]*			{ PrintIdentifier(yytext); }

"+"							{ PrintKeyword(yytext); }
"-"							{ PrintKeyword(yytext); }
"*"							{ PrintKeyword(yytext); }
"/"							{ PrintKeyword(yytext); }

"="							{ PrintKeyword(yytext); }
"<"							{ PrintKeyword(yytext); }
">"							{ PrintKeyword(yytext); }


-?[0-9]+					{ PrintNumber(yytext); }

"["							{ PrintKeyword(yytext); }
"]"							{ PrintKeyword(yytext); }
"("							{ PrintKeyword(yytext); }
")"							{ PrintKeyword(yytext); }


(\t|" ")+					{ /* ignore whitespace */ }

\n							{ yylineno++; }

[0-9][a-zA-Z0-9]*			{ PrintIllegal(yytext); }
.							{ PrintIllegal(yytext); }


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

