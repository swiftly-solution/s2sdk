#include "public/tier0/cbyteswap.h"

CByteswap::CByteswap()
{
	// Default behavior sets the target endian to match the machine native endian (no swap).
	SetTargetBigEndian( IsMachineBigEndian() );
}

bool CByteswap::IsMachineBigEndian()
{
	short nIsBigEndian = 1;

	// if we are big endian, the first byte will be a 0, if little endian, it will be a one.
	return (bool)(0 ==  *(char *)&nIsBigEndian );
}

void CByteswap::SetTargetBigEndian( bool bigEndian )
{
	m_bBigEndian = bigEndian;
	m_bSwapBytes = IsMachineBigEndian() != bigEndian;
}

void CByteswap::FlipTargetEndian( void )
{
	m_bSwapBytes = !m_bSwapBytes;
	m_bBigEndian = !m_bBigEndian;
}

void CByteswap::ActivateByteSwapping( bool bActivate )
{
	SetTargetBigEndian( IsMachineBigEndian() != bActivate );
}

bool CByteswap::IsSwappingBytes( void )
{
	return m_bSwapBytes;
}

bool CByteswap::IsTargetBigEndian( void )
{
	return m_bBigEndian;
}
