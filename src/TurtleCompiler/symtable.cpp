// symtable.cpp
// pete myers
// OIT Fall 2008
//
// C++ Symbol Table Jan 2017
// Singleton
//
// Scope added Jan 2020
//

#include "symtable.h"

// The one instance
SymbolTable * SymbolTable::theInstance = NULL;

SymbolTable * SymbolTable::GetInstance()
{
	if (theInstance == NULL)
	{
		theInstance = new SymbolTable();
	}

	return theInstance;
}

SymbolTable::Entry * SymbolTable::Insert(const std::string & lexeme, int token)
{
	Entry * entry = Lookup(lexeme);
	if (entry == NULL)
	{
		entry = new Entry(lexeme, token);
		entry->Scope() = currentScope;
		entriesByScope[currentScope][lexeme] = entry;
	}

	return entry;
}

SymbolTable::Entry * SymbolTable::Lookup(const std::string & lexeme)
{
	// look first in the current local scope
	if (currentScope > 0)
	{
		std::map<std::string, Entry*>::iterator iter = entriesByScope[currentScope].find(lexeme);
		if (iter != entriesByScope[currentScope].end())
			return iter->second;
	}

	// now look in the global scope
	std::map<std::string, Entry*>::iterator iter = entriesByScope[0].find(lexeme);
	if (iter != entriesByScope[0].end())
		return iter->second;

	return NULL;
}

void SymbolTable::NewLocalScope()
{
	// create a new scope and add it to the collection, make it current
	currentScope = nextScope++;
	entriesByScope.push_back(std::map<std::string, Entry*>());
}

void SymbolTable::ExitLocalScope()
{
	// back to global scope
	currentScope = 0;
}

int SymbolTable::CurrentScope()
{
	return currentScope;
}

int SymbolTable::LastCreatedScope()
{
	return nextScope - 1;
}

std::list<SymbolTable::Entry*> SymbolTable::GetGlobalEntries()
{
	std::list<Entry*> result;
	std::map<std::string, Entry*>::iterator iter;
	for (iter = entriesByScope[0].begin(); iter != entriesByScope[0].end(); iter++)
	{
		result.push_back(iter->second);
	}
	return result;
}

std::list<SymbolTable::Entry*> SymbolTable::GetLocalEntriesForScope(int scope)
{
	std::list<Entry*> result;
	std::map<std::string, Entry*>::iterator iter;
	for (iter = entriesByScope[scope].begin(); iter != entriesByScope[scope].end(); iter++)
	{
		if (iter->second->Storage() == ST_LOCAL)
			result.push_back(iter->second);
	}
	return result;
}


SymbolTable::SymbolTable()
{
	currentScope = 0;	// global scope
	nextScope = 1;		// next local scope to be created

	// create the global scope entries at index 0
	entriesByScope.push_back(std::map<std::string, Entry*>());
}

SymbolTable::~SymbolTable()
{
	std::vector<std::map<std::string, Entry*>>::iterator liter;
	for (liter = entriesByScope.begin(); liter != entriesByScope.end(); liter++)
	{
		std::map<std::string, Entry*>::iterator iter;
		for (iter = liter->begin(); iter != liter->end(); iter++)
		{
			if (iter->second != NULL)
			{
				delete iter->second;
				iter->second = NULL;
			}
		}
	}
}

SymbolTable::Entry::Entry(const std::string & lexeme, int token)
{
	this->lexeme = lexeme;
	this->token = token;
	this->type = IT_UNDEFINED;
	this->storage = ST_UNDEFINED;
	this->address = 0;
	this->scope = -1;
	this->params = 0;
}

SymbolTable::Entry::~Entry()
{
}
