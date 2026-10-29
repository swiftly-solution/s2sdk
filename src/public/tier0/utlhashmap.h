//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: index-based hash map container
//			Use FOR_EACH_HASHMAP to iterate through CUtlHashMap.
//
//=============================================================================//

#ifndef UTLHASHMAP_H
#define UTLHASHMAP_H

#ifdef _WIN32
#pragma once
#endif

#include "dbg.h"
#include "bitstring.h"
#include "utlcommon.h"
#include "utlleanvector.h"

#include <new>
#include <type_traits>
#include <utility>

#define FOR_EACH_HASHMAP( mapName, iteratorName ) \
	for ( decltype( (mapName).MaxElement() ) iteratorName = 0; iteratorName < (mapName).MaxElement(); ++iteratorName ) if ( !(mapName).IsValidIndex( iteratorName ) ) continue; else

template <typename T>
class CDefEquals
{
public:
	bool operator()( const T &lhs, const T &rhs ) const { return lhs == rhs; }
};

//-----------------------------------------------------------------------------
//
// Purpose: An associative container. Each item is not a separate allocation,
// so insertion of items can cause existing items to move in memory.
//
// Growing the buckets rehashes lazily: later inserts migrate one bucket at a time,
// and lookups also probe the buckets an item had under the smaller bucket counts.
//
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L = CDefEquals<K>, typename H = DefaultHashFunctor<K>, typename I = int>
struct CUtlHashMap
{
	// AMNOTE: Safeguard as current container isn't suitable at holding unsigned indicies
	static_assert( std::is_signed_v<I>, "CUtlHashMap needs a signed index type" );

public:
	using KeyType_t = K;
	using ElemType_t = T;
	using IndexType_t = I;
	using EqualityFunc_t = L;
	using HashFunc_t = H;
	static constexpr IndexType_t kInvalidIndex = -1;

	CUtlHashMap()
	{
		m_iNodeFreeListHead = kInvalidIndex;
		m_cElements = 0;
		m_nMinRehashedBucket = kInvalidIndex;
		m_nMaxRehashedBucket = kInvalidIndex;
	}

	CUtlHashMap( int cElementsExpected ) : CUtlHashMap()
	{
		EnsureCapacity( cElementsExpected );
	}

	~CUtlHashMap()
	{
		Purge();
	}

	CUtlHashMap( const CUtlHashMap & ) = delete;
	CUtlHashMap &operator=( const CUtlHashMap & ) = delete;

	void CopyFullHashMap( CUtlHashMap &target ) const
	{
		target.RemoveAll();
		FOR_EACH_HASHMAP( *this, i )
		{
			target.Insert( this->Key( i ), this->Element( i ) );
		}
	}

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

	// Checks if a node is valid and in the map.
	// Don't use it on the result of Find(), compare that against InvalidIndex() instead.
	bool IsValidIndex( IndexType_t i ) const				{ return (unsigned)i < (unsigned)m_memNodes.Count() && m_memNodes[i].m_iNextNode >= kInvalidIndex; }

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

	// Find-or-insert, one-arg - inserts a value-initialized element
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

	// Finds an exact key/value match, even with duplicate keys. Requires operator== for ElemType_t.
	IndexType_t FindExact( const KeyType_t &key, const ElemType_t &elem ) const;

	// Find next element with same key
	IndexType_t NextSameKey( IndexType_t i ) const;

	// Preallocates nodes and buckets for that many elements
	void EnsureCapacity( int amount );

	// Doesn't work if you pass a temporary to defaultValue
	const ElemType_t &FindElement( const KeyType_t &key, const ElemType_t &defaultValue ) const
	{
		IndexType_t i = Find( key );
		if ( i == kInvalidIndex )
			return defaultValue;
		return Element( i );
	}

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
		FOR_EACH_HASHMAP( *this, i )
			delete this->Element( i );
		Purge();
	}

protected:
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
	CBitStringT<CVariableBitStringBase> m_bitsMigratedBuckets;

	// Every index below the count is either in use or on the free list
	CUtlLeanVector<Node_t, IndexType_t> m_memNodes;
	IndexType_t m_iNodeFreeListHead;

	IndexType_t m_cElements;

	// Buckets in [min, max) may still hold nodes that belong to a bucket added by the last growth
	IndexType_t m_nMinRehashedBucket;
	IndexType_t m_nMaxRehashedBucket;

	EqualityFunc_t m_EqualityFunc;
	HashFunc_t m_HashFunc;

public:
	//
	// Range-based for loop iteration over the map. You can iterate
	// over the keys, the values ("elements"), or both ("items").
	//
	//   for ( auto k : map.IterKeys() ) { ... }
	//   for ( auto &v : map.IterValues() ) { ... }
	//   for ( auto item : map.IterItems() )
	//   {
	//       int i = item.Index();
	//       auto &k = item.Key();
	//       auto &v = item.Element();
	//   }
	//

	// A reference to a key/value pair in a map
	class ItemRef
	{
	protected:
		Node_t &m_node;
		const IndexType_t m_idx;
	public:
		ItemRef( const CUtlHashMap &map, IndexType_t idx ) : m_node( const_cast<Node_t &>( map.m_memNodes[idx] ) ), m_idx( idx ) {}
		ItemRef( const ItemRef &x ) = default;
		IndexType_t Index() const { return m_idx; }
		const KeyType_t &Key() const { return m_node.m_key; }
		const ElemType_t &Element() const { return m_node.m_elem; }
	};
	struct MutableItemRef : ItemRef
	{
		MutableItemRef( CUtlHashMap &map, IndexType_t idx ) : ItemRef( map, idx ) {}
		MutableItemRef( const MutableItemRef &x ) = default;
		using ItemRef::Element;
		ElemType_t &Element() const { return this->m_node.m_elem; }
	};

	// Base class iterator
	class Iterator
	{
	protected:
		CUtlHashMap &m_map;
		IndexType_t m_idx;
	public:
		Iterator( const CUtlHashMap &map, IndexType_t idx ) : m_map( const_cast<CUtlHashMap &>( map ) ), m_idx( idx ) {}
		Iterator( const Iterator &x ) = default;
		bool operator==( const Iterator &x ) const { return &m_map == &x.m_map && m_idx == x.m_idx; }
		bool operator!=( const Iterator &x ) const { return &m_map != &x.m_map || m_idx != x.m_idx; }
		void operator++()
		{
			if ( m_idx != kInvalidIndex )
				m_idx = m_map.Next( m_idx );
		}
	};
	struct MutableIterator : Iterator
	{
		MutableIterator( const MutableIterator &x ) = default;
		MutableIterator( CUtlHashMap &map, IndexType_t idx ) : Iterator( map, idx ) {}
	};
	struct KeyIterator : Iterator
	{
		using Iterator::Iterator;
		const KeyType_t &operator*() { return this->m_map.m_memNodes[this->m_idx].m_key; }
	};
	struct ConstValueIterator : Iterator
	{
		using Iterator::Iterator;
		const ElemType_t &operator*() { return this->m_map.m_memNodes[this->m_idx].m_elem; }
	};
	struct MutableValueIterator : MutableIterator
	{
		using MutableIterator::MutableIterator;
		ElemType_t &operator*() { return this->m_map.m_memNodes[this->m_idx].m_elem; }
	};
	struct ConstItemIterator : Iterator
	{
		using Iterator::Iterator;
		ItemRef operator*() { return ItemRef( this->m_map, this->m_idx ); }
	};
	struct MutableItemIterator : MutableIterator
	{
		using MutableIterator::MutableIterator;
		MutableItemRef operator*() { return MutableItemRef( this->m_map, this->m_idx ); }
	};

	// Internal type used by the IterXxx functions
	template <typename TIterator>
	class Range
	{
		CUtlHashMap &m_map;
	public:
		Range( const CUtlHashMap &map ) : m_map( const_cast<CUtlHashMap &>( map ) ) {}
		TIterator begin() const { return TIterator( m_map, m_map.First() ); }
		TIterator end() const { return TIterator( m_map, kInvalidIndex ); }
	};

	// Iterate over the keys. You will receive a reference to the key.
	Range<KeyIterator> IterKeys() const { return Range<KeyIterator>( *this ); }

	// Iterate over the values ("elements"). You will receive a reference to the value.
	Range<ConstValueIterator> IterValues() const { return Range<ConstValueIterator>( *this ); }
	Range<MutableValueIterator> IterValues() { return Range<MutableValueIterator>( *this ); }

	// Iterate over the "items" (key/value pairs). You will receive a small reference
	// object that is cheap to copy.
	Range<ConstItemIterator> IterItems() const { return Range<ConstItemIterator>( *this ); }
	Range<MutableItemIterator> IterItems() { return Range<MutableItemIterator>( *this ); }
};


//-----------------------------------------------------------------------------
// Purpose: inserts and constructs a key into the map.
// Element member is left unconstructed (to be copy constructed or default-constructed by a wrapper function)
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
template <typename pf_key>
inline I CUtlHashMap<K, T, L, H, I>::InsertUnconstructed( pf_key &&key, IndexType_t *piNodeExistingIfDupe, bool bAllowDupes )
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
	::new( &node.m_key ) KeyType_t( std::forward<pf_key>( key ) );

	LinkNodeIntoBucket( iBucket, iNewNode );

	if ( piNodeExistingIfDupe )
		*piNodeExistingIfDupe = kInvalidIndex;

	return iNewNode;
}

//-----------------------------------------------------------------------------
// Purpose: inserts a value-initialized item into the map, or finds the existing one
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
template <typename pf_key>
inline I CUtlHashMap<K, T, L, H, I>::FindOrInsert_Internal( pf_key &&key, bool bReplace )
{
	IndexType_t iNodeExisting;
	IndexType_t iNodeInserted = InsertUnconstructed( std::forward<pf_key>( key ), &iNodeExisting, false );
	if ( iNodeInserted != kInvalidIndex )
	{
		::new( &m_memNodes[iNodeInserted].m_elem ) ElemType_t();
		return iNodeInserted;
	}

	if ( bReplace )
	{
		Destruct( &m_memNodes[iNodeExisting].m_elem );
		::new( &m_memNodes[iNodeExisting].m_elem ) ElemType_t();
	}
	return iNodeExisting;
}

//-----------------------------------------------------------------------------
// Purpose: inserts an item into the map, or finds and optionally replaces the existing one
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
template <typename pf_key, typename pf_elem>
inline I CUtlHashMap<K, T, L, H, I>::FindOrInsert_Internal( pf_key &&key, pf_elem &&insert, bool bReplace )
{
	IndexType_t iNodeExisting;
	IndexType_t iNodeInserted = InsertUnconstructed( std::forward<pf_key>( key ), &iNodeExisting, false );
	if ( iNodeInserted != kInvalidIndex )
	{
		::new( &m_memNodes[iNodeInserted].m_elem ) ElemType_t( std::forward<pf_elem>( insert ) );
		return iNodeInserted;
	}

	if ( bReplace )
		m_memNodes[iNodeExisting].m_elem = std::forward<pf_elem>( insert );
	return iNodeExisting;
}

//-----------------------------------------------------------------------------
// Purpose: inserts element no matter what, even if key already exists
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
template <typename pf_key, typename pf_elem>
inline I CUtlHashMap<K, T, L, H, I>::InsertWithDupes_Internal( pf_key &&key, pf_elem &&insert )
{
	IndexType_t iNodeInserted = InsertUnconstructed( std::forward<pf_key>( key ), nullptr, true );
	::new( &m_memNodes[iNodeInserted].m_elem ) ElemType_t( std::forward<pf_elem>( insert ) );
	return iNodeInserted;
}

//-----------------------------------------------------------------------------
// Purpose: grows the map to fit the specified amount
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::EnsureCapacity( int amount )
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
		// Resize clears the bits it adds
		m_bitsMigratedBuckets.Resize( 0 );
		m_bitsMigratedBuckets.Resize( m_vecHashBuckets.Count() );
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
template <typename K, typename T, typename L, typename H, typename I>
inline I CUtlHashMap<K, T, L, H, I>::AllocNode()
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
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::RehashNodesInBucket( int iBucketSrc )
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
template <typename K, typename T, typename L, typename H, typename I>
inline I CUtlHashMap<K, T, L, H, I>::Find( const KeyType_t &key ) const
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
// Purpose: searches for an item by key and element equality, returning the index handle
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline I CUtlHashMap<K, T, L, H, I>::FindExact( const KeyType_t &key, const ElemType_t &elem ) const
{
	IndexType_t iNode = Find( key );
	while ( iNode != kInvalidIndex )
	{
		if ( elem == m_memNodes[iNode].m_elem )
			return iNode;
		iNode = NextSameKey( iNode );
	}
	return kInvalidIndex;
}

//-----------------------------------------------------------------------------
// Purpose: find the next element with the same key, if InsertWithDupes was used
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline I CUtlHashMap<K, T, L, H, I>::NextSameKey( IndexType_t i ) const
{
	if ( IsValidIndex( i ) )
	{
		const KeyType_t &key = m_memNodes[i].m_key;
		IndexType_t iNode = m_memNodes[i].m_iNextNode;
		while ( iNode != kInvalidIndex )
		{
			if ( m_EqualityFunc( key, m_memNodes[iNode].m_key ) )
				return iNode;

			iNode = m_memNodes[iNode].m_iNextNode;
		}
	}
	return kInvalidIndex;
}

//-----------------------------------------------------------------------------
// Purpose: searches a bucket for an item by key, returning the index handle
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline I CUtlHashMap<K, T, L, H, I>::FindInBucket( int iBucket, const KeyType_t &key ) const
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
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::LinkNodeIntoBucket( int iBucket, IndexType_t iNewNode )
{
	m_memNodes[iNewNode].m_iNextNode = m_vecHashBuckets[iBucket].m_iNode;
	m_vecHashBuckets[iBucket].m_iNode = iNewNode;
}

//-----------------------------------------------------------------------------
// Purpose: unlinks a node from a bucket
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::UnlinkNodeFromBucket( int iBucket, IndexType_t iNodeToUnlink )
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
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::RemoveAt( IndexType_t i )
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
template <typename K, typename T, typename L, typename H, typename I>
inline bool CUtlHashMap<K, T, L, H, I>::RemoveNodeFromBucket( int iBucket, IndexType_t iNodeToRemove )
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
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::IncrementalRehash()
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
		m_bitsMigratedBuckets.Resize( 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: destructs the key and element of every valid node
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::DestructAll()
{
	FOR_EACH_HASHMAP( *this, i )
	{
		Node_t &node = m_memNodes[i];
		Destruct( &node.m_key );
		Destruct( &node.m_elem );
	}
}

//-----------------------------------------------------------------------------
// Purpose: removes all items from the hash map
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::RemoveAll()
{
	if ( m_cElements > 0 )
	{
		DestructAll();

		m_nMinRehashedBucket = m_vecHashBuckets.Count();
		m_cElements = 0;
		m_iNodeFreeListHead = kInvalidIndex;
		m_nMaxRehashedBucket = kInvalidIndex;
		m_bitsMigratedBuckets.Resize( 0 );
		V_memset( m_vecHashBuckets.Base(), 0xFF, m_vecHashBuckets.Count() * sizeof( HashBucket_t ) );
		m_memNodes.RemoveAll();
	}
}

//-----------------------------------------------------------------------------
// Purpose: removes all items from the hash map and frees all memory
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L, typename H, typename I>
inline void CUtlHashMap<K, T, L, H, I>::Purge()
{
	if ( m_cElements > 0 )
		DestructAll();

	m_iNodeFreeListHead = kInvalidIndex;
	m_nMinRehashedBucket = kInvalidIndex;
	m_nMaxRehashedBucket = kInvalidIndex;
	m_cElements = 0;
	m_bitsMigratedBuckets.Resize( 0 );
	m_vecHashBuckets.Purge();
	m_memNodes.Purge();
}

#endif // UTLHASHMAP_H
