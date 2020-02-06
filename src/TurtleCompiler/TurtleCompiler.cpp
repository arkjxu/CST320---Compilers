// TurtleCompiler.cpp
// by Pete Myers
// OIT Jan 2017
// C++ Turtle Compiler
//
// Assignment 2 handout
//

#include <stdio.h>
#include <stdlib.h>


int yyparse();

extern FILE * yyin;
extern FILE * yyout;


int main(int argc, char ** argv)
{
	FILE * in_file = NULL;
	FILE * out_file = NULL;

	// usage: TurtleCompiler [<in_fname> [<out_fname>]]
	// where...
	//	<in_fname> is the source file, defaults to stdin
	//	<out_fname> is the target file, defaults to stdout
	if (argc > 1)
	{
		fopen_s(&in_file, argv[1], "r");
		if (in_file == NULL)
		{
			fprintf(stderr, "Cannot open input file %s\n", argv[1]);
			exit(-1);
		}
		yyin = in_file;
	}
	if (argc > 2)
	{
		fopen_s(&out_file, argv[2], "wb");
		if (out_file == NULL)
		{
			fprintf(stderr, "Cannot open output file %s\n", argv[2]);
			exit(-1);
		}
		yyout = out_file;
	}

	yyparse();

	if (in_file != NULL)
	{
		fclose(in_file);
	}

	if (out_file != NULL)
	{
		fflush(out_file);
		fclose(out_file);
	}

	return 0;
}

