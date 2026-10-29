#include "public/tier0/bufferstring.h"

CBufferString::CBufferString( bool bAllowHeapAllocation )
:
	m_nLength( 0 ),
	m_nAllocatedSize( (bAllowHeapAllocation * ALLOW_HEAP_ALLOCATION) | STACK_ALLOCATED_MARKER | sizeof( m_pString ) ),
	m_pString( nullptr )
{ }

CBufferString::CBufferString( const char *pString, bool bAllowHeapAllocation )
:
	CBufferString( bAllowHeapAllocation )
{
	Insert( 0, pString );
}

CBufferString::CBufferString( size_t nAllocatedSize, bool bAllowHeapAllocation )
:
	m_nLength( 0 ),
	m_nAllocatedSize( (bAllowHeapAllocation * ALLOW_HEAP_ALLOCATION) | STACK_ALLOCATED_MARKER | (int)( nAllocatedSize + sizeof( m_pString ) ) ),
	m_pString( nullptr )
{ }

CBufferString::CBufferString( const CBufferString &other )
: CBufferString()
{ CopyFrom( other ); }

CBufferString::CBufferString( CBufferString &&other )
: CBufferString()
{ MoveFrom( other ); }

CBufferString::~CBufferString()
{ Purge(); }

void CBufferString::CopyFrom( const CBufferString &other )
{ CopyFrom( other.Get() ); }

void CBufferString::CopyFrom( const char *other )
{
	Clear();
	Insert( 0, other );
}

CBufferString & CBufferString::operator=( const CBufferString &src )
{
	if(this != &src)
		CopyFrom( src );

	return *this;
}

CBufferString & CBufferString::operator=( CBufferString &&src )
{
	if ( this != &src )
		MoveFrom( src );

	return *this;
}

CBufferString & CBufferString::operator=( const char *pString )
{
	CopyFrom( pString );
	return *this;
}

void CBufferString::SetHeapAllocationState( bool state )
{
	if(state)
		m_nAllocatedSize |= ALLOW_HEAP_ALLOCATION;
	else
		m_nAllocatedSize &= ~ALLOW_HEAP_ALLOCATION;
}

int CBufferString::AllocatedNum() const
{ return m_nAllocatedSize & LENGTH_MASK; }

int CBufferString::Length() const
{ return m_nLength & LENGTH_MASK; }

bool CBufferString::CanHeapAllocate() const
{ return (m_nAllocatedSize & ALLOW_HEAP_ALLOCATION) != 0; }

bool CBufferString::IsStackAllocated() const
{ return (m_nAllocatedSize & STACK_ALLOCATED_MARKER) != 0; }

bool CBufferString::ShouldFreeMemory() const
{ return (m_nLength & FREE_HEAP_MARKER) != 0; }

bool CBufferString::IsOverflowed() const
{ return (m_nLength & OVERFLOWED_MARKER) != 0; }

bool CBufferString::IsInputStringUnsafe( const char *pData ) const
{
	return ((void *)pData >= this && (void *)pData < &this[1]) ||
			(!IsAllocationEmpty() && pData >= Base() && pData < (Base() + AllocatedNum()));
}

bool CBufferString::IsAllocationEmpty() const
{ return AllocatedNum() == 0; }

char * CBufferString::Base()
{ return IsStackAllocated() ? reinterpret_cast<char *>( this ) + offsetof( CBufferString, m_pString ) : (!IsAllocationEmpty() ? m_pString : nullptr); }

const char * CBufferString::Base() const
{ return const_cast<CBufferString *>( this )->Base(); }

const char * CBufferString::Get() const
{ auto base = Base(); return base ? base : ""; }

void CBufferString::Clear()
{
	if(!IsAllocationEmpty())
		Base()[0] = '\0';

	m_nLength &= ~LENGTH_MASK;
}
