#ifndef UTLSYMBOLTABLELARGE_H
#define UTLSYMBOLTABLELARGE_H

#ifdef _WIN32
#pragma once
#endif

#include "threadtools.h"
#include "utlvector.h"
#include "utlhashtable.h"
#include "mempool.h"
#include "utlsymbollarge.h"

typedef uint32 LargeSymbolTableHashDecoration_t; 

// The structure consists of the hash immediately followed by the string data
struct CUtlSymbolTableLargeBaseTreeEntry_t
{
	LargeSymbolTableHashDecoration_t	m_Hash;
	// Variable length string data
	char								m_String[1];

	bool IsEmpty() const;

	char const *String() const;

	CUtlSymbolLarge ToSymbol() const;
	
	LargeSymbolTableHashDecoration_t HashValue() const;
};

// Base Class for threaded and non-threaded types
template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
class CUtlSymbolTableLargeBase
{
public:
	// constructor, destructor
	CUtlSymbolTableLargeBase( int nGrowSize = 0, int nInitSize = 16 )
		:	m_HashTable( 0 ), 
			m_MemBlocks( nGrowSize, nInitSize ), 
			m_Mutex(), 
			m_MemBlockAllocator( ( nInitSize > 0 ) ? 8 : 0, PAGE_SIZE ), 
			m_nElementLimit( INT_MAX - 1 ), 
			m_bThrowError( true ),
			m_nBytesAccumulated( 0 ) { }

	~CUtlSymbolTableLargeBase() { }

	// Finds and/or creates a symbol based on the string
	CUtlSymbolLarge AddString( const char* pString, bool* created = NULL );
	CUtlSymbolLarge AddString( const char* pString, int nLength, bool* created = NULL );
	UtlSymLargeId_t AddStringRaw( const char* pString, bool* created = NULL );
	UtlSymLargeId_t AddStringRaw( const char* pString, int nLength, bool* created = NULL );

	// Finds the symbol for pString
	CUtlSymbolLarge Find( const char* pString ) const;
	CUtlSymbolLarge Find( const char* pString, int nLength ) const;
	UtlSymLargeId_t FindRaw( const char* pString ) const;
	UtlSymLargeId_t FindRaw( const char* pString, int nLength ) const;

	// Remove all symbols in the table.
	void RemoveAll();
	void Purge();
	
	const char*		String( UtlSymLargeId_t id ) const;
	unsigned int	HashValue( UtlSymLargeId_t id ) const;

private:
	UtlSymLargeId_t AddString( unsigned int hash, const char* pString, int nLength, bool* created = NULL );
	UtlSymLargeId_t Find( unsigned int hash, const char* pString, int nLength ) const;

	struct UtlSymTableLargeAltKey
	{
		const CUtlSymbolTableLargeBase*	m_pTable;
		const char *m_pString;
		int m_nLength;
	};

	struct UtlSymTableLargeIdKey
	{
		explicit operator UtlSymLargeId_t() const { return m_SymId; }

		const CUtlSymbolTableLargeBase*	m_pTable;
		UtlSymLargeId_t m_SymId;
	};

	struct UtlSymTableLargeHashFunctor
	{
		unsigned int operator()( UtlSymTableLargeAltKey k ) const
		{
			return CUtlSymbolLarge_Hash( CASEINSENSITIVE, k.m_pString, k.m_nLength );
		}

		unsigned int operator()( const UtlSymTableLargeIdKey &k ) const
		{
			const char *pString = k.m_pTable->String( k.m_SymId );
			return CUtlSymbolLarge_Hash( CASEINSENSITIVE, pString, V_strlen( pString ) );
		}
	};

	struct UtlSymTableLargeEqualFunctor
	{
		bool operator()( UtlSymTableLargeAltKey a, UtlSymLargeId_t b ) const 
		{ 
			const char* pString = a.m_pTable->String( b );
			int nLength = (int)strlen( pString );

			if ( a.m_nLength != nLength )
				return false;

			if ( !CASEINSENSITIVE ) 
				return strncmp( a.m_pString, pString, a.m_nLength ) == 0; 
			else
				return _V_strnicmp_fast( a.m_pString, pString, a.m_nLength ) == 0; 
		}

		bool operator()( UtlSymLargeId_t a, UtlSymTableLargeAltKey b ) const 
		{ 
			return operator()( b, a );
		}

		bool operator()( UtlSymLargeId_t a, const UtlSymTableLargeIdKey &b ) const
		{
			if ( a == b.m_SymId )
				return true;

			const char *pString = b.m_pTable->String( a );
			const char *pOther = b.m_pTable->String( b.m_SymId );

			if ( !CASEINSENSITIVE )
				return V_strcmp( pString, pOther ) == 0;
			else
				return V_stricmp( pString, pOther ) == 0;
		}
	};

	typedef CUtlHashtable<UtlSymLargeId_t, empty_t, UtlSymTableLargeHashFunctor, UtlSymTableLargeEqualFunctor> Hashtable_t;
	typedef CUtlLeanVector< MemBlockHandle_t > MemBlocksVec_t;

	Hashtable_t						m_HashTable;
	MemBlocksVec_t					m_MemBlocks;
	MUTEX_TYPE						m_Mutex;
	CUtlMemoryBlockAllocator<byte>	m_MemBlockAllocator;
	int								m_nElementLimit;
	bool							m_bThrowError;
	int								m_nBytesAccumulated;
};

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Find( unsigned int hash, const char* pString, int nLength ) const
{
	UtlSymTableLargeAltKey key;
	
	key.m_pTable = this;
	key.m_pString = pString;
	key.m_nLength = nLength;

	UtlHashHandle_t h = m_HashTable.Find( key, hash );

	if ( h == m_HashTable.InvalidHandle() )
		return UTL_INVAL_SYMBOL_LARGE;

	return m_HashTable[h];
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::AddString( unsigned int hash, const char* pString, int nLength, bool* created )
{	
	if ( m_MemBlocks.Count() >= m_nElementLimit )
	{
		if ( m_bThrowError )
		{
			Plat_FatalError( "FATAL ERROR: CUtlSymbolTableLarge element limit of %u exceeded\n", m_nElementLimit );
			DebuggerBreak();
		}

		Warning( "ERROR: CUtlSymbolTableLarge element limit of %u exceeded\n", m_nElementLimit );

		return UTL_INVAL_SYMBOL_LARGE;
	}

	if ( created )
		*created = true;

	MemBlockHandle_t block = m_MemBlockAllocator.Alloc( nLength + sizeof( LargeSymbolTableHashDecoration_t ) + 1 );

	byte *pEntry = (byte *)m_MemBlockAllocator.GetBlock( block );

	V_memcpy( pEntry, &hash, sizeof( LargeSymbolTableHashDecoration_t ) );
	char *pText = (char *)( pEntry + sizeof( LargeSymbolTableHashDecoration_t ) );
	V_memmove( pText, pString, nLength );
	pText[ nLength ] = '\0';

	m_nBytesAccumulated += nLength + 1;

	UtlSymLargeId_t id = m_MemBlocks.AddToTail( block + sizeof( LargeSymbolTableHashDecoration_t ) );

	UtlSymTableLargeIdKey key;
	key.m_pTable = this;
	key.m_SymId = id;

	empty_t empty;
	m_HashTable.Insert( key, empty, hash );

	return id;
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline const char* CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::String( UtlSymLargeId_t id ) const
{
	if(id == UTL_INVAL_SYMBOL_LARGE || id >= (UtlSymLargeId_t)m_MemBlocks.Count())
		return nullptr;

	return ( const char* )m_MemBlockAllocator.GetBlock( m_MemBlocks[ id ] );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline unsigned int CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::HashValue( UtlSymLargeId_t id ) const
{
	if(id == UTL_INVAL_SYMBOL_LARGE || id >= (UtlSymLargeId_t)m_MemBlocks.Count())
		return 0;

	LargeSymbolTableHashDecoration_t hash;
	V_memcpy( &hash, m_MemBlockAllocator.GetBlock( m_MemBlocks[ id ] - sizeof( LargeSymbolTableHashDecoration_t ) ), sizeof( LargeSymbolTableHashDecoration_t ) );

	return hash;
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline CUtlSymbolLarge CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Find( const char *pString, int nLength ) const
{
	UtlSymLargeId_t sym = FindRaw( pString, nLength );

	if(sym == UTL_INVAL_SYMBOL_LARGE)
		return CUtlSymbolLarge();

	return String( sym );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline CUtlSymbolLarge CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Find( const char *pString ) const
{
	return Find( pString, pString ? (int)strlen( pString ) : 0 );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::FindRaw( const char* pString, int nLength ) const
{
	if ( pString && nLength > 0 && *pString )
	{
		unsigned int hash = CUtlSymbolLarge_Hash( CASEINSENSITIVE, pString, nLength );

		AUTO_LOCK( m_Mutex );

		return Find( hash, pString, nLength );
	}

	return UTL_INVAL_SYMBOL_LARGE;
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::FindRaw( const char* pString ) const
{	
	return FindRaw( pString, pString ? (int)strlen( pString ) : 0 );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline CUtlSymbolLarge CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::AddString( const char *pString, int nLength, bool *created )
{
	UtlSymLargeId_t sym = AddStringRaw( pString, nLength, created );

	if(sym == UTL_INVAL_SYMBOL_LARGE)
		return CUtlSymbolLarge();

	return String( sym );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline CUtlSymbolLarge CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::AddString( const char *pString, bool *created )
{
	return AddString( pString, pString ? (int)strlen( pString ) : 0, created );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::AddStringRaw( const char* pString, int nLength, bool* created )
{	
	if ( created )
		*created = false;

	UtlSymLargeId_t sym = UTL_INVAL_SYMBOL_LARGE;

	if ( pString && nLength > 0 && *pString )
	{
		unsigned int hash = CUtlSymbolLarge_Hash( CASEINSENSITIVE, pString, nLength );

		AUTO_LOCK( m_Mutex );

		sym = Find( hash, pString, nLength );

		if ( sym == UTL_INVAL_SYMBOL_LARGE )
			sym = AddString( hash, pString, nLength, created );
	}

	return sym;
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline UtlSymLargeId_t CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::AddStringRaw( const char* pString, bool* created )
{	
	return AddStringRaw( pString, pString ? (int)strlen( pString ) : 0, created );
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline void CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::RemoveAll()
{	
	AUTO_LOCK( m_Mutex );

	m_HashTable.RemoveAll();
	m_MemBlocks.RemoveAll();
	m_MemBlockAllocator.RemoveAll();
}

template < bool CASEINSENSITIVE, size_t PAGE_SIZE, class MUTEX_TYPE >
inline void CUtlSymbolTableLargeBase< CASEINSENSITIVE, PAGE_SIZE, MUTEX_TYPE >::Purge()
{	
	AUTO_LOCK( m_Mutex );

	m_HashTable.Purge();
	m_MemBlocks.Purge();
	m_MemBlockAllocator.Purge();
}

// Case-sensitive
typedef CUtlSymbolTableLargeBase< false, 2048, CThreadNullMutex > CUtlSymbolTableLarge;
// Case-insensitive
typedef CUtlSymbolTableLargeBase< true, 2048, CThreadNullMutex > CUtlSymbolTableLarge_CI;
// Multi-threaded case-sensitive
typedef CUtlSymbolTableLargeBase< false, 2048, CAtomicMutex > CUtlSymbolTableLargeMT;
// Multi-threaded case-insensitive
typedef CUtlSymbolTableLargeBase< true, 2048, CAtomicMutex > CUtlSymbolTableLargeMT_CI;

#endif // UTLSYMBOLTABLELARGE_H
