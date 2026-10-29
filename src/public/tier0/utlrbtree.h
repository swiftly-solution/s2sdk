#ifndef UTLRBTREE_H
#define UTLRBTREE_H

#ifdef _WIN32
#pragma once
#endif

#include "strtools.h"
#include "utlleanvector.h"
#include "utliterator.h"
#include <string.h>
#include <type_traits>

#define FOR_EACH_UTLRBTREE( treeName, iteratorName ) \
	for ( auto iteratorName = treeName.FirstInorder(); iteratorName != treeName.InvalidIndex(); iteratorName = treeName.NextInorder( iteratorName ) )

template <typename T >
class CDefOps
{
public:
	static bool LessFunc( const T &lhs, const T &rhs )	{ return ( lhs < rhs );	}
};

#define DefLessFunc( type ) CDefOps< type >::LessFunc

template <typename T>
class CDefLess
{
public:
	CDefLess() {}
	CDefLess( int i ) {}
	inline bool operator()( const T &lhs, const T &rhs ) const { return ( lhs < rhs );	}
	inline bool operator!() const { return false; }
};

bool StringLessThan( const char * const &lhs, const char * const &rhs);
bool CaselessStringLessThan( const char * const &lhs, const char * const &rhs );	
bool FastCaselessStringLessThan( const char * const &lhs, const char * const &rhs );	
bool CaselessStringLessThanIgnoreSlashes( const char * const &lhs, const char * const &rhs );

class CDefStringLess
{
public:
	CDefStringLess();
	CDefStringLess( int i );
	bool operator()( const char * const &lhs, const char * const &rhs ) const;
	bool operator!() const;
};

class CDefCaselessStringLess
{
public:
	CDefCaselessStringLess();
	CDefCaselessStringLess( int i );
	bool operator()( const char * const &lhs, const char * const &rhs ) const;
	bool operator!() const;
};

class CDefFastCaselessStringLess
{
public:
	CDefFastCaselessStringLess();
	CDefFastCaselessStringLess( int i );
	bool operator()( const char * const &lhs, const char * const &rhs ) const;
	bool operator!() const;
};

class CDefCaselessStringLessIgnoreSlashes
{
public:
	CDefCaselessStringLessIgnoreSlashes();
	CDefCaselessStringLessIgnoreSlashes( int i );
	bool operator()( const char * const &lhs, const char * const &rhs ) const;
	bool operator!() const;
};

template <> inline bool CDefOps<const char *>::LessFunc( const char * const &lhs, const char * const &rhs )	{ return StringLessThan( lhs, rhs ); }
template <> inline bool CDefOps<char *>::LessFunc( char * const &lhs, char * const &rhs )						{ return StringLessThan( lhs, rhs ); }

template <typename RBTREE_T>
void SetDefLessFunc( RBTREE_T &RBTree )
{
	RBTree.SetLessFunc( DefLessFunc( typename RBTREE_T::KeyType_t ) );
}

enum CompareOperands_t
{
	k_EEqual = 0x1,
	k_EGreaterThan = 0x2,
	k_ELessThan = 0x4,
	k_EGreaterThanOrEqualTo = k_EGreaterThan | k_EEqual,
	k_ELessThanOrEqualTo = k_ELessThan | k_EEqual,
};

// For use with Find.
enum FindCondition_t
{
	EXACT_MATCH = 0,
	MATCH_OR_LESS = 1,
	MATCH_OR_GREATER = 2,
};

// For use with Insert.
enum ERBTreeInsertBehavior
{
	// Asserts through UtlRBTree_FailedInsertDuplicate, then replaces the existing element
	k_eInsertAssertAboutDupes = 0,
	k_eInsertAllowDupes = 1,
	// Replaces the existing element
	k_eInsertUpdateDupes = 2,
};

PLATFORM_INTERFACE void UtlRBTree_FailedInsertDuplicate();

template < class I >
struct UtlRBTreeLinks_t
{
	I  m_Left;
	I  m_Right;
	I  m_Parent;
	I  m_Tag;
};

// Data is constructed and destructed by the tree, not by the node vector
template < class T, class I >
struct UtlRBTreeNode_t : public UtlRBTreeLinks_t< I >
{
	typedef void TriviallyRelocatable_t;

	T  m_Data;
};

// Holds the less func, taking no space when it's stateless
template < typename L, bool = std::is_empty_v<L> && !std::is_final_v<L> >
class CUtlRBTreeLessFunc
{
protected:
	CUtlRBTreeLessFunc( const L &lessfunc ) : m_LessFunc( lessfunc ) {}

	L &LessFunc() { return m_LessFunc; }
	const L &LessFunc() const { return m_LessFunc; }

private:
	L m_LessFunc;
};

template < typename L >
class CUtlRBTreeLessFunc<L, true> : private L
{
protected:
	CUtlRBTreeLessFunc( const L &lessfunc ) : L( lessfunc ) {}

	L &LessFunc() { return *this; }
	const L &LessFunc() const { return *this; }
};

template < class T, class I = unsigned short, typename L = CDefLess< T >, class M = CUtlLeanVector< UtlRBTreeNode_t< T, I >, I > >
class CUtlRBTree : protected CUtlRBTreeLessFunc<L>
{
	typedef CUtlRBTreeLessFunc<L> LessFuncBase;
	using LessFuncBase::LessFunc;

public:

	typedef T KeyType_t;
	typedef T ElemType_t;
	typedef I IndexType_t;

	// Less func typedef
	// Returns true if the first parameter is "less" than the second
	typedef L LessFunc_t;

	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
	CUtlRBTree( int growSize = 0, int initSize = 0, const LessFunc_t &lessfunc = 0 );
	CUtlRBTree( const LessFunc_t &lessfunc );
	~CUtlRBTree( );

	void EnsureCapacity( int num );

	void CopyFrom( const CUtlRBTree<T, I, L, M> &other );

	// gets particular elements
	T&			Element( I i );
	T const		&Element( I i ) const;
	T&			operator[]( I i );
	T const		&operator[]( I i ) const;

	// Gets the root
	I  Root() const;

	// Num elements
	I  Count() const;
	bool  IsEmpty() const;

	// Max "size" of the vector
	// it's not generally safe to iterate from index 0 to MaxElement()-1
	// it IS safe to do so when using CUtlVectorMemory_Growable as the allocator,
	// but we should really remove patterns using this anyways, for safety and generality
	I  MaxElement() const;

	// Gets the children                               
	I  Parent( I i ) const;
	I  LeftChild( I i ) const;
	I  RightChild( I i ) const;

	// Tests if a node is a left or right child
	bool  IsLeftChild( I i ) const;
	bool  IsRightChild( I i ) const;

	// Tests if root or leaf
	bool  IsRoot( I i ) const;
	bool  IsLeaf( I i ) const;

	// Checks if a node is valid and in the tree
	bool  IsValidIndex( I i ) const;

	// Checks if the tree as a whole is valid
	bool  IsValid() const;

	// Invalid index
	static I InvalidIndex();

	// returns the tree depth (not a very fast operation)
	int   Depth( I node ) const;
	int   Depth() const;

	// Sets the less func
	void SetLessFunc( const LessFunc_t &func );

	// Allocation method
	I  NewNode( bool bConstructElement );

	// Insert method (inserts in order)
	I  Insert( T const &insert, ERBTreeInsertBehavior eInsertBehavior = k_eInsertAssertAboutDupes );
	// Moves the element in, so it doesn't need to be copyable
	I  Insert( T &&insert, ERBTreeInsertBehavior eInsertBehavior = k_eInsertAssertAboutDupes );
	void Insert( const T *pArray, int nItems, ERBTreeInsertBehavior eInsertBehavior = k_eInsertAssertAboutDupes );
	I  InsertIfNotFound( T const &insert );

	// pInserted reports whether a new element was inserted
	I  FindOrInsert( T const &insert, bool *pInserted = NULL );

	// Find method
	I  Find( T const &search ) const;

	// Finds the key, or the nearest lesser/greater element per eFindCondition
	I  Find( T const &search, FindCondition_t eFindCondition ) const;

	// Finds the first element (inorder) with this key when duplicates exist
	I  FindFirst( T const &search ) const;

	// Finds the closest element to the key per the comparison criteria
	I  FindClosest( T const &search, CompareOperands_t eFindCriteria ) const;

	bool  HasElement( T const &search ) const;

	// Remove methods
	void     RemoveAt( I i );
	bool     Remove( T const &remove );
	void     RemoveAll( );
	void	 Purge();

	// Only valid when T is a pointer type
	void RemoveAllAndDeleteElements();
	void PurgeAndDeleteElements();

	// Allocation, deletion
	void  FreeNode( I i );

	// Iteration
	I  FirstInorder() const;
	I  NextInorder( I i ) const;
	I  PrevInorder( I i ) const;
	I  LastInorder() const;

	I  FirstPreorder() const;
	I  NextPreorder( I i ) const;
	I  PrevPreorder( I i ) const;
	I  LastPreorder( ) const;

	I  FirstPostorder() const;
	I  NextPostorder( I i ) const;

	// STL / C++11-style iterators, in order
	typedef CUtlBidirectionalIteratorImplT< CUtlRBTree< T, I, L, M >, false > iterator;
	typedef CUtlBidirectionalIteratorImplT< CUtlRBTree< T, I, L, M >, true > const_iterator;
	const_iterator begin() const { return const_iterator( this, FirstInorder() ); }
	const_iterator end() const { return const_iterator( this, InvalidIndex() ); }
	iterator begin() { return iterator( this, FirstInorder() ); }
	iterator end() { return iterator( this, InvalidIndex() ); }
	I IteratorNext( I i ) const { return NextInorder( i ); }
	I IteratorPrev( I i ) const { return i == InvalidIndex() ? LastInorder() : PrevInorder( i ); }

	// If you change the search key, this can be used to reinsert the 
	// element into the tree.
	void	Reinsert( I elem );

	// swap in place
	void Swap( CUtlRBTree< T, I, L, M > &that );

private:
	// Can't copy the tree this way!
	CUtlRBTree<T, I, L, M>& operator=( const CUtlRBTree<T, I, L, M> &other );

protected:
	enum NodeColor_t
	{
		RED = 0,
		BLACK
	};

	typedef UtlRBTreeNode_t< T, I > Node_t;
	typedef UtlRBTreeLinks_t< I > Links_t;

	// Sets the children
	void  SetParent( I i, I parent );
	void  SetLeftChild( I i, I child  );
	void  SetRightChild( I i, I child  );
	void  LinkToParent( I i, I parent, bool isLeft );

	// Gets at the links
	Links_t const	&Links( I i ) const;
	Links_t			&Links( I i );      

	// Checks if a link is red or black
	bool IsRed( I i ) const;
	bool IsBlack( I i ) const;

	// Sets/gets node color
	NodeColor_t Color( I i ) const;
	void        SetColor( I i, NodeColor_t c );

	// operations required to preserve tree balance
	void RotateLeft(I i);
	void RotateRight(I i);
	void InsertRebalance(I i);
	void RemoveRebalance(I i);

	// Insertion, removal
	I  InsertAt( I parent, bool leftchild, bool bConstructElement );

	// copy constructors not allowed
	CUtlRBTree( CUtlRBTree<T, I, L, M> const &tree );

	// Inserts a node into the tree, doesn't copy the data in.
	void FindInsertionPosition( T const &insert, I &parent, bool &leftchild );

	template < typename U >
	I  InsertInternal( U &&insert, ERBTreeInsertBehavior eInsertBehavior );

	// Remove and add back an element in the tree.
	void	Unlink( I elem );
	void	Link( I elem );

	M m_Elements;
	I m_Root;
	I m_NumElements;
	I m_FirstFree;

	M const &Elements( void ) const
	{
		return m_Elements;
	}
};


#endif