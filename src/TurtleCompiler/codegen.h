// codegen.h
// pete myers
// OIT Fall 2008, Winter 2020

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "TurtleConstants.h"
#include "tree.h"
#include "TurtleProgram.h"



//
// C++ Version Jan 2017
//



// sub-classes of TreeNode classes that implement generation

class GenBlockTreeNode : public BlockTreeNode
{
public:
	GenBlockTreeNode() { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenNumberTreeNode : public NumberTreeNode
{
public:
	GenNumberTreeNode(int value) : NumberTreeNode(value) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenColorNameTreeNode : public ColorNameTreeNode
{
public:
	GenColorNameTreeNode(COLOR_TYPE color) : ColorNameTreeNode(color) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenFunctionTreeNode : public FunctionTreeNode
{
public:
	GenFunctionTreeNode(FUNCTION_TYPE func) : FunctionTreeNode(func) { }
	GenFunctionTreeNode(FUNCTION_TYPE func, TreeNode* param) : FunctionTreeNode(func, param) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenTurtleCmdTreeNode : public TurtleCmdTreeNode
{
public:
	GenTurtleCmdTreeNode(TURTLE_CMD cmd) : TurtleCmdTreeNode(cmd) { }
	GenTurtleCmdTreeNode(TURTLE_CMD cmd, TreeNode* param) : TurtleCmdTreeNode(cmd, param) { }
	GenTurtleCmdTreeNode(TURTLE_CMD cmd, TreeNode* param1, TreeNode* param2) : TurtleCmdTreeNode(cmd, param1, param2) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenIfTreeNode : public IfTreeNode
{
public:
	GenIfTreeNode(TreeNode* condition, BlockTreeNode* block) : IfTreeNode(condition, block) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenIfElseTreeNode : public IfElseTreeNode
{
public:
	GenIfElseTreeNode(TreeNode* condition, BlockTreeNode* trueBlock, BlockTreeNode* falseBlock) : IfElseTreeNode(condition, trueBlock, falseBlock) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenOperatorTreeNode : public OperatorTreeNode
{
public:
	GenOperatorTreeNode(OPERATOR_TYPE op, TreeNode* lhs, TreeNode* rhs) : OperatorTreeNode(op, lhs, rhs) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenRepeatTreeNode : public RepeatTreeNode
{
public:
	GenRepeatTreeNode(TreeNode* loopFor, BlockTreeNode* block) : RepeatTreeNode(loopFor, block) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenVariableTreeNode : public VariableTreeNode
{
public:
	GenVariableTreeNode(SymbolTable::Entry* symentry) : VariableTreeNode(symentry) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenDeclarationTreeNode : public DeclarationTreeNode
{
public:
	GenDeclarationTreeNode(IDENTIFIER_TYPE vartype, VariableTreeNode* variable) : DeclarationTreeNode(vartype, variable) { }
	virtual void GenerateNode(TurtleProgram* program);
	void SetOffset(int offset) { ((GenVariableTreeNode*)(FirstChild()))->SymEntry()->Address() = offset; }
};

class GenAssignmentTreeNode : public AssignmentTreeNode
{
public:
	GenAssignmentTreeNode(VariableTreeNode* lhs, TreeNode* rhs) : AssignmentTreeNode(lhs, rhs) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenProcDefTreeNode : public ProcDefTreeNode
{
private:
	TurtleProgram::Label* returnLabel;
public:
	GenProcDefTreeNode(SymbolTable::Entry* symentry, int localScope, ParamsTreeNode* params, BlockTreeNode* block) : ProcDefTreeNode(symentry, localScope, params, block), returnLabel(NULL) { }
	virtual void GenerateNode(TurtleProgram* program);
	void SetCallLabel(TurtleProgram::Label* label) { SymEntry()->Address() = (int)label; }
	void SetReturnLabel(TurtleProgram::Label* label) { returnLabel = label; }
	TurtleProgram::Label* GetReturnLabel() { return returnLabel; }
};

class GenParamsTreeNode : public ParamsTreeNode
{
public:
	GenParamsTreeNode() : ParamsTreeNode() { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenReturnTreeNode : public ReturnTreeNode
{
public:
	GenReturnTreeNode() : ReturnTreeNode() { }
	GenReturnTreeNode(TreeNode* returnValue) : ReturnTreeNode(returnValue) { }
	virtual void GenerateNode(TurtleProgram* program);
};

class GenProcCallTreeNode : public ProcCallTreeNode
{
public:
	GenProcCallTreeNode(SymbolTable::Entry* symentry, ArgsTreeNode* args, bool expectReturnValue) : ProcCallTreeNode(symentry, args, expectReturnValue) { }
	virtual void GenerateNode(TurtleProgram* program);
	TurtleProgram::Label* Label() { return (TurtleProgram::Label*)(SymEntry()->Address()); }
};

class GenArgsTreeNode : public ArgsTreeNode
{
public:
	GenArgsTreeNode() : ArgsTreeNode() { }
	virtual void GenerateNode(TurtleProgram* program);
};


// factory
class GenTreeNodeFactory : public TreeNodeFactory
{
public:
	virtual BlockTreeNode* CreateBlock() { return new GenBlockTreeNode(); }
	virtual NumberTreeNode* CreateNumber(int value) { return new GenNumberTreeNode(value); }
	virtual ColorNameTreeNode* CreateColorName(COLOR_TYPE color) { return new GenColorNameTreeNode(color); }
	virtual FunctionTreeNode* CreateFunction(FUNCTION_TYPE func) { return new GenFunctionTreeNode(func); }
	virtual FunctionTreeNode* CreateFunction(FUNCTION_TYPE func, TreeNode* param) { return new GenFunctionTreeNode(func, param); }
	virtual TurtleCmdTreeNode* CreateTurtleCmd(TURTLE_CMD cmd) { return new GenTurtleCmdTreeNode(cmd); }
	virtual TurtleCmdTreeNode* CreateTurtleCmd(TURTLE_CMD cmd, TreeNode* param) { return new GenTurtleCmdTreeNode(cmd, param); }
	virtual TurtleCmdTreeNode* CreateTurtleCmd(TURTLE_CMD cmd, TreeNode* param1, TreeNode* param2) { return new GenTurtleCmdTreeNode(cmd, param1, param2); }
	virtual IfTreeNode* CreateIf(TreeNode* condition, BlockTreeNode* block) { return new GenIfTreeNode(condition, block); }
	virtual IfElseTreeNode* CreateIfElse(TreeNode* condition, BlockTreeNode* trueblock, BlockTreeNode* falseblock) { return new GenIfElseTreeNode(condition, trueblock, falseblock); }
	virtual OperatorTreeNode* CreateOperator(OPERATOR_TYPE op, TreeNode* lhs, TreeNode* rhs) { return new GenOperatorTreeNode(op, lhs, rhs); }
	virtual RepeatTreeNode* CreateRepeat(TreeNode* loopFor, BlockTreeNode* block) { return new GenRepeatTreeNode(loopFor, block); }
	virtual DeclarationTreeNode* CreateDeclaration(IDENTIFIER_TYPE vartype, VariableTreeNode* variable) { return new GenDeclarationTreeNode(vartype, variable); }
	virtual VariableTreeNode* CreateVariable(SymbolTable::Entry* symentry) { return new GenVariableTreeNode(symentry); }
	virtual AssignmentTreeNode* CreateAssignment(VariableTreeNode* lhs, TreeNode* rhs) { return new GenAssignmentTreeNode(lhs, rhs); }
	virtual ProcDefTreeNode* CreateProcDef(SymbolTable::Entry* symentry, int localScope, ParamsTreeNode* params, BlockTreeNode* block) { return new GenProcDefTreeNode(symentry, localScope, params, block); }
	virtual ParamsTreeNode* CreateParams() { return new GenParamsTreeNode(); }
	virtual ReturnTreeNode* CreateReturn() { return new GenReturnTreeNode(); }
	virtual ReturnTreeNode* CreateReturn(TreeNode* returnValue) { return new GenReturnTreeNode(returnValue); }
	virtual ProcCallTreeNode* CreateProcCall(SymbolTable::Entry* symentry, ArgsTreeNode* args) { return new GenProcCallTreeNode(symentry, args, false); }
	virtual ProcCallTreeNode* CreateProcCallWithReturnValue(SymbolTable::Entry* symentry, ArgsTreeNode* args) { return new GenProcCallTreeNode(symentry, args, true); }
	virtual ArgsTreeNode* CreateArgs() { return new GenArgsTreeNode(); }
};


#endif

