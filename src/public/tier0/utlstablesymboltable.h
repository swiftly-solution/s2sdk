#ifndef UTLSTABLESYMBOLTABLE_H
#define UTLSTABLESYMBOLTABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "generichash.h"
#include "utlvector.h"
#include "mempool.h"
#include "strtools.h"
#include "utlhashtable.h"
#include "utlstringtoken.h"

// Strings are case insensitive and get consecutive ids from 0
class CStableSymbolTable
{
public:
	CStableSymbolTable();

	// Returns the string's id, adding it with the next id when it's new, or -1 for nullptr
	DLL_CLASS_IMPORT int GetId( const char *pString, bool *pbAdded = nullptr );
	// Adds the string with the given id, unless the id has a string already
	DLL_CLASS_IMPORT void Insert( int nId, const char *pString );

	// Returns nullptr for an id without a string
	const char *GetSymbol( int nId ) const;
	// The number of ids, including the ones Insert skipped
	int Count() const;

protected:
	struct Hash_t
	{
		uint32 operator()( const char *pString ) const;
	};

	struct Eq_t
	{
		bool operator()( const char *pString1, const char *pString2 ) const;
	};

	CUtlHashtable< const char *, int, Hash_t, Eq_t > m_SymbolToId;
	CUtlVector< const char * > m_IdToSymbol;
	CUtlMemoryBlockAllocator< byte > m_stringStorage;
};

#endif // UTLSTABLESYMBOLTABLE_H
