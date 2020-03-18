// codegen.c
// jiawei xu
// OIT winter 2020, Winter 2020
//
// Assignment 4
//

#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "tree.h"
#include "symtable.h"
#include "TurtleYacc.tab.h"





//
// functions to help generate program
//


TURTLE_OPERATION func_to_turtle_op(FUNCTION_TYPE func)
{
	switch (func)
	{
	case FT_COLOR:
		return TURTLE_OPERATION_COLOR;
	case FT_XCOR:
		return TURTLE_OPERATION_XCOR;
	case FT_YCOR:
		return TURTLE_OPERATION_YCOR;
	case FT_RANDOM:
		return TURTLE_OPERATION_RANDOM;
	case FT_HEADING:
		return TURTLE_OPERATION_HEADING;
	default:
		return (TURTLE_OPERATION)-1;
	}
}

TURTLE_OPERATION turtle_cmd_to_turtle_op(TURTLE_CMD cmd)
{
	switch (cmd)
	{
	case CMD_HOME:
		return TURTLE_OPERATION_HOME;
	case CMD_PU:
		return TURTLE_OPERATION_PU;
	case CMD_PD:
		return TURTLE_OPERATION_PD;
	case CMD_FD:
		return TURTLE_OPERATION_FD;
	case CMD_BK:
		return TURTLE_OPERATION_BK;
	case CMD_RT:
		return TURTLE_OPERATION_RT;
	case CMD_LT:
		return TURTLE_OPERATION_LT;
	case CMD_SETC:
		return TURTLE_OPERATION_SETC;
	case CMD_SETXY:
		return TURTLE_OPERATION_SETXY;
	case CMD_SETH:
		return TURTLE_OPERATION_SETH;
	case CMD_SETX:
		return TURTLE_OPERATION_SETX;
	case CMD_SETY:
		return TURTLE_OPERATION_SETY;
	case CMD_HT:
		return TURTLE_OPERATION_HT;
	case CMD_ST:
		return TURTLE_OPERATION_ST;
	default:
		return (TURTLE_OPERATION)-1;
	}
}

COLORS color_type_to_colors(COLOR_TYPE c)
{
	switch (c)
	{
	case BLACK:
		return COLOR_BLACK;
	case WHITE:
		return COLOR_WHITE;
	case ORANGE:
		return COLOR_ORANGE;
	case YELLOW:
		return COLOR_YELLOW;
	case LIME:
		return COLOR_LIME;
	case CYAN:
		return COLOR_CYAN;
	case BLUE:
		return COLOR_BLUE;
	case MAGENTA:
		return COLOR_MAGENTA;
	case RED:
		return COLOR_RED;
	case BROWN:
		return COLOR_BROWN;
	case GREEN:
		return COLOR_GREEN;
	case TURQUOISE:
		return COLOR_TURQUOISE;
	case SKY:
		return COLOR_SKY;
	case VIOLET:
		return COLOR_VIOLET;
	case PINK:
		return COLOR_PINK;
	default:
		return (COLORS)-1;
	}
}



//
// C++ Version Jan 2017
//


// Node classes for generation

void GenBlockTreeNode::GenerateNode(TurtleProgram* program)
{
	// good to go!
	std::list<TreeNode*> children = GetChildren();
	std::list<TreeNode*>::iterator iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		(*iter)->GenerateNode(program);
	}
}

void GenNumberTreeNode::GenerateNode(TurtleProgram* program)
{
	// good to go!
	program->LOAD_R(REGISTER_G1, Value());
	program->PUSH_R(REGISTER_G1);
}

void GenColorNameTreeNode::GenerateNode(TurtleProgram* program)
{
	COLORS c = color_type_to_colors(Color());
	program->LOAD_R(REGISTER_G1, c);
	program->PUSH_R(REGISTER_G1);
}

void GenFunctionTreeNode::GenerateNode(TurtleProgram* program)
{
	if (Function() == FT_RANDOM)
	{
		TreeNode* param1 = FirstChild();
		param1->GenerateNode(program);
		program->POP_R(REGISTER_P1);
	}
	program->TURTLE(func_to_turtle_op(Function()));
	program->PUSH_R(REGISTER_RE);
}

void GenTurtleCmdTreeNode::GenerateNode(TurtleProgram* program)
{
	switch (Command())
	{
	case CMD_SETXY:
		{
			TreeNode* param1 = FirstChild();
			param1->GenerateNode(program);

			TreeNode* param2 = SecondChild();
			param2->GenerateNode(program);

			program->POP_R(REGISTER_P2);
			program->POP_R(REGISTER_P1);
			program->TURTLE(turtle_cmd_to_turtle_op(Command()));
		}
		break;
	case CMD_BK:
	case CMD_RT:
	case CMD_LT:
	case CMD_SETX:
	case CMD_SETY:
	case CMD_SETC:
	case CMD_SETH:
	case CMD_FD:
		{
			TreeNode* param1 = FirstChild();
			param1->GenerateNode(program);

			program->POP_R(REGISTER_P1);
			program->TURTLE(turtle_cmd_to_turtle_op(Command()));
		}
		break;

	case CMD_HOME:
	case CMD_PD:
	case CMD_PU:
	case CMD_ST:
	case CMD_HT:
		program->TURTLE(turtle_cmd_to_turtle_op(Command()));
		break;

	default:
		break;
	}
}

void GenIfTreeNode::GenerateNode(TurtleProgram* program)
{
	TreeNode* condition = FirstChild();
	TreeNode* trueblock = SecondChild();
	condition->GenerateNode(program);
	program->POP_R(REGISTER_G1);
	program->LOAD_R(REGISTER_G2, 0);
	program->CMP_RR(REGISTER_G1, REGISTER_G2);

	TurtleProgram::Label* skipLabel = program->LABEL();
	program->JEq(skipLabel);

	trueblock->GenerateNode(program);
	program->PlaceLabel(skipLabel);
}

void GenIfElseTreeNode::GenerateNode(TurtleProgram* program)
{
	TreeNode* condition = FirstChild();
	TreeNode* trueBlock = SecondChild();
	TreeNode* falseBlock = ThirdChild();

	condition->GenerateNode(program);

	program->POP_R(REGISTER_G1);
	program->LOAD_R(REGISTER_G2, 0);
	program->CMP_RR(REGISTER_G1, REGISTER_G2);

	TurtleProgram::Label* falseBlockLabel = program->LABEL();
	program->JEq(falseBlockLabel);

	trueBlock->GenerateNode(program);

	TurtleProgram::Label* skipLabel = program->LABEL();
	program->JMPRe(skipLabel);

	program->PlaceLabel(falseBlockLabel);

	falseBlock->GenerateNode(program);

	program->PlaceLabel(skipLabel);
}

void GenOperatorTreeNode::GenerateNode(TurtleProgram* program)
{
	TreeNode* lhs = FirstChild();
	TreeNode* rhs = SecondChild();

	// lhs child
	lhs->GenerateNode(program);

	// rhs child
	rhs->GenerateNode(program);

	// pop operands from the stack
	program->POP_R(REGISTER_G2);
	program->POP_R(REGISTER_G1);

	// do the operation
	switch (Operator())
	{
	case OT_PLUS:
		program->ADD_R(REGISTER_G1, REGISTER_G2);
		break;
	case OT_TIMES:
		program->MUL_R(REGISTER_G1, REGISTER_G2);
		break;
	case OT_MINUS:
		program->SUB_R(REGISTER_G1, REGISTER_G2);
		break;
	case OT_DIVIDE:
		program->DIV_R(REGISTER_G1, REGISTER_G2);
		break;
	case OT_EQUALS:
	case OT_LESSTHAN:
	case OT_GREATERTHAN:
		{
			TurtleProgram::Label* label = program->LABEL();
			program->CMP_RR(REGISTER_G1, REGISTER_G2);
			program->LOAD_R(REGISTER_G1, 1);

			if (Operator() == OT_EQUALS) program->JEq(label);
			else if (Operator() == OT_LESSTHAN) program->JLt(label);
			else if (Operator() == OT_GREATERTHAN) program->JGt(label);

			program->LOAD_R(REGISTER_G1, 0);
			program->PlaceLabel(label);
		}
		break;
	default:
		break;
	}

	// leave result on the stack
	program->PUSH_R(REGISTER_G1);
}

void GenRepeatTreeNode::GenerateNode(TurtleProgram* program)
{
	TreeNode* loop = FirstChild();
	TreeNode* block = SecondChild();

	loop->GenerateNode(program);

	TurtleProgram::Label* loopLabel = program->LABEL();
	program->PlaceLabel(loopLabel);


	TurtleProgram::Label* loopEndLabel = program->LABEL();
	program->PEEK_R(REGISTER_G1);
	program->LOAD_R(REGISTER_G2, 1);
	program->CMP_RR(REGISTER_G1, REGISTER_G2);
	program->JLt(loopEndLabel);

	block->GenerateNode(program);

	program->POP_R(REGISTER_G1);
	program->DEC_R(REGISTER_G1);
	program->PUSH_R(REGISTER_G1);

	program->JMPTo(loopLabel);

	program->PlaceLabel(loopEndLabel);
	program->POP_R(REGISTER_G1);
}

void GenDeclarationTreeNode::GenerateNode(TurtleProgram* program)
{
	// NOTE: nothing is required here
	// declarations are handled in the bison file by adding variable's type to the symbol table
}

void GenVariableTreeNode::GenerateNode(TurtleProgram* program)
{
	if (SymEntry()->Storage() == ST_GLOBAL)
	{
		program->MOVE_RV(REGISTER_G1, SymEntry());
	}
	else if (SymEntry()->Storage() == ST_LOCAL || SymEntry()->Storage() == ST_PARAM)
	{
		program->MOVE_RO(REGISTER_G1, SymEntry()->Address());
	}
	program->PUSH_R(REGISTER_G1);
}

void GenAssignmentTreeNode::GenerateNode(TurtleProgram* program)
{
	VariableTreeNode* lhs = static_cast<VariableTreeNode*>(FirstChild());
	TreeNode* rhs = SecondChild();
	rhs->GenerateNode(program);
	program->POP_R(REGISTER_G1);

	if (lhs->SymEntry()->Storage() == ST_GLOBAL)
	{
		//global
		program->MOVE_VR(lhs->SymEntry(), REGISTER_G1);
	}
	else if (lhs->SymEntry()->Storage() == ST_LOCAL || lhs->SymEntry()->Storage() == ST_PARAM)
	{
		int offset = lhs->SymEntry()->Address();
		program->MOVE_OR(offset, REGISTER_G1);
	}
}

void GenProcDefTreeNode::GenerateNode(TurtleProgram* program)
{
	//Create abd save the returnLabel
	TurtleProgram::Label* returnLabel = program->LABEL();
	SetReturnLabel(returnLabel);

	//Create and jump to afterLabel
	TurtleProgram::Label* afterLabel = program->LABEL();
	program->JMPTo(afterLabel);

	//Create and save the procStart Label
	TurtleProgram::Label* procStart = program->LABEL();
	SetCallLabel(procStart);

	//place the procStart label
	program->PlaceLabel(procStart);

	//generate params
	ParamsTreeNode* params = (ParamsTreeNode*)FirstChild();
	params->GenerateNode(program);
	char numParams = params->GetChildren().size();

	int numLocals = 0;
	std::list<SymbolTable::Entry*> locals = SymbolTable::GetInstance()->GetLocalEntriesForScope(LocalScope());
	int offset = 0;
	std::list<SymbolTable::Entry*>::iterator iter;
	for (iter = locals.begin(); iter != locals.end(); iter++)
	{
		if ((*iter)->Type() == IT_VARIABLE)
		{
			(*iter)->Address() = offset;
			offset++;
			numLocals++;
		}
	}


	//Emit Enter
	program->ENTER(numLocals);

	//Generate block child node
	BlockTreeNode* body = (BlockTreeNode*)SecondChild();
	body->GenerateNode(program);

	//Place return label
	program->PlaceLabel(returnLabel);

	//Emit LEAVE
	program->LEAVE();

	//emit RET
	program->RET(numParams);

	//Place label
	program->PlaceLabel(afterLabel);
}

void GenParamsTreeNode::GenerateNode(TurtleProgram* program)
{
	std::list<TreeNode*> decls = GetChildren();
	int nParams = decls.size();
	int iParams = 0;
	std::list<TreeNode*>::iterator iter;
	for (iter = decls.begin(); iter != decls.end(); iter++, iParams++)
	{
		int offset = -2 - nParams + iParams;
		GenDeclarationTreeNode* decl = (GenDeclarationTreeNode*)(*iter);
		decl->SetOffset(offset);
	}
}

void GenReturnTreeNode::GenerateNode(TurtleProgram* program)
{
	TreeNode* parent = GetParent();
	while (parent->NodeType() != NT_PROCDEF)
	{
		parent = parent->GetParent();
	}
	TurtleProgram::Label* returnLabel = ((GenProcDefTreeNode*)parent)->GetReturnLabel();

	if (GetChildren().size() > 0)
	{
		TreeNode* returnValueExpression = FirstChild();
		returnValueExpression->GenerateNode(program);
	}

	program->LOAD_R(REGISTER_RE, 0);
	program->JMPTo(returnLabel);
}

void GenProcCallTreeNode::GenerateNode(TurtleProgram* program)
{
	ArgsTreeNode* args = (ArgsTreeNode*)FirstChild();
	args->GenerateNode(program);

	TurtleProgram::Label* procStart = Label();
	program->CALL(procStart);

	if (ExpectReturnValue())
	{
		program->PUSH_R(REGISTER_RE);
	}
}

void GenArgsTreeNode::GenerateNode(TurtleProgram* program)
{
	std::list<TreeNode*> args = GetChildren();
	for (auto iter = args.begin(); iter != args.end(); iter++)
	{
		(*iter)->GenerateNode(program);
	}
}

