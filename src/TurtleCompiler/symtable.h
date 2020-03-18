// symtable.h
// pete myers
// OIT Fall 2008
//
// C++ Symbol Table Jan 2017
// Singleton
//
// Scope added Jan 2020
//

#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <string>
#include <map>
#include <list>
#include <vector>

//
// type constants used for representing types of identifiers and storage locations
//

typedef enum
{
	IT_UNDEFINED = -1,
	IT_VARIABLE = 0,
	IT_LABEL = 1,
	IT_PROC = 2
} IDENTIFIER_TYPE;

typedef enum
{
	ST_UNDEFINED = -1,
	ST_GLOBAL = 0,
	ST_LOCAL = 1,
	ST_PARAM = 2
} STORAGE_TYPE;


class SymbolTable
{
public:
	class Entry
	{
	private:
		std::string lexeme;
		int token;
		IDENTIFIER_TYPE type;
		STORAGE_TYPE storage;
		int address;
		int scope;
		int params;

	public:
		Entry(const std::string & lexeme, int token);
		~Entry();

		std::string & Lexeme() { return lexeme; }
		int & Token() { return token; }
		bool IsDefined() { return type != IT_UNDEFINED; }
		IDENTIFIER_TYPE & Type() { return type; }
		STORAGE_TYPE & Storage() { return storage; }
		int & Address() { return address; }
		int & Scope() { return scope; }
		int & Params() { return params; }
	};

	static SymbolTable * GetInstance();

	Entry * Insert(const std::string & lexeme, int token);
	Entry * Lookup(const std::string & lexeme);

	void NewLocalScope();
	void ExitLocalScope();
	int CurrentScope();
	int LastCreatedScope();

	std::list<Entry*> GetGlobalEntries();
	std::list<Entry*> GetLocalEntriesForScope(int scope);

private:
	SymbolTable();
	~SymbolTable();
	static SymbolTable * theInstance;
	std::vector<std::map<std::string, Entry*>> entriesByScope;
	int nextScope;
	int currentScope;
};

#endif

