//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: index-based hash map container well suited for large and growing
//	datasets. It uses less memory than other hash maps and incrementally
//	rehashes to reduce reallocation spikes.
//
//=============================================================================//

#ifndef UTLHASHMAPLARGE_H
#define UTLHASHMAPLARGE_H

#ifdef _WIN32
#pragma once
#endif

#include "dbg.h"
#include "utlcommon.h"
#include "utlleanvector.h"
#include "murmurhash3.h"
#include "utlstring.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>

#include "memdbgon.h"

// Case sensitive hash and compare for const char * keys
struct MurmurHash3ConstCharPtr
{
	uint32 operator()( const char *pszKey ) const;
};

struct CaseSensitiveStrEquals
{
	bool operator()( const char *pszLhs, const char *pszRhs ) const;
};

template <typename T>
class CUtlHashMapLargeDefEquals
{
public:
	bool operator()( const T &lhs, const T &rhs ) const { return lhs == rhs; }
};

//-----------------------------------------------------------------------------
//
// Purpose:	An associative container. Pretty much identical to CUtlMap without the ability to walk in-order
//	This container is well suited for large and growing datasets. It uses less
//	memory than other hash maps and incrementally rehashes to reduce reallocation spikes.
//
// Iterate it with FOR_EACH_MAP_FAST or First() and Next().
//
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L = CUtlHashMapLargeDefEquals<K>, typename H = MurmurHash3Functor<K>>
class CUtlHashMapLarge
{
public:
	// This enum exists so that FOR_EACH_MAP_FAST can be used on this container
	enum CompileTimeCheck
	{
		IsUtlMap = 1
	};

	using KeyType_t = K;
	using ElemType_t = T;
	using IndexType_t = int;
	using EqualityFunc_t = L;
	using HashFunc_t = H;
	static constexpr IndexType_t kInvalidIndex = -1;

	CUtlHashMapLarge()
	{
		m_iNodeFreeListHead = kInvalidIndex;
		m_cElements = 0;
		m_nMinRehashedBucket = kInvalidIndex;
		m_nMaxRehashedBucket = kInvalidIndex;
	}

	CUtlHashMapLarge( int cElementsExpected ) : CUtlHashMapLarge()
	{
		EnsureCapacity( cElementsExpected );
	}

	~CUtlHashMapLarge()
	{
		RemoveAll();
	}

	CUtlHashMapLarge( const CUtlHashMapLarge & ) = delete;
	CUtlHashMapLarge &operator=( const CUtlHashMapLarge & ) = delete;

	// gets particular elements
	ElemType_t &		Element( IndexType_t i )			{ return m_memNodes.Element( i ).m_elem; }
	const ElemType_t &	Element( IndexType_t i ) const		{ return m_memNodes.Element( i ).m_elem; }
	ElemType_t &		operator[]( IndexType_t i )			{ return m_memNodes.Element( i ).m_elem; }
	const ElemType_t &	operator[]( IndexType_t i ) const	{ return m_memNodes.Element( i ).m_elem; }
	const KeyType_t &	Key( IndexType_t i ) const			{ return m_memNodes.Element( i ).m_key; }

	// Num elements
	IndexType_t Count() const								{ return m_cElements; }

	// One past the highest index ever handed out, valid or not
	IndexType_t MaxElement() const							{ return m_memNodes.Count(); }

	// Checks if a node is valid and in the map
	bool IsValidIndex( IndexType_t i ) const				{ return i >= 0 && i < m_memNodes.Count() && m_memNodes[i].m_iNextNode >= kInvalidIndex; }

	// Invalid index
	static constexpr IndexType_t InvalidIndex()				{ return kInvalidIndex; }

	// Iteration, in index order
	IndexType_t First() const								{ return NextValid( 0 ); }
	IndexType_t Next( IndexType_t i ) const					{ return NextValid( i + 1 ); }

	// Insert or replace the existing if found (no dupes)
	IndexType_t Insert( const KeyType_t &key )								{ return FindOrInsert_Internal( key, true ); }
	IndexType_t Insert( KeyType_t &&key )									{ return FindOrInsert_Internal( std::move( key ), true ); }
	IndexType_t Insert( const KeyType_t &key, const ElemType_t &insert )	{ return FindOrInsert_Internal( key, insert, true ); }
	IndexType_t Insert( const KeyType_t &key, ElemType_t &&insert )			{ return FindOrInsert_Internal( key, std::move( insert ), true ); }
	IndexType_t Insert( KeyType_t &&key, const ElemType_t &insert )			{ return FindOrInsert_Internal( std::move( key ), insert, true ); }
	IndexType_t Insert( KeyType_t &&key, ElemType_t &&insert )				{ return FindOrInsert_Internal( std::move( key ), std::move( insert ), true ); }

	// Insert or replace the existing if found (no dupes)
	IndexType_t InsertOrReplace( const KeyType_t &key, const ElemType_t &insert )	{ return FindOrInsert_Internal( key, insert, true ); }
	IndexType_t InsertOrReplace( const KeyType_t &key, ElemType_t &&insert )		{ return FindOrInsert_Internal( key, std::move( insert ), true ); }
	IndexType_t InsertOrReplace( KeyType_t &&key, const ElemType_t &insert )		{ return FindOrInsert_Internal( std::move( key ), insert, true ); }
	IndexType_t InsertOrReplace( KeyType_t &&key, ElemType_t &&insert )				{ return FindOrInsert_Internal( std::move( key ), std::move( insert ), true ); }

	// Insert ALWAYS, possibly creating a dupe
	IndexType_t InsertWithDupes( const KeyType_t &key, const ElemType_t &insert )	{ return InsertWithDupes_Internal( key, insert ); }
	IndexType_t InsertWithDupes( const KeyType_t &key, ElemType_t &&insert )		{ return InsertWithDupes_Internal( key, std::move( insert ) ); }
	IndexType_t InsertWithDupes( KeyType_t &&key, const ElemType_t &insert )		{ return InsertWithDupes_Internal( std::move( key ), insert ); }
	IndexType_t InsertWithDupes( KeyType_t &&key, ElemType_t &&insert )				{ return InsertWithDupes_Internal( std::move( key ), std::move( insert ) ); }

	// Find-or-insert, one-arg - inserts a default-constructed element
	IndexType_t FindOrInsert( const KeyType_t &key )						{ return FindOrInsert_Internal( key, false ); }
	IndexType_t FindOrInsert( KeyType_t &&key )								{ return FindOrInsert_Internal( std::move( key ), false ); }

	// Find-or-insert, two-arg - leaves an existing element untouched
	IndexType_t FindOrInsert( const KeyType_t &key, const ElemType_t &insert )	{ return FindOrInsert_Internal( key, insert, false ); }
	IndexType_t FindOrInsert( const KeyType_t &key, ElemType_t &&insert )		{ return FindOrInsert_Internal( key, std::move( insert ), false ); }
	IndexType_t FindOrInsert( KeyType_t &&key, const ElemType_t &insert )		{ return FindOrInsert_Internal( std::move( key ), insert, false ); }
	IndexType_t FindOrInsert( KeyType_t &&key, ElemType_t &&insert )			{ return FindOrInsert_Internal( std::move( key ), std::move( insert ), false ); }

	// Find key, insert with default value if not found. Returns pointer to the element
	ElemType_t *FindOrInsertGetPtr( const KeyType_t &key )	{ return &Element( FindOrInsert( key ) ); }
	ElemType_t *FindOrInsertGetPtr( KeyType_t &&key )		{ return &Element( FindOrInsert( std::move( key ) ) ); }

	// Finds an element. Returns index of element, or InvalidIndex() if not found
	IndexType_t Find( const KeyType_t &key ) const;

	// Finds an element, returns pointer to element or NULL if not found
	ElemType_t *FindGetPtr( const KeyType_t &key )
	{
		IndexType_t i = Find( key );
		return i == kInvalidIndex ? nullptr : &Element( i );
	}
	const ElemType_t *FindGetPtr( const KeyType_t &key ) const
	{
		IndexType_t i = Find( key );
		return i == kInvalidIndex ? nullptr : &Element( i );
	}

	// Returns true if the specified *key* (not the "element"!!!) can be found
	bool HasElement( const KeyType_t &key ) const { return Find( key ) != kInvalidIndex; }

	// Doesn't work if you pass a temporary to defaultValue
	const ElemType_t &FindElement( const KeyType_t &key, const ElemType_t &defaultValue ) const
	{
		IndexType_t i = Find( key );
		if ( i == kInvalidIndex )
			return defaultValue;
		return Element( i );
	}

	// Preallocates nodes and buckets for that many elements
	void EnsureCapacity( int amount );

	void RemoveAt( IndexType_t i );
	bool Remove( const KeyType_t &key )
	{
		IndexType_t iMap = Find( key );
		if ( iMap != kInvalidIndex )
		{
			RemoveAt( iMap );
			return true;
		}
		return false;
	}

	// Removes every element but keeps the memory
	void RemoveAll();

	// Removes every element and frees the memory
	void Purge();

	// call delete on each element (as a pointer) and then purge
	void PurgeAndDeleteElements()
	{
		for ( IndexType_t i = First(); i != kInvalidIndex; i = Next( i ) )
			delete Element( i );
		Purge();
	}

private:
	template <typename pf_key>
	IndexType_t FindOrInsert_Internal( pf_key &&key, bool bReplace );

	template <typename pf_key, typename pf_elem>
	IndexType_t FindOrInsert_Internal( pf_key &&key, pf_elem &&insert, bool bReplace );

	template <typename pf_key, typename pf_elem>
	IndexType_t InsertWithDupes_Internal( pf_key &&key, pf_elem &&insert );

	// Inserts and constructs the key, leaving the element unconstructed
	template <typename pf_key>
	IndexType_t InsertUnconstructed( pf_key &&key, IndexType_t *piNodeExistingIfDupe, bool bAllowDupes );

	// Free nodes chain through m_iNextNode, stored as values below kInvalidIndex
	static IndexType_t FreeNodeIDToIndex( IndexType_t i )	{ return ( 0 - i ) - 3; }
	static IndexType_t FreeNodeIndexToID( IndexType_t i )	{ return ( -3 ) - i; }

	IndexType_t NextValid( IndexType_t i ) const
	{
		for ( ; i < m_memNodes.Count(); ++i )
		{
			if ( m_memNodes[i].m_iNextNode >= kInvalidIndex )
				return i;
		}
		return kInvalidIndex;
	}

	uint32 HashKey( const KeyType_t &key ) const { return (uint32)m_HashFunc( key ); }
	int BucketMask() const { return m_vecHashBuckets.Count() - 1; }

	// Smallest bucket count worth probing for items placed before the last growth
	int MinBucketCountToProbe() const { return MAX( m_nMinRehashedBucket, 1 ); }

	IndexType_t FindInBucket( int iBucket, const KeyType_t &key ) const;
	IndexType_t AllocNode();
	void RehashNodesInBucket( int iBucket );
	void LinkNodeIntoBucket( int iBucket, IndexType_t iNewNode );
	void UnlinkNodeFromBucket( int iBucket, IndexType_t iNodeToUnlink );
	bool RemoveNodeFromBucket( int iBucket, IndexType_t iNodeToRemove );
	void IncrementalRehash();
	void DestructAll();

	struct HashBucket_t
	{
		IndexType_t m_iNode;
	};

	// One bit per bucket, kept inline while it fits in two ints
	class MigratedBits_t
	{
	public:
		MigratedBits_t() : m_numBits( 0 ), m_numInts( 0 ), m_pInts( nullptr ) {}
		~MigratedBits_t() { Free(); }

		MigratedBits_t( const MigratedBits_t & ) = delete;
		MigratedBits_t &operator=( const MigratedBits_t & ) = delete;

		bool GetBit( int bitNum ) const { return ( Base()[bitNum >> 5] & ( 1u << ( bitNum & 31 ) ) ) != 0; }
		void SetBit( int bitNum ) { Base()[bitNum >> 5] |= 1u << ( bitNum & 31 ); }

		// Frees the current bits and holds numBits cleared ones
		void Reset( int numBits )
		{
			Free();

			int numInts = ( numBits + 31 ) >> 5;
			if ( numInts > kInlineInts )
			{
				m_pInts = (uint32 *)malloc( numInts * sizeof( uint32 ) );
				memset( m_pInts, 0, numInts * sizeof( uint32 ) );
			}
			else
			{
				memset( m_inlineInts, 0, sizeof( m_inlineInts ) );
			}

			m_numBits = numBits;
			m_numInts = numInts;
		}

	private:
		static constexpr int kInlineInts = 2;

		uint32 *Base() { return m_numInts > kInlineInts ? m_pInts : m_inlineInts; }
		const uint32 *Base() const { return m_numInts > kInlineInts ? m_pInts : m_inlineInts; }

		void Free()
		{
			if ( m_numInts > kInlineInts )
				free( m_pInts );

			m_numBits = 0;
			m_numInts = 0;
		}

		int m_numBits;
		int m_numInts;
		union
		{
			uint32 m_inlineInts[kInlineInts];
			uint32 *m_pInts;
		};
	};

	// Key and element are constructed and destructed by the map, not by the node vector
	struct Node_t
	{
		typedef void TriviallyRelocatable_t;

		Node_t() {}
		~Node_t() {}

		union { KeyType_t m_key; };
		union { ElemType_t m_elem; };
		IndexType_t m_iNextNode;
	};

	CUtlLeanVector<HashBucket_t, IndexType_t> m_vecHashBuckets;

	// Buckets whose nodes were already moved to their current bucket
	MigratedBits_t m_bitsMigratedBuckets;

	// Every index below the count is either in use or on the free list
	CUtlLeanVector<Node_t, IndexType_t> m_memNodes;
	IndexType_t m_iNodeFreeListHead;

	IndexType_t m_cElements;

	// Buckets in [min, max) may still hold nodes that belong to a bucket added by the last growth
	IndexType_t m_nMinRehashedBucket;
	IndexType_t m_nMaxRehashedBucket;

	EqualityFunc_t m_EqualityFunc;
	HashFunc_t m_HashFunc;
};


//-----------------------------------------------------------------------------
// Purpose: inserts and constructs a key into the map.
// Element member is left unconstructed (to be copy constructed or default-constructed by a wrapper function)
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
template <typename pf_key>
inline int CUtlHashMapLarge<K, T, L, H>::InsertUnconstructed( pf_key &&key, IndexType_t *piNodeExistingIfDupe, bool bAllowDupes )
{
	// make sure we have room in the hash table
	if ( m_cElements >= m_vecHashBuckets.Count() )
		EnsureCapacity( MAX( 16, m_vecHashBuckets.Count() * 2 ) );
	if ( m_cElements >= m_memNodes.NumAllocated() )
		m_memNodes.EnsureCapacity( MAX( 16, m_memNodes.NumAllocated() * 2 ), true );

	if ( m_nMinRehashedBucket < m_nMaxRehashedBucket )
		IncrementalRehash();

	uint32 hash = HashKey( key );

	// Migrate the buckets this key had under the smaller bucket counts, so all
	// nodes with this key end up in its current bucket
	int cBucketsToModAgainst = m_vecHashBuckets.Count() >> 1;
	int iBucketMigrate = hash & ( cBucketsToModAgainst - 1 );
	while ( cBucketsToModAgainst >= 1 && iBucketMigrate >= m_nMinRehashedBucket && !m_bitsMigratedBuckets.GetBit( iBucketMigrate ) )
	{
		RehashNodesInBucket( iBucketMigrate );
		cBucketsToModAgainst >>= 1;
		iBucketMigrate = hash & ( cBucketsToModAgainst - 1 );
	}

	int iBucket = hash & BucketMask();

	// return existing node without insert, if duplicates are not permitted
	if ( !bAllowDupes && m_cElements != 0 )
	{
		IndexType_t iNode = FindInBucket( iBucket, key );
		if ( piNodeExistingIfDupe )
			*piNodeExistingIfDupe = iNode;
		if ( iNode != kInvalidIndex )
			return kInvalidIndex;
	}

	IndexType_t iNewNode = AllocNode();
	Node_t &node = m_memNodes[iNewNode];
	node.m_iNextNode = kInvalidIndex;
	Construct( &node.m_key, std::forward<pf_key>( key ) );

	LinkNodeIntoBucket( iBucket, iNewNode );

	if ( piNodeExistingIfDupe )
		*piNodeExistingIfDupe = kInvalidIndex;

	return iNewNode;
}

//-----------------------------------------------------------------------------
// Purpose: inserts a default-constructed item into the map, or finds the existing one
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
template <typename pf_key>
inline int CUtlHashMapLarge<K, T, L, H>::FindOrInsert_Internal( pf_key &&key, bool bReplace )
{
	IndexType_t iNodeExisting;
	IndexType_t iNodeInserted = InsertUnconstructed( std::forward<pf_key>( key ), &iNodeExisting, false );
	if ( iNodeInserted != kInvalidIndex )
	{
		Construct( &m_memNodes[iNodeInserted].m_elem );
		return iNodeInserted;
	}

	if ( bReplace )
	{
		Destruct( &m_memNodes[iNodeExisting].m_elem );
		Construct( &m_memNodes[iNodeExisting].m_elem );
	}
	return iNodeExisting;
}

//-----------------------------------------------------------------------------
// Purpose: inserts an item into the map, or finds and optionally replaces the existing one
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
template <typename pf_key, typename pf_elem>
inline int CUtlHashMapLarge<K, T, L, H>::FindOrInsert_Internal( pf_key &&key, pf_elem &&insert, bool bReplace )
{
	IndexType_t iNodeExisting;
	IndexType_t iNodeInserted = InsertUnconstructed( std::forward<pf_key>( key ), &iNodeExisting, false );
	if ( iNodeInserted != kInvalidIndex )
	{
		Construct( &m_memNodes[iNodeInserted].m_elem, std::forward<pf_elem>( insert ) );
		return iNodeInserted;
	}

	if ( bReplace )
		m_memNodes[iNodeExisting].m_elem = std::forward<pf_elem>( insert );
	return iNodeExisting;
}

//-----------------------------------------------------------------------------
// Purpose: inserts element no matter what, even if key already exists
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
template <typename pf_key, typename pf_elem>
inline int CUtlHashMapLarge<K, T, L, H>::InsertWithDupes_Internal( pf_key &&key, pf_elem &&insert )
{
	IndexType_t iNodeInserted = InsertUnconstructed( std::forward<pf_key>( key ), nullptr, true );
	Construct( &m_memNodes[iNodeInserted].m_elem, std::forward<pf_elem>( insert ) );
	return iNodeInserted;
}

//-----------------------------------------------------------------------------
// Purpose: grows the map to fit the specified amount
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::EnsureCapacity( int amount )
{
	m_memNodes.EnsureCapacity( amount, true );

	if ( amount <= m_vecHashBuckets.Count() )
		return;

	int cBucketsNeeded = MAX( 16, m_vecHashBuckets.Count() );
	while ( cBucketsNeeded < amount )
		cBucketsNeeded *= 2;

	// grow the hash buckets, new ones start empty
	int grow = cBucketsNeeded - m_vecHashBuckets.Count();
	int iFirst = m_vecHashBuckets.AddMultipleToTail( grow );
	V_memset( &m_vecHashBuckets[iFirst], 0xFF, grow * sizeof( HashBucket_t ) );

	// every bucket that existed before the growth has to be rehashed
	m_nMinRehashedBucket = 0;
	m_nMaxRehashedBucket = iFirst;
	if ( m_cElements > 0 )
	{
		m_bitsMigratedBuckets.Reset( m_vecHashBuckets.Count() );
	}
	else
	{
		// no elements - no rehashing
		m_nMinRehashedBucket = m_vecHashBuckets.Count();
	}
}

//-----------------------------------------------------------------------------
// Purpose: gets a new node, from the free list if possible
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline int CUtlHashMapLarge<K, T, L, H>::AllocNode()
{
	// if we're out of free elements, get the max
	if ( m_cElements == m_memNodes.Count() )
	{
		m_cElements++;
		return m_memNodes.AddToTail();
	}

	// pull from the free list
	Assert( m_iNodeFreeListHead != kInvalidIndex );
	IndexType_t iNewNode = m_iNodeFreeListHead;
	m_iNodeFreeListHead = FreeNodeIDToIndex( m_memNodes[iNewNode].m_iNextNode );
	m_cElements++;
	return iNewNode;
}

//-----------------------------------------------------------------------------
// Purpose: moves the nodes of a bucket to the bucket they hash to now
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::RehashNodesInBucket( int iBucketSrc )
{
	m_bitsMigratedBuckets.SetBit( iBucketSrc );

	IndexType_t iNode = m_vecHashBuckets[iBucketSrc].m_iNode;
	while ( iNode != kInvalidIndex )
	{
		IndexType_t iNodeNext = m_memNodes[iNode].m_iNextNode;

		int iBucketDest = HashKey( m_memNodes[iNode].m_key ) & BucketMask();
		if ( iBucketDest != iBucketSrc )
		{
			UnlinkNodeFromBucket( iBucketSrc, iNode );
			LinkNodeIntoBucket( iBucketDest, iNode );
		}

		iNode = iNodeNext;
	}
}

//-----------------------------------------------------------------------------
// Purpose: searches for an item by key, returning the index handle
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline int CUtlHashMapLarge<K, T, L, H>::Find( const KeyType_t &key ) const
{
	if ( m_cElements == 0 )
		return kInvalidIndex;

	uint32 hash = HashKey( key );

	IndexType_t iNode = FindInBucket( hash & BucketMask(), key );
	if ( iNode != kInvalidIndex )
		return iNode;

	// not found? we may have to look in older buckets
	for ( int cBucketsToModAgainst = m_vecHashBuckets.Count() >> 1; cBucketsToModAgainst >= MinBucketCountToProbe(); cBucketsToModAgainst >>= 1 )
	{
		int iBucket = hash & ( cBucketsToModAgainst - 1 );
		if ( !m_bitsMigratedBuckets.GetBit( iBucket ) )
		{
			iNode = FindInBucket( iBucket, key );
			if ( iNode != kInvalidIndex )
				return iNode;
		}
	}

	return kInvalidIndex;
}

//-----------------------------------------------------------------------------
// Purpose: searches a bucket for an item by key, returning the index handle
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline int CUtlHashMapLarge<K, T, L, H>::FindInBucket( int iBucket, const KeyType_t &key ) const
{
	IndexType_t iNode = m_vecHashBuckets[iBucket].m_iNode;
	while ( iNode != kInvalidIndex )
	{
		const Node_t &node = m_memNodes[iNode];
		if ( m_EqualityFunc( key, node.m_key ) )
			return iNode;

		iNode = node.m_iNextNode;
	}

	return kInvalidIndex;
}

//-----------------------------------------------------------------------------
// Purpose: links a node into the start of a bucket
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::LinkNodeIntoBucket( int iBucket, IndexType_t iNewNode )
{
	m_memNodes[iNewNode].m_iNextNode = m_vecHashBuckets[iBucket].m_iNode;
	m_vecHashBuckets[iBucket].m_iNode = iNewNode;
}

//-----------------------------------------------------------------------------
// Purpose: unlinks a node from a bucket
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::UnlinkNodeFromBucket( int iBucket, IndexType_t iNodeToUnlink )
{
	IndexType_t iNodeNext = m_memNodes[iNodeToUnlink].m_iNextNode;

	IndexType_t iNode = m_vecHashBuckets[iBucket].m_iNode;
	if ( iNode == iNodeToUnlink )
	{
		m_vecHashBuckets[iBucket].m_iNode = iNodeNext;
		return;
	}

	while ( iNode != kInvalidIndex )
	{
		if ( m_memNodes[iNode].m_iNextNode == iNodeToUnlink )
		{
			m_memNodes[iNode].m_iNextNode = iNodeNext;
			return;
		}
		iNode = m_memNodes[iNode].m_iNextNode;
	}

	Assert( false );
}

//-----------------------------------------------------------------------------
// Purpose: removes a single item from the map
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::RemoveAt( IndexType_t i )
{
	if ( !IsValidIndex( i ) )
	{
		Assert( false );
		return;
	}

	// we have to re-hash to find which bucket we're in
	uint32 hash = HashKey( m_memNodes[i].m_key );
	if ( RemoveNodeFromBucket( hash & BucketMask(), i ) )
		return;

	// wasn't found; look in older buckets
	for ( int cBucketsToModAgainst = m_vecHashBuckets.Count() >> 1; cBucketsToModAgainst >= MinBucketCountToProbe(); cBucketsToModAgainst >>= 1 )
	{
		int iBucket = hash & ( cBucketsToModAgainst - 1 );
		if ( !m_bitsMigratedBuckets.GetBit( iBucket ) && RemoveNodeFromBucket( iBucket, i ) )
			return;
	}

	// never found, container is busted
	Assert( false );
}

//-----------------------------------------------------------------------------
// Purpose: removes a node from the bucket, return true if it was found
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline bool CUtlHashMapLarge<K, T, L, H>::RemoveNodeFromBucket( int iBucket, IndexType_t iNodeToRemove )
{
	IndexType_t iNode = m_vecHashBuckets[iBucket].m_iNode;
	while ( iNode != kInvalidIndex )
	{
		if ( iNodeToRemove == iNode )
		{
			UnlinkNodeFromBucket( iBucket, iNodeToRemove );

			Node_t &node = m_memNodes[iNode];
			Destruct( &node.m_key );
			Destruct( &node.m_elem );

			// link into free list
			node.m_iNextNode = FreeNodeIndexToID( m_iNodeFreeListHead );
			m_iNodeFreeListHead = iNode;
			if ( --m_cElements == 0 )
			{
				// no elements - no rehashing
				m_nMinRehashedBucket = m_vecHashBuckets.Count();
			}
			return true;
		}

		iNode = m_memNodes[iNode].m_iNextNode;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: rehashes the next bucket that still needs it, if any
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::IncrementalRehash()
{
	while ( m_nMinRehashedBucket < m_nMaxRehashedBucket )
	{
		int iBucket = m_nMinRehashedBucket++;
		if ( m_vecHashBuckets[iBucket].m_iNode != kInvalidIndex && !m_bitsMigratedBuckets.GetBit( iBucket ) )
		{
			// only do one, we may be on a rapid growth path
			RehashNodesInBucket( iBucket );
			break;
		}
	}

	if ( m_nMinRehashedBucket >= m_nMaxRehashedBucket )
	{
		m_nMinRehashedBucket = m_vecHashBuckets.Count();
		m_nMaxRehashedBucket = kInvalidIndex;
		m_bitsMigratedBuckets.Reset( 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: destructs the key and element of every valid node
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::DestructAll()
{
	for ( IndexType_t i = First(); i != kInvalidIndex; i = Next( i ) )
	{
		Node_t &node = m_memNodes[i];
		Destruct( &node.m_key );
		Destruct( &node.m_elem );
	}
}

//-----------------------------------------------------------------------------
// Purpose: removes all items from the hash map
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::RemoveAll()
{
	DestructAll();

	m_iNodeFreeListHead = kInvalidIndex;
	m_cElements = 0;
	m_nMinRehashedBucket = m_vecHashBuckets.Count();
	m_nMaxRehashedBucket = kInvalidIndex;
	m_bitsMigratedBuckets.Reset( 0 );
	if ( m_vecHashBuckets.Count() > 0 )
		V_memset( m_vecHashBuckets.Base(), 0xFF, m_vecHashBuckets.Count() * sizeof( HashBucket_t ) );
	m_memNodes.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: removes all items from the hash map and frees all memory
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H>
inline void CUtlHashMapLarge<K, T, L, H>::Purge()
{
	DestructAll();

	m_iNodeFreeListHead = kInvalidIndex;
	m_cElements = 0;
	m_nMinRehashedBucket = kInvalidIndex;
	m_nMaxRehashedBucket = kInvalidIndex;
	m_bitsMigratedBuckets.Reset( 0 );
	m_memNodes.Purge();
	m_vecHashBuckets.Purge();
}

#include "memdbgoff.h"

#endif // UTLHASHMAPLARGE_H
