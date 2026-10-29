#include "public/tier0/bitbuf.h"
#include "public/common.h"

int BitByte( int bits )
{
    return (bits + 7) >> 3;
}

namespace bitbuf
{
    uint32_t ZigZagEncode32(int32_t n) 
	{
		return(n << 1) ^ (n >> 31);
	}

	int32_t ZigZagDecode32(uint32_t n) 
	{
		return(n >> 1) ^ -static_cast<int32_t>(n & 1);
	}

	uint64_t ZigZagEncode64(int64_t n) 
	{
		return(n << 1) ^ (n >> 63);
	}

	int64_t ZigZagDecode64(uint64_t n) 
	{
		return(n >> 1) ^ -static_cast<int64_t>(n & 1);
	}
}

int bf_write::GetNumBytesWritten() const	
{
	return BitByte(m_iCurBit);
}

int bf_write::GetNumBitsWritten() const	
{
	return m_iCurBit;
}

int bf_write::GetMaxNumBits()		
{
	return m_nDataBits;
}

int bf_write::GetNumBitsLeft()
{
	return m_nDataBits - m_iCurBit;
}

int bf_write::GetNumBytesLeft()	
{
	return GetNumBitsLeft() >> 3;
}

unsigned char* bf_write::GetData()			
{
	return (unsigned char*) m_pData;
}

const unsigned char* bf_write::GetData()	const
{
	return (unsigned char*) m_pData;
}

bool bf_write::CheckForOverflow(int nBits)
{
	if ( m_iCurBit + nBits > m_nDataBits )
	{
		SetOverflowFlag();
        DebuggerBreak();
	}
	
	return m_bOverflow;
}

void bf_write::SetOverflowFlag()
{
	m_bOverflow = true;
}

void bf_write::WriteOneBitNoCheck(int nValue)
{
	extern uint32_t g_LittleBits[32];
	if(nValue)
		m_pData[m_iCurBit >> 5] |= g_LittleBits[m_iCurBit & 31];
	else
		m_pData[m_iCurBit >> 5] &= ~g_LittleBits[m_iCurBit & 31];

	++m_iCurBit;
}

void bf_write::WriteOneBit(int nValue)
{
	if( m_iCurBit >= m_nDataBits )
	{
		SetOverflowFlag();
		DebuggerBreak();
		return;
	}
	WriteOneBitNoCheck( nValue );
}


void	bf_write::WriteOneBitAt( int iBit, int nValue )
{
	if( iBit >= m_nDataBits )
	{
		SetOverflowFlag();
		DebuggerBreak();
		return;
	}

	extern uint32_t g_LittleBits[32];
	if(nValue)
		m_pData[iBit >> 5] |= g_LittleBits[iBit & 31];
	else
		m_pData[iBit >> 5] &= ~g_LittleBits[iBit & 31];
}

void bf_write::WriteUBitLong( unsigned int curData, int numbits, bool bCheckRange )
{
	if ( GetNumBitsLeft() < numbits )
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		DebuggerBreak();
		return;
	}

	int iCurBitMasked = m_iCurBit & 31;
	int iDWord = m_iCurBit >> 5;
	m_iCurBit += numbits;

	// Mask in a dword.
	Assert( (iDWord*4 + sizeof(uint32_t)) <= (unsigned int)m_nDataBytes );
	uint32_t * pOut = &m_pData[iDWord];

	// Rotate data into dword alignment
	curData = (curData << iCurBitMasked) | (curData >> (32 - iCurBitMasked));

	// Calculate bitmasks for first and second word
	unsigned int temp = 1 << (numbits-1);
	unsigned int mask1 = (temp*2-1) << iCurBitMasked;
	unsigned int mask2 = (temp-1) >> (31 - iCurBitMasked);
	
	// Only look beyond current word if necessary (avoid access violation)
	int i = mask2 & 1;
	uint32_t dword1 = LoadLittleDWord( pOut, 0 );
	uint32_t dword2 = LoadLittleDWord( pOut, i );
	
	// Drop bits into place
	dword1 ^= ( mask1 & ( curData ^ dword1 ) );
	dword2 ^= ( mask2 & ( curData ^ dword2 ) );

	// Note reversed order of writes so that dword1 wins if mask2 == 0 && i == 0
	StoreLittleDWord( pOut, i, dword2 );
	StoreLittleDWord( pOut, 0, dword1 );
}

void bf_write::WriteUBitVar( unsigned int n )
{
	if(n < 16)
		WriteUBitLong( n, 6 );
	else
		if(n < 256)
			WriteUBitLong( (n & 15) | 16 | ((n & (128 | 64 | 32 | 16)) << 2), 10 );
		else
			if(n < 4096)
				WriteUBitLong( (n & 15) | 32 | ((n & (2048 | 1024 | 512 | 256 | 128 | 64 | 32 | 16)) << 2), 14 );
			else
			{
				WriteUBitLong( (n & 15) | 48, 6 );
				WriteUBitLong( (n >> 4), 32 - 4 );
			}
}

void bf_write::WriteBitFloat(const float val)
{
	WriteUBitLong( FloatBits( val ), 32 );
}

int bf_read::GetNumBytesRead()	
{
	return BitByte(m_iCurBit);
}

int bf_read::GetNumBitsLeft()
{
	return m_nDataBits - m_iCurBit;
}

int bf_read::GetNumBytesLeft()	
{
	return GetNumBitsLeft() >> 3;
}

int bf_read::GetNumBitsRead() const
{
	return m_iCurBit;
}

bool bf_read::Seek(int iBit)
{
	if(iBit < 0 || iBit > m_nDataBits)
	{
		SetOverflowFlag();
		m_iCurBit = m_nDataBits;
		return false;
	}
	else
	{
		m_iCurBit = iBit;
		return true;
	}
}

bool	bf_read::SeekRelative(int iBitDelta)		
{
	return Seek(m_iCurBit+iBitDelta);
}	

bool bf_read::CheckForOverflow(int nBits)
{
	if( m_iCurBit + nBits > m_nDataBits )
	{
		SetOverflowFlag();
		DebuggerBreak();
	}

	return m_bOverflow;
}

int bf_read::ReadOneBitNoCheck()
{
	unsigned int value;
	if ( m_bCanReadDwords )
		value = ((uint32_t *)m_pData)[m_iCurBit >> 5] >> (m_iCurBit & 31);
	else
		value = m_pData[m_iCurBit >> 3] >> (m_iCurBit & 7);
	++m_iCurBit;
	return value & 1;
}

inline int bf_read::ReadOneBit()
{
	if( GetNumBitsLeft() <= 0 )
	{
		SetOverflowFlag();
		DebuggerBreak();
		return 0;
	}
	return ReadOneBitNoCheck();
}

inline float bf_read::ReadBitFloat()
{
	union { uint32_t u; float f; } c = { ReadUBitLong(32) };
	return c.f;
}

unsigned int bf_read::ReadUBitVar()
{
	unsigned int ret = ReadUBitLong( 6 );
	switch(ret & (16 | 32))
	{
		case 16:
		ret = (ret & 15) | (ReadUBitLong( 4 ) << 4);
		Assert( ret >= 16 );
		break;

		case 32:
		ret = (ret & 15) | (ReadUBitLong( 8 ) << 4);
		Assert( ret >= 256 );
		break;
		case 48:
		ret = (ret & 15) | (ReadUBitLong( 32 - 4 ) << 4);
		Assert( ret >= 4096 );
		break;
	}
	return ret;
}

unsigned int bf_read::ReadUBitLong( int numbits )
{
	Assert( numbits > 0 && numbits <= 32 );

	if ( GetNumBitsLeft() < numbits )
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		DebuggerBreak();
		return 0;
	}

	unsigned int iStartBit = m_iCurBit & 31u;
	int iLastBit = m_iCurBit + numbits - 1;
	unsigned int iWordOffset1 = m_iCurBit >> 5;
	unsigned int iWordOffset2 = iLastBit >> 5;

	if ( !m_bCanReadDwords && (int)( iWordOffset2 * 4 + 3 ) >= m_nDataBytes )
	{
		unsigned int ret = 0;
		int nShift = 0;
		for ( ; numbits >= 8; numbits -= 8, nShift += 8 )
		{
			unsigned int iByteBit = m_iCurBit & 7;
			unsigned int nByte = ( m_pData[m_iCurBit >> 3] >> iByteBit ) | (uint8_t)( m_pData[(m_iCurBit + 7) >> 3] << ( 8 - iByteBit ) );
			ret |= nByte << nShift;
			m_iCurBit += 8;
		}

		for ( int i = 0; i < numbits; i++ )
			ret |= ReadOneBitNoCheck() << ( nShift + i );

		return ret;
	}

	m_iCurBit += numbits;

	unsigned int bitmask = g_BitBuf_ExtraMasks[numbits];
	unsigned int dw1 = LoadLittleDWord( (uint32_t*)m_pData, iWordOffset1 ) >> iStartBit;
	unsigned int dw2 = LoadLittleDWord( (uint32_t*)m_pData, iWordOffset2 ) << (32 - iStartBit);

	return (dw1 | dw2) & bitmask;
}

int bf_read::CompareBits( bf_read * other, int numbits )
{
	return (ReadUBitLong(numbits) != other->ReadUBitLong(numbits));
}

unsigned char* bf_write::	GetBasePointer()
{ return (unsigned char*) m_pData; }

bool bf_write::		IsOverflowed() const
{return m_bOverflow;}

const char* bf_read::		GetDebugName() const
{ return m_pDebugName; }

const unsigned char* bf_read::	GetBasePointer()
{ return m_pData; }

int bf_read:: TotalBytesAvailable( void ) const
{
	return m_nDataBytes;
}

int bf_read::	ReadChar()
{ return (char)ReadUBitLong(8); }

int bf_read::	ReadByte()
{ return ReadUBitLong(8); }

int bf_read::	ReadShort()
{ return (short)ReadUBitLong(16); }

int bf_read::	ReadWord()
{ return ReadUBitLong(16); }

long bf_read:: ReadLong()
{ return ReadUBitLong(32); }

bool bf_read::		IsOverflowed() const
{return m_bOverflow;}
