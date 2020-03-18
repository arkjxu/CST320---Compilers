%{

/* Turtle Compiler yacc file
   by Pete Myers
   OIT Portland Fall 2008
   Bison C++ version Jan 2017
*/

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "symtable.h"

typedef enum {
	SE000 = 0, SE001, SE002, SE003, SE004,
	SE101, SE102, SE103, SE104, SE105, SE106, SE107, SE108, SE109, SE110, SE111,
	SE_COUNT
} SEMANTIC_ERROR;

char * SEMANTIC_ERROR_STRINGS[SE_COUNT][2] =
{
	{ "SE000", "Using a variable name prior to declaration (global, local or parameter)" },
	{ "SE001", "Calling a procedure prior to declaration" },
	{ "SE002", "Using a variable name as if it were a procedure name (e.g. calling a variable)" },
	{ "SE003", "Using a procedure name as if it were a variable name (e.g. assigning a value to a procedure)" },
	{ "SE004", "Calling a procedure with the wrong number of arguments" },
	{ "SE101", "Declaring a global variable multiple times with the same name" },
	{ "SE102", "Declaring a global variable with the same name as a procedure" },
	{ "SE103", "Declaring a procedure multiple times with the same name" },
	{ "SE104", "Declaring a procedure with the same name as a global variable" },
	{ "SE105", "Declaring a parameter multiple times with the same name in the same procedure" },
	{ "SE106", "Declaring a parameter with the same name as a global variable" },
	{ "SE107", "Declaring a parameter with the same name as a procedure" },
	{ "SE108", "Declaring a local variable multiple times with the same name in the same scope" },
	{ "SE109", "Declaring a local variable with the same name as a global variable" },
	{ "SE110", "Declaring a local variable the same name as a procedure" },
	{ "SE111", "Declaring a local variable with the same name as a parameter in the same scope" }
};

TreeNode * root;

extern TreeNodeFactory * factory;
extern STORAGE_TYPE storageForNextDecl;


extern FILE * yyin;
extern FILE * yyout;
extern int yylineno;

void yyerror(const char *);
void semantic_error(SEMANTIC_ERROR err, const char * param);

int yylex(void);

%}

%union
{
	TreeNode * node;
	BlockTreeNode * blocknode;
	ParamsTreeNode * paramsnode;
	ArgsTreeNode * argsnode;
	DeclarationTreeNode * declnode;
	int value;
	SymbolTable::Entry * symentry;
	COLOR_TYPE color_type;
}

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

%token INT
%token <symentry> ID

%token <value> NUMBER
%token <color_type> COLOR_NAME

%type <blocknode> statements
%type <node> statement
%type <node> expression
%type <node> condition
%type <node> function
%type <declnode> vardecl
%type <paramsnode> procsig;
%type <paramsnode> params;
%type <argsnode> args;

%left '+' '-'
%left '*' '/'


%%

statements:	statement statements		{
											if ($1 != NULL)
											{
												$$ = factory->CreateBlock();
												$$->AddChild($1);
												if ($2 != NULL) 
												{
													$$->AdoptChildren($2);
												}
												root = $$;
											} 
											else 
											{
												$$ = $2;
											}
										}
	|									{ $$ = NULL; }
	;

statement:	HOME						{ $$ = factory->CreateTurtleCmd(CMD_HOME); }
	|	FD expression					{ $$ = factory->CreateTurtleCmd(CMD_FD, $2); }
	|	BK expression					{ $$ = factory->CreateTurtleCmd(CMD_BK, $2); }
	|	RT expression					{ $$ = factory->CreateTurtleCmd(CMD_RT, $2); }
	|	SETH expression					{ $$ = factory->CreateTurtleCmd(CMD_SETH, $2); }
	|	LT expression					{ $$ = factory->CreateTurtleCmd(CMD_LT, $2); }
	|	SETC expression					{ $$ = factory->CreateTurtleCmd(CMD_SETC, $2); }
	|	PD								{ $$ = factory->CreateTurtleCmd(CMD_PD); }
	|	PU								{ $$ = factory->CreateTurtleCmd(CMD_PU); }
	|	ST								{ $$ = factory->CreateTurtleCmd(CMD_ST); }
	|	HT								{ $$ = factory->CreateTurtleCmd(CMD_HT); }
	|	SETXY expression expression		{ $$ = factory->CreateTurtleCmd(CMD_SETXY, $2, $3); }
	|	SETY expression					{ $$ = factory->CreateTurtleCmd(CMD_SETY, $2); }
	|	SETX expression					{ $$ = factory->CreateTurtleCmd(CMD_SETX, $2); }
	|	IF '('	condition	')'	'['	statements ']'	{ 
														$$ = factory->CreateIf($3, ($6 == NULL) ? factory->CreateBlock() : $6);
													}
	|	IFELSE '(' condition ')' '[' statements ']' '[' statements ']'	{ 
																			$$ = factory->CreateIfElse(
																				$3, 
																				($6 == NULL) ? factory->CreateBlock() : $6, 
																				($9 == NULL) ? factory->CreateBlock() : $9); 
																		}
	|	REPEAT expression '[' statements ']'	{ $$ = factory->CreateRepeat($2, ($4 == NULL) ? factory->CreateBlock() : $4); }
	|	vardecl							{ $$ = $1; }
	|	ID '=' expression				{ 
											if (!$1->IsDefined())
											{
												semantic_error(SE000, $1->Lexeme().c_str());
												YYABORT;
											}
											else if ($1->Type() == IT_PROC)
											{
												semantic_error(SE003, $1->Lexeme().c_str());
												YYABORT;
											}
											$$ = factory->CreateAssignment(factory->CreateVariable($1), $3); 
										}
	|	procsig '[' statements ']'		{
											int localScope = SymbolTable::GetInstance()->LastCreatedScope();
											$$ = factory->CreateProcDef(
												$1->SymEntry(),
												localScope,
												$1,
												($3 == NULL) ? factory->CreateBlock() : $3
											);
										}
	|	ID '(' args ')'					{ 
											// Check # args vs # params
											if (!$1->IsDefined())
											{
												semantic_error(SE001, $1->Lexeme().c_str());
												YYABORT;
											}
											else if ($1->Type() == IT_VARIABLE)
											{
												semantic_error(SE002, $1->Lexeme().c_str());
												YYABORT;
											}
											else if ($1->Params() != $3->GetChildren().size())
											{
												semantic_error(SE004, $1->Lexeme().c_str());
												YYABORT;
											}
											$$ = factory->CreateProcCall($1, $3); 
										}
	|	RETURN							{ $$ = factory->CreateReturn(); }
	|	RETURN expression				{ $$ = factory->CreateReturn($2); }
	;

vardecl:	INT ID						{ 
											if ($2->IsDefined())
											{
												// TODO: Check combinations of global/local/param/proc
												if ($2->Scope() == ST_GLOBAL)
												{
													if ($2->Type() == IT_PROC)
													{
														if (storageForNextDecl == ST_LOCAL)
														{
															semantic_error(SE110, $2->Lexeme().c_str());
														}
														else 
														{
															semantic_error(SE102, $2->Lexeme().c_str());
														}
													}
													else if ($2->Type() == IT_VARIABLE)
													{
														if (storageForNextDecl == ST_LOCAL)
														{
															semantic_error(SE109, $2->Lexeme().c_str());
														}
														else
														{
															semantic_error(SE101, $2->Lexeme().c_str());
														}
													}
												}
												else if ($2->Scope() == ST_LOCAL)
												{
													semantic_error(SE108, $2->Lexeme().c_str());
												}
												else if ($2->Scope() == ST_PARAM)
												{
													if (storageForNextDecl == ST_LOCAL)
													{
														semantic_error(SE111, $2->Lexeme().c_str());
													}
													else
													{
														semantic_error(SE105, $2->Lexeme().c_str());
													}
												}
												YYABORT;
											}
											$2->Storage() = storageForNextDecl;
											$2->Type() = IT_VARIABLE;
											$$ = factory->CreateDeclaration(IT_VARIABLE, factory->CreateVariable($2)); 
										}
	;

procsig:	TO ID '(' params ')'		{ 
											if ($2->IsDefined())
											{
												if ($2->Type() == IT_PROC)
												{
													semantic_error(SE103, $2->Lexeme().c_str());
												}
												else if ($2->Type() == IT_VARIABLE)
												{
													if ($2->Scope() == ST_GLOBAL)
													{
														semantic_error(SE104, $2->Lexeme().c_str());
													}
												}
												YYABORT;
											}
											$2->Type() = IT_PROC;
											$2->Params() = $4->GetChildren().size();
											$4->SymEntry() = $2;
											$$ = $4;
										}
	;

params:	vardecl params					{
											$$ = factory->CreateParams();
											$$->AddChild($1);
											$$->AdoptChildren($2);
										}
	|									{ $$ = factory->CreateParams(); }
	;

args:	expression args					{
											$$ = factory->CreateArgs();
											$$->AddChild($1);
											$$->AdoptChildren($2);
										}
	|									{$$ = factory->CreateArgs(); }
	;

expression:	expression '+' expression	{ $$ = factory->CreateOperator(OT_PLUS, $1, $3); }
	|	expression '-' expression		{ $$ = factory->CreateOperator(OT_MINUS, $1, $3); }
	|	expression '*' expression		{ $$ = factory->CreateOperator(OT_TIMES, $1, $3); }
	|	expression '/' expression		{ $$ = factory->CreateOperator(OT_DIVIDE, $1, $3); }
	|	'(' expression ')'				{ $$ = $2; }
	|	NUMBER							{ $$ = factory->CreateNumber($1); }
	|	COLOR_NAME						{ $$ = factory->CreateColorName($1); }
	|	function						{ $$ = $1; }
	|	ID								{ 
											if (!$1->IsDefined())
											{
												semantic_error(SE000, $1->Lexeme().c_str());
												YYABORT;
											}
											// TODO: Check if its a variable
											$$ = factory->CreateVariable($1);
										}
	|	ID '(' args ')'					{ 
											if (!$1->IsDefined())
											{
												semantic_error(SE001, $1->Lexeme().c_str());
												YYABORT;
											}
											else if ($1->Type() == IT_VARIABLE)
											{
												semantic_error(SE002, $1->Lexeme().c_str());
												YYABORT;
											}
											else if ($1->Params() != $3->GetChildren().size())
											{
												semantic_error(SE004, $1->Lexeme().c_str());
												YYABORT;
											}
											$$ = factory->CreateProcCall($1, $3); 
										}
	;

function:	COLOR						{ $$ = factory->CreateFunction(FT_COLOR); }
	|	XCOR							{ $$ = factory->CreateFunction(FT_XCOR); }
	|	YCOR							{ $$ = factory->CreateFunction(FT_YCOR); }
	|	HEADING							{ $$ = factory->CreateFunction(FT_HEADING); }
	|	RANDOM '(' expression ')'		{ $$ = factory->CreateFunction(FT_RANDOM, $3); }
	;

condition: expression '=' expression	{ $$ = factory->CreateOperator(OT_EQUALS, $1, $3); }
	|	expression '<' expression		{ $$ = factory->CreateOperator(OT_LESSTHAN, $1, $3); }
	|	expression '>' expression		{ $$ = factory->CreateOperator(OT_GREATERTHAN, $1, $3); }
	;
%%

void semantic_error(SEMANTIC_ERROR err, const char * param)
{
	char msg[256];
	sprintf_s(msg, sizeof(msg), "%s, %s, name='%s'", SEMANTIC_ERROR_STRINGS[err][0], SEMANTIC_ERROR_STRINGS[err][1], param);
	yyerror(msg);
}
