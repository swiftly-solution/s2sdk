#ifndef UTLITERATOR_H
#define UTLITERATOR_H

#ifdef _WIN32
#pragma once
#endif

#include "utlcommon.h"
#include <iterator>

template < typename Container, bool bConstIterator >
class CUtlForwardIteratorImplT
{
public:
	typedef typename CTypeSelect< bConstIterator, const Container, Container >::type Container_t;
	typedef typename Container::IndexType_t IndexType_t;

	// STL-like typedefs
	typedef std::forward_iterator_tag iterator_category;
	typedef typename CTypeSelect< bConstIterator, const typename Container::ElemType_t, typename Container::ElemType_t >::type value_type;
	typedef value_type * pointer;
	typedef value_type & reference;
	typedef uintptr_t difference_type;

	CUtlForwardIteratorImplT() { }
	CUtlForwardIteratorImplT( const CUtlForwardIteratorImplT< Container, false >& copy_or_requalify_to_const )
		: m_pContainer( copy_or_requalify_to_const._container() ), m_iElement( copy_or_requalify_to_const._index() ) { }

	CUtlForwardIteratorImplT& operator++() // pre-increment
	{
		Assert( m_pContainer && *this != m_pContainer->end() );
		m_iElement = m_pContainer->IteratorNext( m_iElement );
		return *this;
	}

	CUtlForwardIteratorImplT operator++(int) // post-increment
	{
		CUtlForwardIteratorImplT pre = *this;
		this->operator++();
		return pre;
	}

	template < bool bOtherConst >
	bool operator==( const CUtlForwardIteratorImplT< Container, bOtherConst >& other ) const
	{
		Assert( m_pContainer == other._container() );
		return m_iElement == other._index();
	}

	template < bool bOtherConst >
	bool operator!=( const CUtlForwardIteratorImplT< Container, bOtherConst >& other ) const
	{
		Assert( m_pContainer == other._container() );
		return m_iElement != other._index();
	}

	value_type & operator*() const { return m_pContainer->Element( m_iElement ); }
	value_type * operator->() const { return &m_pContainer->Element( m_iElement ); }

	Container_t * _container() const { return m_pContainer; }
	IndexType_t _index() const { return m_iElement; }

protected:
	Container_t *m_pContainer = nullptr;
	IndexType_t m_iElement = 0;

	friend Container;
	CUtlForwardIteratorImplT( Container_t *pContainer, IndexType_t iElement ) : m_pContainer( pContainer ), m_iElement( iElement ) { }
};

template < typename Container, bool bConstIterator >
class CUtlBidirectionalIteratorImplT : public CUtlForwardIteratorImplT< Container, bConstIterator >
{
protected:
	typedef CUtlForwardIteratorImplT< Container, bConstIterator > Base;
	using Base::m_pContainer;
	using Base::m_iElement;

public:
	typedef std::bidirectional_iterator_tag iterator_category;

	CUtlBidirectionalIteratorImplT() { }
	CUtlBidirectionalIteratorImplT( const CUtlBidirectionalIteratorImplT< Container, false >& copy_or_requalify_to_const )
		: Base( copy_or_requalify_to_const ) { }

	CUtlBidirectionalIteratorImplT& operator++() // pre-increment
	{
		Base::operator++();
		return *this;
	}

	CUtlBidirectionalIteratorImplT operator++(int) // post-increment
	{
		CUtlBidirectionalIteratorImplT pre = *this;
		Base::operator++();
		return pre;
	}

	CUtlBidirectionalIteratorImplT& operator--() // pre-decrement
	{
		Assert( m_pContainer && *this != m_pContainer->begin() );
		m_iElement = m_pContainer->IteratorPrev( m_iElement );
		return *this;
	}

	CUtlBidirectionalIteratorImplT operator--(int) // post-decrement
	{
		CUtlBidirectionalIteratorImplT pre = *this;
		this->operator--();
		return pre;
	}

protected:
	friend Container;
	CUtlBidirectionalIteratorImplT( typename Base::Container_t *pContainer, typename Base::IndexType_t iElement ) : Base( pContainer, iElement ) { }
};


#endif