#include "public/tier0/bitvec.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "public/tier0/memdbgon.h"

int FirstBitInWord( unsigned int elem, int offset )
{
#if _WIN32
	if ( !elem )
		return -1;
	unsigned long out;
	_BitScanForward(&out, elem);
	return out + offset;

#else
	static uint32_t firstBitLUT[256] = 
	{
		0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,
		3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
		4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,
		3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
		5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,
		3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
		4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0
	};
	uint32_t elemByte;

	elemByte = (elem & 0xFF);
	if ( elemByte )
		return offset + firstBitLUT[elemByte];

	elem >>= 8;
	offset += 8;
	elemByte = (elem & 0xFF);
	if ( elemByte )
		return offset + firstBitLUT[elemByte];

	elem >>= 8;
	offset += 8;
	elemByte = (elem & 0xFF);
	if ( elemByte )
		return offset + firstBitLUT[elemByte];

	elem >>= 8;
	offset += 8;
	elemByte = (elem & 0xFF);
	if ( elemByte )
		return offset + firstBitLUT[elemByte];

	return -1;
#endif
}

uint32_t GetEndMask( int numBits ) 
{ 
	static uint32_t bitStringEndMasks[] = 
	{
		0xffffffff,
		0x00000001,
		0x00000003,
		0x00000007,
		0x0000000f,
		0x0000001f,
		0x0000003f,
		0x0000007f,
		0x000000ff,
		0x000001ff,
		0x000003ff,
		0x000007ff,
		0x00000fff,
		0x00001fff,
		0x00003fff,
		0x00007fff,
		0x0000ffff,
		0x0001ffff,
		0x0003ffff,
		0x0007ffff,
		0x000fffff,
		0x001fffff,
		0x003fffff,
		0x007fffff,
		0x00ffffff,
		0x01ffffff,
		0x03ffffff,
		0x07ffffff,
		0x0fffffff,
		0x1fffffff,
		0x3fffffff,
		0x7fffffff,
	};

	return bitStringEndMasks[numBits % BITS_PER_INT]; 
}


int GetBitForBitnum( int bitNum ) 
{ 
	static int bitsForBitnum[] = 
	{
		( 1 << 0 ),
		( 1 << 1 ),
		( 1 << 2 ),
		( 1 << 3 ),
		( 1 << 4 ),
		( 1 << 5 ),
		( 1 << 6 ),
		( 1 << 7 ),
		( 1 << 8 ),
		( 1 << 9 ),
		( 1 << 10 ),
		( 1 << 11 ),
		( 1 << 12 ),
		( 1 << 13 ),
		( 1 << 14 ),
		( 1 << 15 ),
		( 1 << 16 ),
		( 1 << 17 ),
		( 1 << 18 ),
		( 1 << 19 ),
		( 1 << 20 ),
		( 1 << 21 ),
		( 1 << 22 ),
		( 1 << 23 ),
		( 1 << 24 ),
		( 1 << 25 ),
		( 1 << 26 ),
		( 1 << 27 ),
		( 1 << 28 ),
		( 1 << 29 ),
		( 1 << 30 ),
		( 1 << 31 ),
	};

	return bitsForBitnum[ (bitNum) & (BITS_PER_INT-1) ]; 
}

int GetBitForBitnumByte( int bitNum ) 
{ 
	static int bitsForBitnum[] = 
	{
		( 1 << 0 ),
		( 1 << 1 ),
		( 1 << 2 ),
		( 1 << 3 ),
		( 1 << 4 ),
		( 1 << 5 ),
		( 1 << 6 ),
		( 1 << 7 ),
	};

	return bitsForBitnum[ bitNum & 7 ]; 
}

int CalcNumIntsForBits( int numBits )	{ return (numBits + (BITS_PER_INT-1)) / BITS_PER_INT; }

CVarBitVecBase::CVarBitVecBase()
{
	memset( (void *)this, 0, sizeof( *this ) );
}

CVarBitVecBase::CVarBitVecBase(int numBits)
{
	Assert( numBits );
	m_numBits	= numBits;

	// Figure out how many ints are needed
	m_numInts = CalcNumIntsForBits( numBits );
	m_pInt = NULL;
	AllocInts( m_numInts );
}

CVarBitVecBase::CVarBitVecBase( const CVarBitVecBase &from )
{
	if ( from.m_numInts )
	{
		m_numBits = from.m_numBits;
		m_numInts = from.m_numInts;
		m_pInt = NULL;
		AllocInts( m_numInts );
		memcpy( m_pInt, from.m_pInt, m_numInts * sizeof(int) );
	}
	else
		memset( (void *)this, 0, sizeof( *this ) );
}

CVarBitVecBase &CVarBitVecBase::operator=( const CVarBitVecBase &from )
{
	Resize( from.GetNumBits() );
	if ( m_pInt )
		memcpy( m_pInt, from.m_pInt, m_numInts * sizeof(int) );
	return (*this);
}

CVarBitVecBase::~CVarBitVecBase(void)
{
	FreeInts();
}

void CVarBitVecBase::Attach( uint32_t *pBits, int numBits )
{
	FreeInts();
	m_numBits = numBits;
	m_numInts = CalcNumIntsForBits( numBits );
	if ( m_numInts > 1 )
	{
		m_pInt = pBits;
	}
	else
	{
		m_iBitStringStorage = *pBits;
		m_pInt = &m_iBitStringStorage;
		free( pBits );
	}
}

bool CVarBitVecBase::Detach( uint32_t **ppBits, int *pNumBits )
{
	if ( !m_numBits )
	{
		return false;
	}

	*pNumBits = m_numBits;
	if ( m_numInts > 1 )
	{
		*ppBits = m_pInt;
	}
	else
	{
		*ppBits = (uint32_t *)malloc( sizeof(uint32_t) );
		**ppBits = m_iBitStringStorage;
		free( m_pInt );
	}

	memset( (void *)this, 0, sizeof( *this ) );
	return true;
}

uint32_t GetStartBitMask( int startBit )
{
	static uint32_t g_StartMask[32] =
	{
		0xffffffff,
		0xfffffffe,
		0xfffffffc,
		0xfffffff8,
		0xfffffff0,
		0xffffffe0,
		0xffffffc0,
		0xffffff80,
		0xffffff00,
		0xfffffe00,
		0xfffffc00,
		0xfffff800,
		0xfffff000,
		0xffffe000,
		0xffffc000,
		0xffff8000,
		0xffff0000,
		0xfffe0000,
		0xfffc0000,
		0xfff80000,
		0xfff00000,
		0xffe00000,
		0xffc00000,
		0xff800000,
		0xff000000,
		0xfe000000,
		0xfc000000,
		0xf8000000,
		0xf0000000,
		0xe0000000,
		0xc0000000,
		0x80000000,
	};

	return g_StartMask[ startBit & 31 ];
}

int CVarBitVecBase::FindNextSetBit( int startBit ) const
{
	if ( startBit < GetNumBits() )
	{
		int wordIndex = BitVec_Int(startBit);
		unsigned int startMask = GetStartBitMask( startBit );
		int lastWord = GetNumDWords()-1;

		// handle non dword lengths
		if ( (GetNumBits() % BITS_PER_INT) != 0 )
		{
			unsigned int elem = Base()[wordIndex];
			elem &= startMask;
			if ( wordIndex == lastWord)
			{
				elem &= (GetEndMask());
				// there's a bit remaining in this word
				if ( elem )
					return FirstBitInWord(elem, wordIndex << 5);
			}
			else
			{
				// there's a bit remaining in this word
				if ( elem )
					return FirstBitInWord(elem, wordIndex << 5);

				// iterate the words
				for ( int i = wordIndex+1; i < lastWord; i++ )
				{
					elem = Base()[i];
					if ( elem )
						return FirstBitInWord(elem, i << 5);
				}
				elem = Base()[lastWord] & GetEndMask();
				if ( elem )
					return FirstBitInWord(elem, lastWord << 5);
			}
		}
		else
		{
			const uint32_t * pCurElem = Base() + wordIndex;
			unsigned int elem = *pCurElem;
			elem &= startMask;
			do 
			{
				if ( elem )
					return FirstBitInWord(elem, wordIndex << 5);
				++pCurElem;
				elem = *pCurElem;
				++wordIndex;
			} while( wordIndex <= lastWord );
		}

	}

	return -1;
}

#include "public/tier0/memdbgon.h"

void CVarBitVecBase::Resize( int resizeNumBits, bool bClearAll )
{
	Assert( resizeNumBits >= 0 && resizeNumBits <= USHRT_MAX );

	int newIntCount = CalcNumIntsForBits( resizeNumBits );
	if ( newIntCount != GetNumDWords() )
	{
		if ( Base() )
		{
			ReallocInts( newIntCount );
			if ( !bClearAll && resizeNumBits >= GetNumBits() )
			{
				Base()[GetNumDWords() - 1] &= GetEndMask();
				memset( Base() + GetNumDWords(), 0, (newIntCount - GetNumDWords()) * sizeof(int) );
			}
		}
		else
		{
			// Figure out how many ints are needed
			AllocInts( newIntCount );
			// Initialize bitstring by clearing all bits
			bClearAll = true;
		}

		m_numInts = newIntCount;
	} 
	else if ( !bClearAll && resizeNumBits >= GetNumBits() && Base() )
	{
		Base()[GetNumDWords() - 1] &= GetEndMask();
	}

	if ( bClearAll && Base() )
	{
		memset( Base(), 0, newIntCount * sizeof(int) );
	}

	// store the new size and end mask
	m_numBits = resizeNumBits;
}

void CVarBitVecBase::AllocInts( int numInts )
{
	Assert( !m_pInt );

	if ( numInts == 0 )
		return;

	if ( numInts == 1 )
	{
		m_pInt = &m_iBitStringStorage;
		return;
	}

	m_pInt = (uint32_t *)malloc( numInts * sizeof(int) );
}

void CVarBitVecBase::ReallocInts( int numInts )
{
	Assert( Base() );
	if ( numInts == 0)
	{
		FreeInts();
		return;
	}

	if ( m_pInt == &m_iBitStringStorage )
	{
		if ( numInts != 1 )
		{
			m_pInt = ((uint32_t *)malloc( numInts * sizeof(int) ));
			*m_pInt = m_iBitStringStorage;
		}

		return;
	}

	if ( numInts == 1 )
	{
		m_iBitStringStorage = *m_pInt;
		free( m_pInt );
		m_pInt = &m_iBitStringStorage;
		return;
	}

	m_pInt = (uint32_t *)realloc( m_pInt,  numInts * sizeof(int) );
}

void CVarBitVecBase::FreeInts( void )
{
	if ( m_numInts > 1 )
	{
		free( m_pInt );
	}
	m_pInt = NULL;
}

CBitVecAccessor::CBitVecAccessor(uint32_t *pDWords, int iBit)
{
	m_pDWords = pDWords;
	m_iBit = iBit;
}

void CBitVecAccessor::operator=(int val)
{
	if(val)
		m_pDWords[m_iBit >> 5] |= (1 << (m_iBit & 31));
	else
		m_pDWords[m_iBit >> 5] &= ~(unsigned long)(1 << (m_iBit & 31));
}

CBitVecAccessor::operator uint32_t()
{
	return m_pDWords[m_iBit >> 5] & (1 << (m_iBit & 31));
}

#include "public/tier0/memdbgoff.h"

bool CVarBitVecBase::IsFixedSize() const
{ return false; }

int CVarBitVecBase::GetNumBits(void) const
{ return m_numBits; }

int CVarBitVecBase::GetNumDWords() const
{ return m_numInts; }

uint32_t * CVarBitVecBase::Base()
{ return m_pInt;	}

const uint32_t * CVarBitVecBase::Base() const
{ return m_pInt;	}

void CVarBitVecBase::ValidateOperand( const CVarBitVecBase &operand ) const
{ Assert(GetNumBits() == operand.GetNumBits()); }

unsigned CVarBitVecBase::GetEndMask() const
{ return ::GetEndMask( GetNumBits() ); }

CVarBitVec::CVarBitVec()
{
}

CVarBitVec::CVarBitVec(int numBits)
: CBitVecT<CVarBitVecBase>(numBits)
{
}
