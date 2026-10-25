#ifndef UTLSYMBOLTABLE_H
#define UTLSYMBOLTABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "threadtools.h"
#include "utlsymbol.h"
#include "utlbuffer.h"
#include "utlvector.h"
#include "utlhashtable.h"

class CUtlSymbolTable
{
public:
	// constructor, destructor
	DLL_CLASS_IMPORT CUtlSymbolTable( int growSize = 0, int initSize = 16, bool caseInsensitive = false );
	DLL_CLASS_IMPORT ~CUtlSymbolTable();
	
	// Finds and/or creates a symbol based on the string
	DLL_CLASS_IMPORT CUtlSymbol AddString( const char* pString, bool* created = NULL );
	DLL_CLASS_IMPORT CUtlSymbol AddString( const char* pString, int nLength, bool* created = NULL );

	// Finds the symbol for pString
	DLL_CLASS_IMPORT CUtlSymbol Find( const char* pString ) const;
	DLL_CLASS_IMPORT CUtlSymbol Find( const char* pString, int nLength ) const;
	
	// Look up the string associated with a particular symbol
	DLL_CLASS_IMPORT const char* String( CUtlSymbol id ) const;
	
	// Remove all symbols in the table.
	DLL_CLASS_IMPORT void RemoveAll();
	DLL_CLASS_IMPORT void Purge();

	// Returns elements in the table
	DLL_CLASS_IMPORT int GetElements( int nFirstElement, int nCount, CUtlSymbol *pElements ) const;

	DLL_CLASS_IMPORT size_t AllocSize() const;

	DLL_CLASS_IMPORT void SetPageSize( unsigned int nSize );

	DLL_CLASS_IMPORT bool SaveToBuffer( CUtlBuffer& buff ) const;
	DLL_CLASS_IMPORT bool RestoreFromBuffer( CUtlBuffer& buff );

	int GetNumStrings( void ) const
	{
		return m_MemBlocks.Count();
	}

protected:
	struct UtlSymTableAltKey
	{ 
		const CUtlSymbolTable*	m_pTable;
		const char*				m_pString;
		int						m_nLength;
	};

	struct UtlSymTableHashFunctor
	{
		UtlSymTableHashFunctor();
		unsigned int operator()( UtlSymTableAltKey k ) const;
		unsigned int operator()( int k ) const;
	};

	struct UtlSymTableEqualFunctor
	{
		UtlSymTableEqualFunctor();
		bool operator()( int a, int b ) const;
		bool operator()( UtlSymTableAltKey a, int b ) const;
		bool operator()( int a, UtlSymTableAltKey b ) const;
	};

	typedef CUtlHashtable<int, empty_t, UtlSymTableHashFunctor, UtlSymTableEqualFunctor> Hashtable_t;
	typedef CUtlVector<MemBlockHandle_t> MemBlocksVec_t;

	Hashtable_t						m_HashTable;
	MemBlocksVec_t					m_MemBlocks;
	CUtlMemoryBlockAllocator<uint8_t>	m_MemBlockAllocator;

	bool m_bInsensitive;
};

class CUtlSymbolTableMT :  public CUtlSymbolTable
{
public:
	CUtlSymbolTableMT( int growSize = 0, int initSize = 32, bool caseInsensitive = false )
		: CUtlSymbolTable( growSize, initSize, caseInsensitive )
	{
	}

	CUtlSymbol AddString( const char* pString, bool* created = NULL )
	{
		m_lock.LockForWrite( __FILE__, __LINE__ );
		CUtlSymbol result = CUtlSymbolTable::AddString( pString, created );
		m_lock.UnlockWrite( __FILE__, __LINE__ );
		return result;
	}

	CUtlSymbol AddString( const char* pString, int nLength, bool* created = NULL )
	{
		m_lock.LockForWrite( __FILE__, __LINE__ );
		CUtlSymbol result = CUtlSymbolTable::AddString( pString, nLength, created );
		m_lock.UnlockWrite( __FILE__, __LINE__ );
		return result;
	}

	CUtlSymbol Find( const char* pString ) const
	{
		m_lock.LockForWrite( __FILE__, __LINE__ );
		CUtlSymbol result = CUtlSymbolTable::Find( pString );
		m_lock.UnlockWrite( __FILE__, __LINE__ );
		return result;
	}

	CUtlSymbol Find( const char* pString, int nLength ) const
	{
		m_lock.LockForWrite( __FILE__, __LINE__ );
		CUtlSymbol result = CUtlSymbolTable::Find( pString, nLength );
		m_lock.UnlockWrite( __FILE__, __LINE__ );
		return result;
	}

	const char* String( CUtlSymbol id ) const
	{
		m_lock.LockForRead( __FILE__, __LINE__ );
		const char *pszResult = CUtlSymbolTable::String( id );
		m_lock.UnlockRead( __FILE__, __LINE__ );
		return pszResult;
	}
	
private:
	mutable CThreadSpinRWLock m_lock;
};

#endif