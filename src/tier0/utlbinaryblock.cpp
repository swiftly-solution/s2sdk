#include "public/tier0/utlbinaryblock.h"

CUtlBinaryBlock::CUtlBinaryBlock( int growSize, int initSize )
: m_Memory( growSize, initSize )
{}

CUtlBinaryBlock::CUtlBinaryBlock( void* pMemory, int nSizeInBytes, int nInitialLength )
: m_Memory( (uint8 *)pMemory, nSizeInBytes, nInitialLength )
{}

CUtlBinaryBlock::CUtlBinaryBlock( const CUtlBinaryBlock& src )
{ *this = src; }

const void * CUtlBinaryBlock::Get( ) const
{
	return m_Memory.Base();
}

void * CUtlBinaryBlock::Get( )
{
	return m_Memory.Base();
}

int CUtlBinaryBlock::Length() const
{
	return m_Memory.Count();
}

unsigned char& CUtlBinaryBlock::operator[]( int i )
{
	return m_Memory[i];
}

const unsigned char& CUtlBinaryBlock::operator[]( int i ) const
{
	return m_Memory[i];
}

bool CUtlBinaryBlock::IsEmpty() const
{
	return Length() == 0;
}

void CUtlBinaryBlock::Clear()
{
	SetLength( 0 );
}

void CUtlBinaryBlock::Purge()
{
	SetLength( 0 );
	m_Memory.Purge();
}
