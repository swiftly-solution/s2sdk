#ifndef UTLDICT_H
#define UTLDICT_H

#ifdef _WIN32
#pragma once
#endif

#include "utlmap.h"

#include "memdbgon.h"

enum EDictCompareType
{
	k_eDictCompareTypeCaseSensitive=0,
	k_eDictCompareTypeCaseInsensitive=1,
	k_eDictCompareTypeCaseInsensitiveFast=2,
	k_eDictCompareTypeFilenames				// Slashes and backslashes count as the same character..
};

template <int COMPARE_TYPE> struct CDictCompareTypeDeducer {};
template <> struct CDictCompareTypeDeducer<k_eDictCompareTypeCaseSensitive> { typedef CDefStringLess Type_t; };
template <> struct CDictCompareTypeDeducer<k_eDictCompareTypeCaseInsensitive> { typedef CDefCaselessStringLess Type_t; };
template <> struct CDictCompareTypeDeducer<k_eDictCompareTypeCaseInsensitiveFast> { typedef CDefFastCaselessStringLess Type_t; };
template <> struct CDictCompareTypeDeducer<k_eDictCompareTypeFilenames> { typedef CDefCaselessStringLessIgnoreSlashes Type_t; };

#define FOR_EACH_DICT( dictName, iteratorName ) \
	for( auto iteratorName=dictName.First(); iteratorName != dictName.InvalidIndex(); iteratorName = dictName.Next( iteratorName ) )

// faster iteration, but in an unspecified order
#define FOR_EACH_DICT_FAST( dictName, iteratorName ) \
	for ( decltype( dictName.MaxElement() ) iteratorName = 0; iteratorName < dictName.MaxElement(); ++iteratorName ) if ( !dictName.IsValidIndex( iteratorName ) ) continue; else
	
template <class T, class I = int, int COMPARE_TYPE = k_eDictCompareTypeCaseInsensitiveFast> 
class CUtlDict
{
public:
	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	CUtlDict( int growSize = 0, int initSize = 0 );
	~CUtlDict( );

	void EnsureCapacity( int );
	
	// gets particular elements
	T&         Element( I i );
	const T&   Element( I i ) const;
	T&         operator[]( I i );
	const T&   operator[]( I i ) const;

	// gets element names
	char	   *GetElementName( I i );
	char const *GetElementName( I i ) const;

	void		SetElementName( I i, char const *pName );

	// Number of elements
	unsigned int Count() const;
	
	// Number of allocated slots
	I MaxElement() const;
	
	// Checks if a node is valid and in the tree
	bool  IsValidIndex( I i ) const;
	
	// Invalid index
	static I InvalidIndex();
	
	// Insert method (inserts in order)
	I  Insert( const char *pName, const T &element );
	I  Insert( const char *pName );
	
	// Find method
	I  Find( const char *pName ) const;
	
	// Remove methods
	void	RemoveAt( I i );
	void	Remove( const char *pName );
	void	RemoveAll( );
	
	// Purge memory
	void	Purge();
	void	PurgeAndDeleteElements();	// Call delete on each element.

	// Iteration methods
	I		First() const;
	I		Next( I i ) const;

	// Nested typedefs, for code that might need 
	// to fish out the index type from a given dict
	typedef I IndexType_t;

protected:
	typedef CUtlOrderedMap<const char *, T, typename CDictCompareTypeDeducer<COMPARE_TYPE>::Type_t, I> DictElementMap_t;
	DictElementMap_t m_Elements;

public:
	// STL / C++11-style iterators over the nodes in name order, with the name in key and the element in elem.
	// The dictionary owns the names, change one with SetElementName.
	typedef typename DictElementMap_t::iterator iterator;
	typedef typename DictElementMap_t::const_iterator const_iterator;
	const_iterator begin() const { return m_Elements.begin(); }
	const_iterator end() const { return m_Elements.end(); }
	iterator begin() { return m_Elements.begin(); }
	iterator end() { return m_Elements.end(); }
};

template <class T, class I, int COMPARE_TYPE>
CUtlDict<T, I, COMPARE_TYPE>::CUtlDict( int growSize, int initSize ) : m_Elements( growSize, initSize )
{
}

template <class T, class I, int COMPARE_TYPE> 
CUtlDict<T, I, COMPARE_TYPE>::~CUtlDict()
{
	Purge();
}

template <class T, class I, int COMPARE_TYPE>
inline void CUtlDict<T, I, COMPARE_TYPE>::EnsureCapacity( int num )        
{ 
	return m_Elements.EnsureCapacity( num ); 
}

template <class T, class I, int COMPARE_TYPE>
inline T& CUtlDict<T, I, COMPARE_TYPE>::Element( I i )        
{ 
	return m_Elements[i]; 
}

template <class T, class I, int COMPARE_TYPE>
inline const T& CUtlDict<T, I, COMPARE_TYPE>::Element( I i ) const  
{ 
	return m_Elements[i]; 
}

template <class T, class I, int COMPARE_TYPE>
inline char *CUtlDict<T, I, COMPARE_TYPE>::GetElementName( I i )
{
	return (char *)m_Elements.Key( i );
}

template <class T, class I, int COMPARE_TYPE>
inline char const *CUtlDict<T, I, COMPARE_TYPE>::GetElementName( I i ) const
{
	return m_Elements.Key( i );
}

template <class T, class I, int COMPARE_TYPE>
inline T& CUtlDict<T, I, COMPARE_TYPE>::operator[]( I i )        
{ 
	return Element(i); 
}

template <class T, class I, int COMPARE_TYPE>
inline const T & CUtlDict<T, I, COMPARE_TYPE>::operator[]( I i ) const  
{ 
	return Element(i); 
}

template <class T, class I, int COMPARE_TYPE>
inline void CUtlDict<T, I, COMPARE_TYPE>::SetElementName( I i, char const *pName )
{
	// TODO:  This makes a copy of the old element
	// TODO:  This relies on the rb tree putting the most recently
	//  removed element at the head of the insert list
	free( (void *)m_Elements.Key( i ) );
	m_Elements.Reinsert( strdup( pName ), i );
}

//-----------------------------------------------------------------------------
// Num elements
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE>
inline unsigned int CUtlDict<T, I, COMPARE_TYPE>::Count() const          
{ 
	return m_Elements.Count(); 
}

//-----------------------------------------------------------------------------
// Number of allocated slots
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE>
inline I CUtlDict<T, I, COMPARE_TYPE>::MaxElement() const
{
	return m_Elements.MaxElement();
}

//-----------------------------------------------------------------------------
// Checks if a node is valid and in the tree
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE>
inline bool CUtlDict<T, I, COMPARE_TYPE>::IsValidIndex( I i ) const 
{
	return m_Elements.IsValidIndex(i);
}
	
	
//-----------------------------------------------------------------------------
// Invalid index
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE>
inline I CUtlDict<T, I, COMPARE_TYPE>::InvalidIndex()         
{ 
	return DictElementMap_t::InvalidIndex(); 
}

	
//-----------------------------------------------------------------------------
// Delete a node from the tree
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE>
void CUtlDict<T, I, COMPARE_TYPE>::RemoveAt(I elem) 
{
	free( (void *)m_Elements.Key( elem ) );
	m_Elements.RemoveAt(elem);
}


//-----------------------------------------------------------------------------
// remove a node in the tree
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE> 
void CUtlDict<T, I, COMPARE_TYPE>::Remove( const char *search )
{
	I node = Find( search );
	if (node != InvalidIndex())
	{
		RemoveAt(node);
	}
}


//-----------------------------------------------------------------------------
// Removes all nodes from the tree
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE>
void CUtlDict<T, I, COMPARE_TYPE>::RemoveAll()
{
	typename DictElementMap_t::IndexType_t index = m_Elements.FirstInorder();
	while ( index != m_Elements.InvalidIndex() )
	{
		free( (void *)m_Elements.Key( index ) );
		index = m_Elements.NextInorder( index );
	}

	m_Elements.RemoveAll();
}

template <class T, class I, int COMPARE_TYPE>
void CUtlDict<T, I, COMPARE_TYPE>::Purge()
{
	RemoveAll();
}


template <class T, class I, int COMPARE_TYPE>
void CUtlDict<T, I, COMPARE_TYPE>::PurgeAndDeleteElements()
{
	// Delete all the elements.
	I index = m_Elements.FirstInorder();
	while ( index != m_Elements.InvalidIndex() )
	{
		free( (void *)m_Elements.Key( index ) );
		if constexpr ( std::is_pointer_v<T> )
		{
			delete m_Elements[index];
		}
		index = m_Elements.NextInorder( index );
	}

	m_Elements.RemoveAll();
}


//-----------------------------------------------------------------------------
// inserts a node into the tree
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE> 
I CUtlDict<T, I, COMPARE_TYPE>::Insert( const char *pName, const T &element )
{
	return m_Elements.Insert( strdup( pName ), element );
}

template <class T, class I, int COMPARE_TYPE> 
I CUtlDict<T, I, COMPARE_TYPE>::Insert( const char *pName )
{
	return m_Elements.Insert( strdup( pName ) );
}


//-----------------------------------------------------------------------------
// finds a node in the tree
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE> 
I CUtlDict<T, I, COMPARE_TYPE>::Find( const char *pName ) const
{
	if ( pName )
		return m_Elements.Find( pName );
	else
		return InvalidIndex();
}


//-----------------------------------------------------------------------------
// Iteration methods
//-----------------------------------------------------------------------------
template <class T, class I, int COMPARE_TYPE> 
I CUtlDict<T, I, COMPARE_TYPE>::First() const
{
	return m_Elements.FirstInorder();
}

template <class T, class I, int COMPARE_TYPE> 
I CUtlDict<T, I, COMPARE_TYPE>::Next( I i ) const
{
	return m_Elements.NextInorder(i);
}

#include "memdbgoff.h"

#endif