#ifndef UTLLEANVECTOR_H
#define UTLLEANVECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "rawallocator.h"

#include <type_traits>

#define FOR_EACH_LEANVEC( vecName, iteratorName ) \
	for ( auto iteratorName = vecName.First(); vecName.IsValidIterator( iteratorName ); iteratorName = vecName.Next( iteratorName ) )

template< class T, class = void >
struct IsTriviallyRelocatable : std::is_trivially_copyable<T> {};

template< class T >
struct IsTriviallyRelocatable< T, std::void_t<typename T::TriviallyRelocatable_t> > : std::true_type {};

template< class T, class I >
inline void RelocateLeanVectorElements( T *pDest, T *pSrc, I nCount )
{
	if constexpr ( IsTriviallyRelocatable<T>::value )
	{
		V_memmove( pDest, pSrc, nCount * sizeof( T ) );
	}
	else
	{
		for ( I i = 0; i < nCount; i++ )
		{
			MoveConstruct( &pDest[ i ], std::move( pSrc[ i ] ) );
			Destruct( &pSrc[ i ] );
		}
	}
}

template< class A, class T, class I >
inline T *ReallocLeanVectorElements( T *pMemory, I nCount, I &nAllocated )
{
	if constexpr ( IsTriviallyRelocatable<T>::value )
	{
		return A::Realloc( pMemory, nAllocated, nAllocated );
	}
	else
	{
		T *pNew = A::template Alloc<T>( nAllocated, nAllocated );
		RelocateLeanVectorElements( pNew, pMemory, nCount );
		A::Free( pMemory );
		return pNew;
	}
}

template< class T, class I, class A >
class CUtlLeanVectorBase
{
	typedef A CAllocator;

public:
	enum : std::make_unsigned_t<I>
	{
		EXTERNAL_BUFFER_MARKER = std::make_unsigned_t<I> { 1 } << ((sizeof( I ) * 8) - 1)
	};

	// constructor, destructor
	CUtlLeanVectorBase( I growSize = 0, I initSize = 0 );
	CUtlLeanVectorBase( T *pMemory, I allocationCount, I numElements = 0 );
	~CUtlLeanVectorBase();
	
	// Gets the base address (can change when adding elements!)
	T* Base();
	const T* Base() const;

	void Swap( CUtlLeanVectorBase<T, I, A> &mem );

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity( I num, bool force = false );
	
	bool IsExternallyAllocated() const { return (m_nAllocated & EXTERNAL_BUFFER_MARKER) != 0; }

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T *pMemory, I allocationCount, I numElements = 0 );
	void SetExternalBuffer( const T *pMemory, I allocationCount, I numElements = 0 );
	void AssumeMemory( T *pMemory, I allocationCount, I numElements = 0 );
	T *Detach();
	void *DetachMemory();

	I NumAllocated() const { return (m_nAllocated & (~EXTERNAL_BUFFER_MARKER)); }

	// Element removal
	void RemoveAll(); // doesn't deallocate memory

	bool IsIdxValid( I i ) const { return (i >= 0) && (i < m_nCount); }
	
	// Memory deallocation
	void Purge();
protected:

	struct
	{
		I m_nCount;
		I m_nAllocated;
		T* m_pElements;
	};
};

template< class T, class I, class A >
inline CUtlLeanVectorBase<T, I, A>::CUtlLeanVectorBase( I growSize, I initSize ) :
	m_nCount( 0 ), m_nAllocated( 0 ), m_pElements( nullptr )
{
	EnsureCapacity( initSize );
}
	
template< class T, class I, class A >
inline CUtlLeanVectorBase<T, I, A>::CUtlLeanVectorBase( T *pMemory, I allocationCount, I numElements ) :
	m_nCount( numElements ), m_nAllocated( allocationCount | EXTERNAL_BUFFER_MARKER ), m_pElements( pMemory )
{
}

template< class T, class I, class A >
inline CUtlLeanVectorBase<T, I, A>::~CUtlLeanVectorBase()
{
	Purge();
}

template< class T, class I, class A >
inline T* CUtlLeanVectorBase<T, I, A>::Base()
{
	return NumAllocated() ? m_pElements : nullptr;
}

template< class T, class I, class A >
inline const T* CUtlLeanVectorBase<T, I, A>::Base() const
{
	return NumAllocated() ? m_pElements : nullptr;
}

template< class T, class I, class A >
void CUtlLeanVectorBase<T, I, A>::Swap( CUtlLeanVectorBase<T, I, A> &vec )
{
	V_swap( m_nCount, vec.m_nCount );
	V_swap( m_nAllocated, vec.m_nAllocated );
	V_swap( m_pElements, vec.m_pElements );
}

template< class T, class I, class A >
inline void CUtlLeanVectorBase<T, I, A>::SetExternalBuffer( T *pMemory, I allocationCount, I numElements )
{
	Purge();

	m_nCount = numElements;
	m_nAllocated = allocationCount | EXTERNAL_BUFFER_MARKER;
	m_pElements = pMemory;
}

template< class T, class I, class A >
inline void CUtlLeanVectorBase<T, I, A>::SetExternalBuffer( const T *pMemory, I allocationCount, I numElements )
{
	Purge();

	m_nCount = numElements;
	m_nAllocated = allocationCount | EXTERNAL_BUFFER_MARKER;
	m_pElements = const_cast<T *>(pMemory);
}

template< class T, class I, class A >
inline void CUtlLeanVectorBase<T, I, A>::AssumeMemory( T *pMemory, I allocationCount, I numElements )
{
	// Blow away any existing allocated memory
	Purge();

	m_nCount = numElements;
	m_nAllocated = allocationCount;

	// Simply take the pointer but don't mark us as external
	m_pElements = pMemory;
}

template< class T, class I, class A >
inline T *CUtlLeanVectorBase<T, I, A>::Detach()
{
	return (T *)DetachMemory();
}

template< class T, class I, class A >
inline void *CUtlLeanVectorBase<T, I, A>::DetachMemory()
{
	if(IsExternallyAllocated())
		return nullptr;

	void *pMemory = m_pElements;
	m_nCount = 0;
	m_nAllocated = 0;

	m_pElements = 0;
	return pMemory;
}

template< class T, class I, class A >
void CUtlLeanVectorBase<T, I, A>::EnsureCapacity( I num, bool force )
{
	if(num <= NumAllocated())
		return;
	
	I nMinAllocated = (31 + sizeof( T )) / sizeof( T );
	I nMaxAllocated = (std::numeric_limits<std::make_signed_t<I>>::max)();

	if ( (uint64_t)num > (uint64_t)nMaxAllocated )
	{
		Msg( "%s allocation count overflow( %llu > %llu )\n", __FUNCTION__, ( uint64_t )num, ( uint64_t )nMaxAllocated );
		Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
		DebuggerBreak();
	}
	
	I nNewAllocated = num;
	if ( !force )
		nNewAllocated = CalcNewDoublingCount( NumAllocated(), num, nMinAllocated, nMaxAllocated );
	
	T *pNew = nullptr;
	if(IsExternallyAllocated())
	{
		pNew = CAllocator::template Alloc<T>( nNewAllocated, nNewAllocated );
		RelocateLeanVectorElements( pNew, Base(), m_nCount );
	}
	else
	{
		pNew = ReallocLeanVectorElements<CAllocator>( m_pElements, m_nCount, nNewAllocated );
	}

	m_pElements = pNew;
	m_nAllocated = MIN( nNewAllocated, nMaxAllocated );
}

template< class T, class I, class A >
void CUtlLeanVectorBase<T, I, A>::RemoveAll()
{
	// External memory belongs to its owner and may be read-only, so it gets no debug fill
	const size_t nFillSize = IsExternallyAllocated() ? 0 : sizeof( T );

	T* pElement = Base();
	const T* pEnd = &pElement[ m_nCount ];
	while ( pElement != pEnd )
		Destruct( pElement++, nFillSize );

	m_nCount = 0;
}

template< class T, class I, class A >
inline void CUtlLeanVectorBase<T, I, A>::Purge()
{
	RemoveAll();

	if(NumAllocated() > 0)
	{
		if(!IsExternallyAllocated())
			CAllocator::Free( m_pElements );

		m_pElements = nullptr;
	}

	m_nAllocated = 0;
}

template< class T, size_t N, class I, class A >
class CUtlLeanVectorFixedGrowableBase
{
	typedef A CAllocator;

public:
	enum : std::make_unsigned_t<I>
	{
		EXTERNAL_BUFFER_MARKER = std::make_unsigned_t<I> { 1 } << ((sizeof( I ) * 8) - 1)
	};

	// constructor, destructor
	CUtlLeanVectorFixedGrowableBase( I growSize = 0, I initSize = 0 );
	~CUtlLeanVectorFixedGrowableBase();
	
	// Gets the base address (can change when adding elements!)
	T* Base();
	const T* Base() const;

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity( I num, bool force = false );

	// AMNOTE: The top bit is masked off like in CUtlLeanVectorBase, but never set or checked, so external buffers aren't supported
	I NumAllocated() const { return (m_nAllocated & (~EXTERNAL_BUFFER_MARKER)); }

	// Element removal
	void RemoveAll(); // doesn't deallocate memory
	
	bool IsIdxValid( I i ) const { return (i >= 0) && (i < m_nCount); }

	// Memory deallocation
	void Purge();

protected:

	struct ALIGN8 FixedLayout_t
	{
		I m_nCount;
		I m_nAllocated;
		T m_Elements[ N ];
	} ALIGN8_POST;

	// AMNOTE: Ensures all the added padding is also utilized for static storage if possible
	static constexpr size_t FIXED_CAPACITY = ( sizeof( FixedLayout_t ) - offsetof( FixedLayout_t, m_Elements ) ) / sizeof( T );

	union
	{
		struct
		{
			I m_nCount;
			I m_nAllocated;
		};
		
		struct
		{
			I m_nFixedCount;
			I m_nFixedAllocated;
			T m_FixedAlloc[ FIXED_CAPACITY ];
		};
		
		struct
		{
			I m_nAllocCount;
			I m_nAllocAllocated;
			T* m_pElements;
		};
	};
};

template< class T, size_t N, class I, class A >
inline CUtlLeanVectorFixedGrowableBase<T, N, I, A>::CUtlLeanVectorFixedGrowableBase( I growSize, I initSize ) :
	m_nCount( 0 ), m_nAllocated( FIXED_CAPACITY )
{
	EnsureCapacity( initSize );
}

template< class T, size_t N, class I, class A >
inline CUtlLeanVectorFixedGrowableBase<T, N, I, A>::~CUtlLeanVectorFixedGrowableBase()
{
	Purge();
}

template< class T, size_t N, class I, class A >
inline T* CUtlLeanVectorFixedGrowableBase<T, N, I, A>::Base()
{
	if ( NumAllocated() )
	{
		if ( ( size_t )NumAllocated() > FIXED_CAPACITY )
			return m_pElements;
		else
			return &m_FixedAlloc[ 0 ];
	}
	
	return nullptr;
}

template< class T, size_t N, class I, class A >
inline const T* CUtlLeanVectorFixedGrowableBase<T, N, I, A>::Base() const
{
	if ( NumAllocated() )
	{
		if ( ( size_t )NumAllocated() > FIXED_CAPACITY )
			return m_pElements;
		else
			return &m_FixedAlloc[ 0 ];
	}
	
	return nullptr;
}

template< class T, size_t N, class I, class A >
void CUtlLeanVectorFixedGrowableBase<T, N, I, A>::EnsureCapacity( I num, bool force )
{
	if ( num <= NumAllocated() )
		return;

	if((size_t)num <= FIXED_CAPACITY)
	{
		m_nAllocated = num;
		return;
	}
	
	I nMinAllocated = (31 + sizeof( T )) / sizeof( T );
	I nMaxAllocated = (std::numeric_limits<std::make_signed_t<I>>::max)();
	I nNewAllocated = num;

	if((size_t)num > (size_t)nMaxAllocated)
	{
		Msg( "%s allocation count overflow( %llu > %llu )\n", __FUNCTION__, (size_t)num, (size_t)nMaxAllocated );
		Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
		DebuggerBreak();
	}

	if(!force)
		nNewAllocated = CalcNewDoublingCount( NumAllocated(), num, nMinAllocated, nMaxAllocated );
	
	T *pNew = nullptr;
	if((size_t)NumAllocated() > FIXED_CAPACITY)
	{
		pNew = ReallocLeanVectorElements<CAllocator>( m_pElements, m_nCount, nNewAllocated );
	}
	else
	{
		pNew = CAllocator::template Alloc<T>( nNewAllocated, nNewAllocated );
		RelocateLeanVectorElements( pNew, Base(), m_nCount );
	}
	
	m_pElements = pNew;
	m_nAllocated = MIN( nNewAllocated, nMaxAllocated );
}

template< class T, size_t N, class I, class A >
void CUtlLeanVectorFixedGrowableBase<T, N, I, A>::RemoveAll()
{
	T* pElement = Base();
	const T* pEnd = &pElement[ m_nCount ];
	while ( pElement != pEnd )
		Destruct( pElement++ );

	m_nCount = 0;
}

template< class T, size_t N, class I, class A >
inline void CUtlLeanVectorFixedGrowableBase<T, N, I, A>::Purge()
{
	RemoveAll();

	if((size_t)NumAllocated() > FIXED_CAPACITY)
		CAllocator::Free( m_pElements );

	m_nAllocated = FIXED_CAPACITY;
}

template< class B, class T, class I >
class CUtlLeanVectorImpl : public B
{
	typedef B BaseClass;
public:
	using BaseClass::BaseClass;
	~CUtlLeanVectorImpl() {};

	// Copy the array.
	CUtlLeanVectorImpl<B, T, I>& operator=( const CUtlLeanVectorImpl<B, T, I> &other );

	class Iterator_t
	{
	public:
		Iterator_t( I i ) : index( i ) {}
		I index;

		bool operator==( const Iterator_t it ) const { return index == it.index; }
		bool operator!=( const Iterator_t it ) const { return index != it.index; }
	};
	Iterator_t First() const { return Iterator_t( this->IsIdxValid( 0 ) ? 0 : InvalidIndex() ); }
	Iterator_t Next( const Iterator_t &it ) const { return Iterator_t( this->IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex() ); }
	I GetIndex( const Iterator_t &it ) const { return it.index; }
	bool IsIdxAfter( I i, const Iterator_t &it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t &it ) const { return this->IsIdxValid( it.index ); }
	Iterator_t InvalidIterator() const { return Iterator_t( InvalidIndex() ); }

	T &operator[]( const Iterator_t &it ) { return Element( it.index ); }
	const T &operator[]( const Iterator_t &it ) const { return Element( it.index ); }

	// element access
	T& operator[]( I i );
	const T& operator[]( I i ) const;
	T& Element( I i );
	const T& Element( I i ) const;
	T& Head();
	const T& Head() const;
	T& Tail();
	const T& Tail() const;
	
	// STL compatible iteration, for range-based for loops and std algorithms.
	typedef T* iterator;
	typedef const T* const_iterator;
	iterator begin()						{ return this->Base(); }
	const_iterator begin() const			{ return this->Base(); }
	iterator end()							{ return this->Base() + Count(); }
	const_iterator end() const				{ return this->Base() + Count(); }

	// Returns the number of elements in the vector
	I Count() const;

	// Is element index valid?
	bool IsValidIndex( I i ) const;

	// Specify the invalid ('null') index that we'll only return on failure
	static const I INVALID_INDEX = (I)-1; // For use with static_assert
	static I InvalidIndex() { return INVALID_INDEX; }

	// Adds an element, uses default constructor
	T* AddToTailGetPtr();

	// Adds an element, uses copy constructor
	I AddToTail();
	I AddToTail( const T& src );

	// Adds multiple elements, uses default constructor
	I AddMultipleToTail( I nSize );

	// Adds multiple elements, uses default constructor
	T* InsertBeforeGetPtr( I nBeforeIndex, I nSize = 1 );

	void SetSize( I size );
	void SetCount( I count );

	void EnsureCount( I num );

	// Finds an element (element needs operator== defined)
	I Find( const T& src ) const;

	// Element removal
	void FastRemove( I elem ); // doesn't preserve order
	void Remove( I elem ); // preserves order, shifts elements
	bool FindAndRemove( const T& src );	// removes first occurrence of src, preserves order, shifts elements
	bool FindAndFastRemove( const T& src );	// removes first occurrence of src, doesn't preserve order
	void RemoveMultiple( I elem, I num ); // preserves order, shifts elements
	void RemoveMultipleFromHead( I num ); // preserves order, shifts elements
	void RemoveMultipleFromTail( I num ); // preserves order, shifts elements

protected:
	// Can't copy this unless we explicitly do it!
	CUtlLeanVectorImpl( CUtlLeanVectorImpl const& vec ) = delete;

	// Shifts elements....
	void ShiftElements( T* pDest, const T* pSrc, const T* pSrcEnd );

	// construct, destruct elements
	void ConstructElements( T* pElement, const T* pEnd );
	void DestructElements( T* pElement, const T* pEnd );
};

template< class B, class T, class I >
inline CUtlLeanVectorImpl<B, T, I>& CUtlLeanVectorImpl<B, T, I>::operator=( const CUtlLeanVectorImpl<B, T, I> &other )
{
	I nCount = other.Count();
	SetSize( nCount );

	T* pDest = this->Base();
	const T* pSrc = other.Base();
	const T* pEnd = &pSrc[ nCount ];

	while ( pSrc != pEnd )
		*(pDest++) = *(pSrc++);

	return *this;
}

template< class B, class T, class I >
inline T& CUtlLeanVectorImpl<B, T, I>::operator[]( I i )
{
	Assert( i < Count() );
	return this->Base()[ i ];
}

template< class B, class T, class I >
inline const T& CUtlLeanVectorImpl<B, T, I>::operator[]( I i ) const
{
	Assert( i < Count() );
	return this->Base()[ i ];
}

template< class B, class T, class I >
inline T& CUtlLeanVectorImpl<B, T, I>::Element( I i )
{
	Assert( i < Count() );
	return this->Base()[ i ];
}

template< class B, class T, class I >
inline const T& CUtlLeanVectorImpl<B, T, I>::Element( I i ) const
{
	Assert( i < Count() );
	return this->Base()[ i ];
}

template< class B, class T, class I >
inline T& CUtlLeanVectorImpl<B, T, I>::Head()
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ 0 ];
}

template< class B, class T, class I >
inline const T& CUtlLeanVectorImpl<B, T, I>::Head() const
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ 0 ];
}

template< class B, class T, class I >
inline T& CUtlLeanVectorImpl<B, T, I>::Tail()
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ this->m_nCount - 1 ];
}

template< class B, class T, class I >
inline const T& CUtlLeanVectorImpl<B, T, I>::Tail() const
{
	Assert( this->m_nCount > 0 );
	return this->Base()[ this->m_nCount - 1 ];
}

template< class B, class T, class I >
inline I CUtlLeanVectorImpl<B, T, I>::Count() const
{
	return this->m_nCount;
}

template< class B, class T, class I >
inline bool CUtlLeanVectorImpl<B, T, I>::IsValidIndex( I i ) const
{
	return (i >= 0) && (i < Count());
}

template< class B, class T, class I >
T* CUtlLeanVectorImpl<B, T, I>::AddToTailGetPtr()
{
	this->EnsureCapacity( this->m_nCount + 1 );
	T* pBase = this->Base();
	Construct( &pBase[ this->m_nCount ] );
	return &pBase[ this->m_nCount++ ];
}

template< class B, class T, class I >
I CUtlLeanVectorImpl<B, T, I>::AddToTail()
{
	this->EnsureCapacity( this->m_nCount + 1 );
	T* pBase = this->Base();
	Construct( &pBase[ this->m_nCount ] );
	return this->m_nCount++;
}

template< class B, class T, class I >
I CUtlLeanVectorImpl<B, T, I>::AddToTail( const T& src )
{
	this->EnsureCapacity( this->m_nCount + 1 );
	T* pBase = this->Base();
	CopyConstruct( &pBase[ this->m_nCount ], src );
	return this->m_nCount++;
}

template< class B, class T, class I >
I CUtlLeanVectorImpl<B, T, I>::AddMultipleToTail( I nSize )
{
	I nOldSize = this->m_nCount;

	if ( nSize > 0 )
	{
		I nMaxSize = (std::numeric_limits<std::make_signed_t<I>>::max)();

		if ( ( nMaxSize - nOldSize ) < nSize )
		{
			Msg( "%s allocation count overflow( add %llu + current %llu > max %llu )\n", __FUNCTION__, ( uint64_t )nSize, ( uint64_t )nOldSize, ( uint64_t )nMaxSize );
			Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
			DebuggerBreak();
		}

		I nNewSize = nOldSize + nSize;

		this->EnsureCapacity( nNewSize );

		T* pBase = this->Base();

		ConstructElements( &pBase[ nOldSize ], &pBase[ nNewSize ] );

		this->m_nCount = nNewSize;
	}

	return nOldSize;
}

template< class B, class T, class I >
T* CUtlLeanVectorImpl<B, T, I>::InsertBeforeGetPtr( I nBeforeIndex, I nSize )
{
	I nOldSize = this->m_nCount;

	// Unsigned, so a negative index fails too
	if ( ( std::make_unsigned_t<I> )nBeforeIndex > ( std::make_unsigned_t<I> )nOldSize )
	{
		Plat_FatalError( "%s: invalid nBeforeIndex %lld\n", __FUNCTION__, ( int64_t )nBeforeIndex );
		DebuggerBreak();
	}

	if ( nSize <= 0 )
	{
		Plat_FatalError( "%s: invalid nSize %lld\n", __FUNCTION__, ( int64_t )nSize );
		DebuggerBreak();
	}

	I nMaxSize = (std::numeric_limits<std::make_signed_t<I>>::max)();

	if ( ( nMaxSize - nOldSize ) < nSize )
	{
		Msg( "%s allocation count overflow( add %llu + current %llu > max %llu )\n", __FUNCTION__, ( uint64_t )nSize, ( uint64_t )nOldSize, ( uint64_t )nMaxSize );
		Plat_FatalError( "%s allocation count overflow", __FUNCTION__ );
		DebuggerBreak();
	}

	I nNewSize = nOldSize + nSize;

	this->EnsureCapacity( nNewSize );

	T* pBase = this->Base();

	ShiftElements( &pBase[ nBeforeIndex + nSize ], &pBase[ nBeforeIndex ], &pBase[ nOldSize ] );
	ConstructElements( &pBase[ nBeforeIndex ], &pBase[ nBeforeIndex + nSize ] );

	this->m_nCount = nNewSize;

	return &pBase[ nBeforeIndex ];
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::SetCount( I count )
{
	this->EnsureCapacity( count );

	T* pBase = this->Base();

	if ( Count() < count )
		ConstructElements( &pBase[ this->m_nCount ], &pBase[ count ] );
	else if ( Count() > count )
		DestructElements( &pBase[ count ], &pBase[ this->m_nCount ] );

	this->m_nCount = count;
}

template< class B, class T, class I >
inline void CUtlLeanVectorImpl<B, T, I>::SetSize( I size )
{
	SetCount( size );
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::EnsureCount( I num )
{
	if(Count() < num)
	{
		AddMultipleToTail( num - Count() );
	}
}

template< class B, class T, class I >
I CUtlLeanVectorImpl<B, T, I>::Find( const T& src ) const
{
	const T* pBase = this->Base();
	for ( I i = 0; i < Count(); ++i )
	{
		if ( pBase[i] == src )
			return i;
	}
	return InvalidIndex();
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::FastRemove( I elem )
{
	Assert( IsValidIndex(elem) );

	T* pBase = this->Base();
	Destruct( &pBase[ elem ] );
	if ( this->m_nCount > 0 )
	{
		if ( elem != Count() - 1 )
			V_memmove( &pBase[ elem ], &pBase[ this->m_nCount - 1 ], sizeof( T ) );
		--this->m_nCount;
	}
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::Remove( I elem )
{
	T* pBase = this->Base();
	Destruct( &pBase[ elem ] );
	ShiftElements( &pBase[ elem ], &pBase[ elem + 1 ], &pBase[ this->m_nCount ] );
	--this->m_nCount;
}

template< class B, class T, class I >
bool CUtlLeanVectorImpl<B, T, I>::FindAndRemove( const T& src )
{
	I elem = Find( src );
	if ( elem != InvalidIndex() )
	{
		Remove( elem );
		return true;
	}
	return false;
}

template< class B, class T, class I >
bool CUtlLeanVectorImpl<B, T, I>::FindAndFastRemove( const T& src )
{
	I elem = Find( src );
	if ( elem != InvalidIndex() )
	{
		FastRemove( elem );
		return true;
	}
	return false;
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::RemoveMultiple( I elem, I num )
{
	Assert( elem >= 0 );
	Assert( elem + num <= Count() );

	T* pBase = this->Base();
	DestructElements( &pBase[ elem ], &pBase[ elem + num ] );
	ShiftElements( &pBase[ elem ], &pBase[ elem + num ], &pBase[ this->m_nCount ] );
	this->m_nCount -= num;
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::RemoveMultipleFromHead( I num )
{
	Assert( num <= Count() );
	RemoveMultiple( 0, num );
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::RemoveMultipleFromTail( I num )
{
	Assert( num <= Count() );
	RemoveMultiple( Count() - num, num );
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::ShiftElements( T* pDest, const T* pSrc, const T* pSrcEnd )
{
	ptrdiff_t numToMove = pSrcEnd - pSrc;
	if ( numToMove > 0 )
		V_memmove( pDest, pSrc, numToMove * sizeof( T ) );
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::ConstructElements( T* pElement, const T* pEnd )
{
	while ( pElement < pEnd )
		Construct( pElement++ );
}

template< class B, class T, class I >
void CUtlLeanVectorImpl<B, T, I>::DestructElements( T* pElement, const T* pEnd )
{
	while ( pElement < pEnd )
		Destruct( pElement++ );
}

template < class T, class I = int, class A = CMemAllocAllocator >
class CUtlLeanVector : public CUtlLeanVectorImpl< CUtlLeanVectorBase< T, I, A >, T, I >
{
	typedef CUtlLeanVectorImpl< CUtlLeanVectorBase< T, I, A >, T, I > BaseClass;
public:

	using BaseClass::BaseClass;
};

template < class T, size_t N = 3, class I = int, class A = CMemAllocAllocator >
class CUtlLeanVectorFixedGrowable : public CUtlLeanVectorImpl< CUtlLeanVectorFixedGrowableBase< T, N, I, A >, T, I >
{
	typedef CUtlLeanVectorImpl< CUtlLeanVectorFixedGrowableBase< T, N, I, A >, T, I > BaseClass;
public:

	using BaseClass::BaseClass;
};

#endif