%{

/* Turtle Compiler yacc file
   by Pete Myers
   OIT Portland Fall 2008
   Bison C++ version Jan 2017

   Assignment 2 handout
*/

#include <stdio.h>
#include <stdlib.h>

extern FILE * yyin;
extern FILE * yyout;
extern int yylineno;

void yyerror(const char *);

int yylex(void);

%}

%token HOME
%token FD
%token RT
%token SETXY
%token BK
%token LT
%token SETC
%token SETY
%token SETX
%token SETH
%token PD
%token PU
%token HT
%token ST
%token REPEAT
%token IF
%token IFELSE
%token TO
%token RETURN

%token COLOR
%token XCOR
%token YCOR
%token HEADING
%token RANDOM

%token NUMBER
%token COLOR_NAME

%token INT
%token ID

%left '+' '-'
%left '*' '/'

%%

statements:	statement statements		{ printf("statements->statement statements\n"); }
	|									{ printf("statements-><empty>\n"); }
	;

statement:	HOME						{ printf("statement->HOME\n"); }
	|	FD expression					{ printf("statement->FD expression\n"); }
	|	BK expression					{ printf("statement->BK expression\n"); }
	|	RT expression					{ printf("statement->RT expression\n"); }
	|	SETH expression					{ printf("statement->SETH expression\n"); }
	|	LT expression					{ printf("statement->LT expression\n"); }
	|	SETC expression					{ printf("statement->SETC expression\n"); }
	|	PD								{ printf("statement->PD\n"); }
	|	PU								{ printf("statement->PU\n"); }
	|	ST								{ printf("statement->ST\n"); }
	|	HT								{ printf("statement->HT\n"); }
	|	SETXY expression expression		{ printf("statement->SETXY expression expression\n"); }
	|	SETY expression					{ printf("statement->SETY expression\n"); }
	|	SETX expression					{ printf("statement->SETX expression\n"); }
	|	IF '('	condition	')'	'['	statements ']'	{ printf("statement->IF ( condition ) [ statements ]\n"); }
	|	IFELSE '(' condition ')' '[' statements ']' '[' statements ']' { printf("statement->IFELSE ( condition ) [ statements ] [ statements ]\n"); }
	|	REPEAT expression '[' statements ']'	{ printf("statement->REPEAT expression [ statements ]\n"); }
	|	vardecl							{ printf("statement->vardecl\n"); }
	|	ID '=' expression				{ printf("statement->ID = expression\n"); }
	|	procsig '[' statements ']'		{ printf("statement->procsig [ statements ]\n") }
	|	ID '(' args ')'					{ printf("statement->ID ( args )\n"); }
	|	RETURN							{ printf("statement->RETURN\n"); }
	|	RETURN expression				{ printf("statement->RETURN expression\n"); }
	;

vardecl:	INT ID						{ printf("vardecl->INT ID\n"); }
	;

procsig:	TO ID '(' params ')'		{ printf("procsig->TO ID ( params )\n"); }
	;

params:	vardecl params					{ printf("params->vardecl params\n"); }
	|									{ printf("params-><empty>\n"); }
	;

args:	expression args					{ printf("args->expression args\n"); }
	|									{ printf("args-><empty>\n"); }
	;

expression:	expression '+' expression	{ printf("expression->expression + expression\n"); }
	|	expression '-' expression		{ printf("expression->expression - expression\n"); }
	|	expression '*' expression		{ printf("expression->expression * expression\n"); }
	|	expression '/' expression		{ printf("expression->expression / expression\n"); }
	|	'(' expression ')'				{ printf("expression->( expression )\n"); }
	|	NUMBER							{ printf("expression->NUMBER\n"); }
	|	COLOR_NAME						{ printf("expression->COLOR_NAME\n"); }
	|	function						{ printf("expression->function\n"); }
	|	ID								{ printf("expression->ID\n"); }
	|	ID '(' args ')'					{ printf("expression->ID ( args )\n"); }
	;

function:	COLOR						{ printf("function->COLOR\n"); }
	|	XCOR							{ printf("function->XCOR\n"); }
	|	YCOR							{ printf("function->YCOR\n"); }
	|	HEADING							{ printf("function->HEADING\n"); }
	|	RANDOM '(' expression ')'		{ printf("function->RANDOM ( expression )\n"); }
	;

condition: expression '=' expression	{ printf("condition->expression = expression\n"); }
	|	expression '<' expression		{ printf("condition->expression < expression\n"); }
	|	expression '>' expression		{ printf("condition->expression > expression\n"); }
	;
%%


