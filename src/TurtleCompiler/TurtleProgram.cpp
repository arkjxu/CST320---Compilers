// TurtleProgram2.cpp
// by Pete Myers
// OIT Mar 2017, Winter 2020
//

#include "TurtleProgram.h"


TurtleProgram::TurtleProgram(int stackSize)
{
	this->stackSize = stackSize;
	this->program = NULL;
}

TurtleProgram::~TurtleProgram()
{
	if (program != NULL)
	{
		delete[] program;
	}
}

void TurtleProgram::ResetBinary()
{
	// clear out the binary representation if it exists

	// binary code
	if (program != NULL)
		delete[] program;
	program = NULL;

	// addresses for instructions and labels
	std::list<Element*>::iterator iter;
	for (iter = elements.begin(); iter != elements.end(); iter++)
	{
		(*iter)->ResetAddress();
	}

	// use locations for labels
	std::list<Label*>::iterator liter;
	for (liter = labels.begin(); liter != labels.end(); liter++)
	{
		Label* label = (*liter);
		label->ResetUses();
	}
}

void TurtleProgram::CommitBinary()
{
	ResetBinary();

	program = new unsigned char[MAX_PROGRAM];
	int pc = 0;

	// header
	pc += 8;

	// static variables
	program[2] = hibyte(pc);
	program[3] = lobyte(pc);
	pc = GenerateStaticVariables(pc);

	// program
	program[4] = hibyte(pc);
	program[5] = lobyte(pc);
	pc = GenerateInstructions(pc);

	// stack
	program[6] = hibyte(pc);
	program[7] = lobyte(pc);
	pc += stackSize;

	// fix up header's size member
	program[0] = hibyte(pc);
	program[1] = lobyte(pc);
}

int TurtleProgram::GenerateInstructions(int pc)
{
	std::list<Element*>::iterator iter;
	for (iter = elements.begin(); iter != elements.end(); iter++)
	{
		// record addres of instruction/label
		(*iter)->Address() = pc;

		if ((*iter)->IsInstruction())
		{
			Instruction* instruction = (Instruction*)(*iter);
			program[pc++] = instruction->opcode;

			switch (instruction->opcode)
			{
			case OPCODE_LOAD_R:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.val);
				program[pc++] = lobyte(instruction->operand2.val);
				break;

			case OPCODE_LOAD_V:
				program[pc++] = hibyte(instruction->operand1.var->Address());
				program[pc++] = lobyte(instruction->operand1.var->Address());
				program[pc++] = hibyte(instruction->operand2.val);
				program[pc++] = lobyte(instruction->operand2.val);
				break;

			case OPCODE_LOAD_O:
				program[pc++] = instruction->operand1.offset;
				program[pc++] = hibyte(instruction->operand2.val);
				program[pc++] = lobyte(instruction->operand2.val);
				break;

			case OPCODE_MOVE_RR:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_MOVE_RV:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_MOVE_VR:
				program[pc++] = hibyte(instruction->operand1.var->Address());
				program[pc++] = lobyte(instruction->operand1.var->Address());
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_MOVE_VV:
				program[pc++] = hibyte(instruction->operand1.var->Address());
				program[pc++] = lobyte(instruction->operand1.var->Address());
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_MOVE_RO:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.offset;
				break;

			case OPCODE_MOVE_OR:
				program[pc++] = instruction->operand1.offset;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_JMPTo:
				instruction->operand1.label->AddAbsoluteUse(pc);
				pc += 2;	// leave hole for label
				break;

			case OPCODE_JMPRe:
			case OPCODE_JEq:
			case OPCODE_JNe:
			case OPCODE_JGt:
			case OPCODE_JLt:
			case OPCODE_JZe:
			case OPCODE_JNz:
				instruction->operand1.label->AddRelativeUse(pc);
				pc += 2;	// leave hole for label
				break;

			case OPCODE_CMP_RR:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_CMP_RV:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_CMP_VR:
				program[pc++] = hibyte(instruction->operand1.var->Address());
				program[pc++] = lobyte(instruction->operand1.var->Address());
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_CMP_VV:
				program[pc++] = hibyte(instruction->operand1.var->Address());
				program[pc++] = lobyte(instruction->operand1.var->Address());
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_INC_R:
				program[pc++] = instruction->operand1.reg;
				break;

			case OPCODE_DEC_R:
				program[pc++] = instruction->operand1.reg;
				break;

			case OPCODE_ADD_R:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_ADD_V:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_ADD_O:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.offset;
				break;

			case OPCODE_SUB_R:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_SUB_V:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_SUB_O:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.offset;
				break;

			case OPCODE_MUL_R:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_MUL_V:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_MUL_O:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.offset;
				break;

			case OPCODE_DIV_R:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.reg;
				break;

			case OPCODE_DIV_V:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = hibyte(instruction->operand2.var->Address());
				program[pc++] = lobyte(instruction->operand2.var->Address());
				break;

			case OPCODE_DIV_O:
				program[pc++] = instruction->operand1.reg;
				program[pc++] = instruction->operand2.offset;
				break;

			case OPCODE_PUSH_R:
				program[pc++] = instruction->operand1.reg;
				break;

			case OPCODE_POP_R:
				program[pc++] = instruction->operand1.reg;
				break;

			case OPCODE_PEEK_R:
				program[pc++] = instruction->operand1.reg;
				break;

			case OPCODE_TURTLE:
				program[pc++] = instruction->operand1.top;
				break;

			case OPCODE_CALL:
				instruction->operand1.label->AddAbsoluteUse(pc);
				pc += 2;	// leave hole for label
				break;

			case OPCODE_ENTER:
				program[pc++] = instruction->operand1.offset;
				break;

			case OPCODE_LEAVE:
				break;

			case OPCODE_RET:
				program[pc++] = instruction->operand1.offset;
				break;

			case OPCODE_EXIT:
				break;

			default:
				break;
			}
		}
	}

	// back-patch labels
	std::list<Label*>::iterator liter;
	for (liter = labels.begin(); liter != labels.end(); liter++)
	{
		Label* label = (*liter);

		std::list<Label::LabelUse>::iterator uiter;
		for (uiter = label->Uses().begin(); uiter != label->Uses().end(); uiter++)
		{
			Label::LabelUse use = *uiter;
			if (use.relative)
			{
				// NOTE: +2 because the use is in the middle of the jmp instruction
				program[use.at] = hibyte(label->Address() - (use.at + 2));
				program[use.at + 1] = lobyte(label->Address() - (use.at + 2));
			}
			else
			{
				program[use.at] = hibyte(label->Address());
				program[use.at + 1] = lobyte(label->Address());
			}
		}
	}

	return pc;
}


int TurtleProgram::GenerateStaticVariables(int pc)
{
	// for each variable in symtable, determine its static address
	std::list<SymbolTable::Entry*> symbols = SymbolTable::GetInstance()->GetGlobalEntries();
	std::list<SymbolTable::Entry*>::iterator iter;
	for (iter = symbols.begin(); iter != symbols.end(); iter++)
	{
		if ((*iter)->Storage() == ST_GLOBAL && (*iter)->Type() == IT_VARIABLE)
		{
			// set variable's address
			(*iter)->Address() = pc;
			pc += 2;	// reserve 2-bytes for int
		}
	}

	return pc;
}

int TurtleProgram::GetSize()
{
	return makeint(program[0], program[1]);
}

void TurtleProgram::PrintProgram(FILE* f)
{
	int i;
	OPCODE op;

	// header
	int size = makeint(program[0], program[1]);
	int variables = makeint(program[2], program[3]);
	int code = makeint(program[4], program[5]);
	int stack = makeint(program[6], program[7]);
	fprintf(f, "header->\n");
	fprintf(f, "    size      = 0x%04hX\n", size);
	fprintf(f, "    variables = 0x%04hX\n", variables);
	fprintf(f, "    code      = 0x%04hX\n", code);
	fprintf(f, "    stack     = 0x%04hX\n", stack);
	fprintf(f, "<-header\n");

	// static variables
	fprintf(f, "static variables->\n");
	fprintf(f, "    %d variables\n", (code - variables) / 2);
	if (code > variables)
	{
		fprintf(f, "    addr   = initial_value\n");
		fprintf(f, "    ------   -------------\n");
		for (i = variables; i < code; i += 2)
		{
			fprintf(f, "    0x%04hX = 0x%04hX\n", i, makeint(program[i], program[i + 1]));
		}
	}
	fprintf(f, "<-static variables\n");

	// code
	fprintf(f, "code->\n");
	i = code;
	do
	{
		op = (OPCODE)program[i];
		i = PrintOpcode(f, i);
	} while (op != OPCODE_EXIT);
	fprintf(f, "<-code\n");

	// stack
	fprintf(f, "stack->\n");
	fprintf(f, "    0x%04hX bytes reserved\n", size - stack);
	fprintf(f, "<-stack\n");
}

int TurtleProgram::PrintOpcode(FILE* f, int i)
{
	REGISTER r1;
	REGISTER r2;
	unsigned char valh;
	unsigned char vall;
	unsigned char var1h;
	unsigned char var1l;
	unsigned char var2h;
	unsigned char var2l;
	unsigned char deltah;
	unsigned char deltal;
	int delta;
	unsigned char addrh;
	unsigned char addrl;
	char off;
	TURTLE_OPERATION t;

	// print memory address first
	fprintf(f, "    0x%04hX ", i & 0xffff);

	// find the opcode
	OPCODE op = (OPCODE)program[i++];

	switch (op)
	{
	case OPCODE_LOAD_R:
		r1 = (REGISTER)program[i++];
		valh = program[i++];
		vall = program[i++];
		fprintf(f, "LOAD_R  %s %d\n", RegisterName(r1), makeint(valh, vall));
		break;

	case OPCODE_LOAD_V:
		var1h = program[i++];
		var1l = program[i++];
		valh = program[i++];
		vall = program[i++];
		fprintf(f, "LOAD_V  0x%04hX %d\n", makeint(var1h, var1l), makeint(valh, vall));
		break;

	case OPCODE_LOAD_O:
		off = program[i++];
		valh = program[i++];
		vall = program[i++];
		fprintf(f, "LOAD_O  %d %d\n", (int)off, makeint(valh, vall));
		break;

	case OPCODE_MOVE_RR:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "MOVE_RR %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_MOVE_RV:
		r1 = (REGISTER)program[i++];
		var1h = program[i++];
		var1l = program[i++];
		fprintf(f, "MOVE_RV %s 0x%04hX\n", RegisterName(r1), makeint(var1h, var1l));
		break;

	case OPCODE_MOVE_VR:
		var1h = program[i++];
		var1l = program[i++];
		r1 = (REGISTER)program[i++];
		fprintf(f, "MOVE_VR 0x%04hX %s\n", makeint(var1h, var1l), RegisterName(r1));
		break;

	case OPCODE_MOVE_VV:
		var1h = program[i++];
		var1l = program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "MOVE_VV 0x%04hX 0x%04hX\n", makeint(var1h, var1l), makeint(var2h, var2l));
		break;

	case OPCODE_MOVE_RO:
		r1 = (REGISTER)program[i++];
		off = program[i++];
		fprintf(f, "MOVE_RO %s %d\n", RegisterName(r1), (int)off);
		break;

	case OPCODE_MOVE_OR:
		off = program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "MOVE_OR %d %s\n", (int)off, RegisterName(r2));
		break;

	case OPCODE_JMPRe:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JMPRe   0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_JMPTo:
		addrh = program[i++];
		addrl = program[i++];
		fprintf(f, "JMPTo   0x%04hX\n", makeint(addrh, addrl));
		break;

	case OPCODE_JEq:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JEq     0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_JNe:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JNe     0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_JGt:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JGt     0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_JLt:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JLt     0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_JZe:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JZe     0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_JNz:
		deltah = program[i++];
		deltal = program[i++];
		delta = makeint(deltah, deltal);
		fprintf(f, "JNz     0x%04hX (to 0x%04hX)\n", delta, i + delta);
		break;

	case OPCODE_CMP_RR:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "CMP_RR  %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_CMP_RV:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "CMP_RV  %s 0x%04hX\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_CMP_VR:
		var1h = program[i++];
		var1l = program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "CMP_VR  0x%04hX %s\n", makeint(var1h, var1l), RegisterName(r2));
		break;

	case OPCODE_CMP_VV:
		var1h = program[i++];
		var1l = program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "CMP_VV  0x%04hX 0x%04hX\n", makeint(var1h, var1l), makeint(var2h, var2l));
		break;

	case OPCODE_INC_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "INC_R   %s\n", RegisterName(r1));
		break;

	case OPCODE_DEC_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "DEC_R   %s\n", RegisterName(r1));
		break;

	case OPCODE_ADD_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "ADD_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_ADD_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "ADD_V   %s 0x%04hX\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_ADD_O:
		r1 = (REGISTER)program[i++];
		off = program[i++];
		fprintf(f, "ADD_O   %s %d\n", RegisterName(r1), (int)off);
		break;

	case OPCODE_SUB_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "SUB_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_SUB_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "SUB_V   %s 0x%04hX\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_SUB_O:
		r1 = (REGISTER)program[i++];
		off = program[i++];
		fprintf(f, "SUB_O   %s %d\n", RegisterName(r1), (int)off);
		break;

	case OPCODE_MUL_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "MUL_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_MUL_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "MUL_V   %s 0x%04hX\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_MUL_O:
		r1 = (REGISTER)program[i++];
		off = program[i++];
		fprintf(f, "MUL_O   %s %d\n", RegisterName(r1), (int)off);
		break;

	case OPCODE_DIV_R:
		r1 = (REGISTER)program[i++];
		r2 = (REGISTER)program[i++];
		fprintf(f, "DIV_R   %s %s\n", RegisterName(r1), RegisterName(r2));
		break;

	case OPCODE_DIV_V:
		r1 = (REGISTER)program[i++];
		var2h = program[i++];
		var2l = program[i++];
		fprintf(f, "DIV_V   %s 0x%04hX\n", RegisterName(r1), makeint(var2h, var2l));
		break;

	case OPCODE_DIV_O:
		r1 = (REGISTER)program[i++];
		off = program[i++];
		fprintf(f, "DIV_O   %s %d\n", RegisterName(r1), (int)off);
		break;

	case OPCODE_PUSH_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "PUSH_R  %s\n", RegisterName(r1));
		break;

	case OPCODE_POP_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "POP_R   %s\n", RegisterName(r1));
		break;

	case OPCODE_PEEK_R:
		r1 = (REGISTER)program[i++];
		fprintf(f, "PEEK_R  %s\n", RegisterName(r1));
		break;

	case OPCODE_TURTLE:
		t = (TURTLE_OPERATION)program[i++];
		fprintf(f, "TURTLE  %s\n", TurtleOperationString(t));
		break;

	case OPCODE_CALL:
		addrh = program[i++];
		addrl = program[i++];
		fprintf(f, "CALL    0x%04hX\n", makeint(addrh, addrl));
		break;

	case OPCODE_ENTER:
		off = program[i++];
		fprintf(f, "ENTER   %d\n", (int)off);
		break;

	case OPCODE_LEAVE:
		fprintf(f, "LEAVE\n");
		break;

	case OPCODE_RET:
		off = program[i++];
		fprintf(f, "RET     %d\n", (int)off);
		break;

	case OPCODE_EXIT:
		fprintf(f, "EXIT\n");
		break;

	default:
		fprintf(f, "UNKNOWN\n");
		break;
	}

	return i;
}

char* TurtleProgram::RegisterName(REGISTER r)
{
	switch (r)
	{
	case REGISTER_P1:
		return "p1";
	case REGISTER_P2:
		return "p2";
	case REGISTER_RE:
		return "re";
	case REGISTER_G1:
		return "g1";
	case REGISTER_G2:
		return "g2";
	case REGISTER_G3:
		return "g3";
	case REGISTER_G4:
		return "g4";
	case REGISTER_PC:
		return "pc";
	case REGISTER_ST:
		return "st";
	case REGISTER_FL:
		return "fl";
	case REGISTER_BP:
		return "bp";
	default:
		return "UNKNOWN";
	}
}

char* TurtleProgram::TurtleOperationString(TURTLE_OPERATION t)
{
	switch (t)
	{
	case TURTLE_OPERATION_HOME:
		return "HOME";
	case TURTLE_OPERATION_FD:
		return "FD";
	case TURTLE_OPERATION_BK:
		return "BK";
	case TURTLE_OPERATION_RT:
		return "RT";
	case TURTLE_OPERATION_LT:
		return "LT";
	case TURTLE_OPERATION_PU:
		return "PU";
	case TURTLE_OPERATION_PD:
		return "PD";
	case TURTLE_OPERATION_SETC:
		return "SETC";
	case TURTLE_OPERATION_SETXY:
		return "SETXY";
	case TURTLE_OPERATION_COLOR:
		return "COLOR";
	case TURTLE_OPERATION_XCOR:
		return "XCOR";
	case TURTLE_OPERATION_YCOR:
		return "YCOR";
	case TURTLE_OPERATION_RANDOM:
		return "RANDOM";
	case TURTLE_OPERATION_SETH:
		return "SETH";
	case TURTLE_OPERATION_SETX:
		return "SETX";
	case TURTLE_OPERATION_SETY:
		return "SETY";
	case TURTLE_OPERATION_HT:
		return "HT";
	case TURTLE_OPERATION_ST:
		return "ST";
	case TURTLE_OPERATION_HEADING:
		return "HEADING";
	default:
		return "UNKNOWN";
	}
}

void TurtleProgram::WriteBinary(FILE* f)
{
	fwrite(program, 1, GetSize(), f);
}


TurtleProgram::Label* TurtleProgram::LABEL()
{
	Label* l = new Label();
	labels.push_back(l);
	return l;
}

void TurtleProgram::PlaceLabel(TurtleProgram::Label* label)
{
	elements.push_back(label);
	label->Placed() = true;
}

void TurtleProgram::LOAD_R(REGISTER reg, int val)
{
	elements.push_back(new Instruction(OPCODE_LOAD_R, reg, val));
}

void TurtleProgram::LOAD_V(SymbolTable::Entry* var, int val)
{
	elements.push_back(new Instruction(OPCODE_LOAD_V, var, val));
}

void TurtleProgram::LOAD_O(char offset, int val)
{
	elements.push_back(new Instruction(OPCODE_LOAD_O, offset, val));
}

void TurtleProgram::MOVE_RR(REGISTER reg1, REGISTER reg2)
{
	elements.push_back(new Instruction(OPCODE_MOVE_RR, reg1, reg2));
}

void TurtleProgram::MOVE_RV(REGISTER reg, SymbolTable::Entry* var)
{
	elements.push_back(new Instruction(OPCODE_MOVE_RV, reg, var));
}

void TurtleProgram::MOVE_VR(SymbolTable::Entry* var, REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_MOVE_VR, var, reg));
}

void TurtleProgram::MOVE_VV(SymbolTable::Entry* var1, SymbolTable::Entry* var2)
{
	elements.push_back(new Instruction(OPCODE_MOVE_VV, var1, var2));
}

void TurtleProgram::MOVE_RO(REGISTER reg, char offset)
{
	elements.push_back(new Instruction(OPCODE_MOVE_RO, reg, offset));
}

void TurtleProgram::MOVE_OR(char offset, REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_MOVE_OR, offset, reg));
}

void TurtleProgram::JMPRe(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JMPRe, label));
}

void TurtleProgram::JMPTo(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JMPTo, label));
}

void TurtleProgram::JEq(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JEq, label));
}

void TurtleProgram::JNe(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JNe, label));
}

void TurtleProgram::JGt(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JGt, label));
}

void TurtleProgram::JLt(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JLt, label));
}

void TurtleProgram::JZe(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JZe, label));
}

void TurtleProgram::JNz(Label* label)
{
	elements.push_back(new Instruction(OPCODE_JNz, label));
}

void TurtleProgram::CMP_RR(REGISTER reg1, REGISTER reg2)
{
	elements.push_back(new Instruction(OPCODE_CMP_RR, reg1, reg2));
}

void TurtleProgram::CMP_RV(REGISTER reg, SymbolTable::Entry* var)
{
	elements.push_back(new Instruction(OPCODE_CMP_RV, reg, var));
}

void TurtleProgram::CMP_VR(SymbolTable::Entry* var, REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_CMP_VR, var, reg));
}

void TurtleProgram::CMP_VV(SymbolTable::Entry* var1, SymbolTable::Entry* var2)
{
	elements.push_back(new Instruction(OPCODE_CMP_VV, var1, var2));
}

void TurtleProgram::INC_R(REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_INC_R, reg));
}

void TurtleProgram::DEC_R(REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_DEC_R, reg));
}

void TurtleProgram::ADD_R(REGISTER reg1, REGISTER reg2)
{
	elements.push_back(new Instruction(OPCODE_ADD_R, reg1, reg2));
}

void TurtleProgram::ADD_V(REGISTER reg, SymbolTable::Entry* var)
{
	elements.push_back(new Instruction(OPCODE_ADD_V, reg, var));
}

void TurtleProgram::ADD_O(REGISTER reg, char offset)
{
	elements.push_back(new Instruction(OPCODE_ADD_O, reg, offset));
}

void TurtleProgram::SUB_R(REGISTER reg1, REGISTER reg2)
{
	elements.push_back(new Instruction(OPCODE_SUB_R, reg1, reg2));
}

void TurtleProgram::SUB_V(REGISTER reg, SymbolTable::Entry* var)
{
	elements.push_back(new Instruction(OPCODE_SUB_V, reg, var));
}

void TurtleProgram::SUB_O(REGISTER reg, char offset)
{
	elements.push_back(new Instruction(OPCODE_SUB_O, reg, offset));
}

void TurtleProgram::MUL_R(REGISTER reg1, REGISTER reg2)
{
	elements.push_back(new Instruction(OPCODE_MUL_R, reg1, reg2));
}

void TurtleProgram::MUL_V(REGISTER reg, SymbolTable::Entry* var)
{
	elements.push_back(new Instruction(OPCODE_MUL_V, reg, var));
}

void TurtleProgram::MUL_O(REGISTER reg, char offset)
{
	elements.push_back(new Instruction(OPCODE_MUL_O, reg, offset));
}

void TurtleProgram::DIV_R(REGISTER reg1, REGISTER reg2)
{
	elements.push_back(new Instruction(OPCODE_DIV_R, reg1, reg2));
}

void TurtleProgram::DIV_V(REGISTER reg, SymbolTable::Entry* var)
{
	elements.push_back(new Instruction(OPCODE_DIV_V, reg, var));
}

void TurtleProgram::DIV_O(REGISTER reg, char offset)
{
	elements.push_back(new Instruction(OPCODE_DIV_O, reg, offset));
}

void TurtleProgram::PUSH_R(REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_PUSH_R, reg));
}

void TurtleProgram::POP_R(REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_POP_R, reg));
}

void TurtleProgram::PEEK_R(REGISTER reg)
{
	elements.push_back(new Instruction(OPCODE_PEEK_R, reg));
}

void TurtleProgram::TURTLE(TURTLE_OPERATION op)
{
	elements.push_back(new Instruction(OPCODE_TURTLE, op));
}

void TurtleProgram::CALL(Label* label)
{
	elements.push_back(new Instruction(OPCODE_CALL, label));
}

void TurtleProgram::ENTER(char offset)
{
	elements.push_back(new Instruction(OPCODE_ENTER, offset));
}

void TurtleProgram::LEAVE()
{
	elements.push_back(new Instruction(OPCODE_LEAVE));
}

void TurtleProgram::RET(char offset)
{
	elements.push_back(new Instruction(OPCODE_RET, offset));
}

void TurtleProgram::EXIT()
{
	elements.push_back(new Instruction(OPCODE_EXIT));
}