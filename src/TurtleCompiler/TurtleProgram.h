// TurtleProgram.h
// by Pete Myers
// OIT Mar 2017, Winter 2020
//

#ifndef _TURTLEPROGRAM_H_
#define _TURTLEPROGRAM_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <list>
#include "TurtleConstants.h"
#include "symtable.h"


class TurtleProgram
{
#pragma region Types
public:
	class Element;
	class Instruction;
	class Label;

	union Operand
	{
	public:
		int val;
		REGISTER reg;
		TURTLE_OPERATION top;
		SymbolTable::Entry* var;	// used for variables
		Label* label;				// used for jumps
		char offset;

		Operand(int val) { this->val = val; }
		Operand(REGISTER reg) { this->reg = reg; }
		Operand(TURTLE_OPERATION top) { this->top = top; }
		Operand(SymbolTable::Entry* var) { this->var = var; }
		Operand(Label* label) { this->label = label; }
		Operand(char offset) { this->offset = offset; }
	};

	class Element
	{
	protected:
		Element() { address = -1; }
		int address;			// final address
	public:
		virtual bool IsInstruction() { return false; }
		virtual bool IsLabel() { return false; }

		int& Address() { return address; }
		void ResetAddress() { address = -1; }
	};

	class Instruction : public Element
	{
	public:
		OPCODE opcode;
		Operand operand1;
		Operand operand2;

		Instruction(OPCODE oc) : opcode(oc), operand1(0), operand2(0) { }
		Instruction(OPCODE oc, Operand o1) : opcode(oc), operand1(o1), operand2(0) { }
		Instruction(OPCODE oc, Operand o1, Operand o2) : opcode(oc), operand1(o1), operand2(o2) { }

		virtual bool IsInstruction() { return true; }
	};

	class Label : public Element
	{
	public:
		struct LabelUse
		{
			bool relative;
			int at;
			LabelUse(bool relative, int at) { this->relative = relative; this->at = at; }
		};

	private:
		bool placed;
		std::list<LabelUse> uses;

	public:
		Label() : Element() { placed = false; }

		virtual bool IsLabel() { return true; }
		bool& Placed() { return placed; }

		void AddRelativeUse(int at) { uses.push_back(LabelUse(true, at)); }
		void AddAbsoluteUse(int at) { uses.push_back(LabelUse(false, at)); }
		std::list<LabelUse>& Uses() { return uses; }

		void ResetUses() { uses.clear(); }
	};
#pragma endregion

#pragma region MemberVariables
private:
	int stackSize;
	std::list<Element*> elements;
	std::list<Label*> labels;

	unsigned char* program;
#pragma endregion

public:
	// constants
	static const int MAX_PROGRAM = 0x10000;
	static const int DEFAULT_STACK_SIZE = 0x01000;

	TurtleProgram(int stackSize = DEFAULT_STACK_SIZE);
	~TurtleProgram();

	int& StackSize() { return stackSize; }
	std::list<Element*>& Elements() { return elements; }
	std::list<Label*>& Labels() { return labels; }

	void CommitBinary();

	int GetSize();
	void PrintProgram(FILE* f);
	void WriteBinary(FILE* f);

	// insert label
	Label* LABEL();
	void PlaceLabel(Label* label);

	// insert instructions
	void LOAD_R(REGISTER reg, int val);
	void LOAD_V(SymbolTable::Entry* var, int val);
	void LOAD_O(char offset, int val);
	void MOVE_RR(REGISTER reg1, REGISTER reg2);
	void MOVE_RV(REGISTER reg, SymbolTable::Entry* var);
	void MOVE_VR(SymbolTable::Entry* var, REGISTER reg);
	void MOVE_VV(SymbolTable::Entry* var1, SymbolTable::Entry* var2);
	void MOVE_RO(REGISTER reg, char offset);
	void MOVE_OR(char offset, REGISTER reg);
	void JMPRe(Label* label);
	void JMPTo(Label* label);
	void JEq(Label* label);
	void JNe(Label* label);
	void JGt(Label* label);
	void JLt(Label* label);
	void JZe(Label* label);
	void JNz(Label* label);
	void CMP_RR(REGISTER reg1, REGISTER reg2);
	void CMP_RV(REGISTER reg, SymbolTable::Entry* var);
	void CMP_VR(SymbolTable::Entry* var, REGISTER reg);
	void CMP_VV(SymbolTable::Entry* var1, SymbolTable::Entry* var2);
	void INC_R(REGISTER reg);
	void DEC_R(REGISTER reg);
	void ADD_R(REGISTER reg1, REGISTER reg2);
	void ADD_V(REGISTER reg, SymbolTable::Entry* var);
	void ADD_O(REGISTER reg, char offset);
	void SUB_R(REGISTER reg1, REGISTER reg2);
	void SUB_V(REGISTER reg, SymbolTable::Entry* var);
	void SUB_O(REGISTER reg, char offset);
	void MUL_R(REGISTER reg1, REGISTER reg2);
	void MUL_V(REGISTER reg, SymbolTable::Entry* var);
	void MUL_O(REGISTER reg, char offset);
	void DIV_R(REGISTER reg1, REGISTER reg2);
	void DIV_V(REGISTER reg, SymbolTable::Entry* var);
	void DIV_O(REGISTER reg, char offset);
	void PUSH_R(REGISTER reg);
	void POP_R(REGISTER reg);
	void PEEK_R(REGISTER reg);
	void TURTLE(TURTLE_OPERATION op);
	void CALL(Label* label);
	void ENTER(char offset);
	void LEAVE();
	void RET(char offset);
	void EXIT();

private:
	void ResetBinary();
	int GenerateInstructions(int pc);
	int GenerateStaticVariables(int pc);

	int PrintOpcode(FILE* f, int i);
	char* RegisterName(REGISTER r);
	char* TurtleOperationString(TURTLE_OPERATION t);

public:
	// generation helpers
	static int makeint(unsigned char hi, unsigned char lo) { return (short)((((int)hi << 8) & 0xff00) | (lo & 0x00ff)); }
	static unsigned char hibyte(int x) { return (unsigned char)((x >> 8) & 0x00ff); }
	static unsigned char lobyte(int x) { return (unsigned char)(x & 0x00ff); }
};


#endif
