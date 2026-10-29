#ifndef BITVEC_H
#define BITVEC_H
#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "public/common.h"

class CBitVecAccessor
{
public:
				CBitVecAccessor(uint32_t *pDWords, int iBit);

	void		operator=(int val);
				operator uint32_t();

private:
	uint32_t *m_pDWords;
	int	m_iBit;
};
	
#define LOG2_BITS_PER_INT	5
#define BITS_PER_INT		32

#if _WIN32
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif

int FirstBitInWord( unsigned int elem, int offset );
uint32_t GetEndMask( int numBits );
int GetBitForBitnum( int bitNum );
int GetBitForBitnumByte( int bitNum );
int CalcNumIntsForBits( int numBits );

#define BitVec_Bit( bitNum ) ( 1 << ( (bitNum) & (BITS_PER_INT-1) ) )
#define BitVec_BitInByte( bitNum ) ( 1 << ( (bitNum) & 7 ) )
#define BitVec_Int( bitNum ) ( (bitNum) >> LOG2_BITS_PER_INT )

template <class BASE_OPS>
class CBitVecT : public BASE_OPS
{
public:
	CBitVecT();
	CBitVecT(int numBits);			// Must be initialized with the number of bits

	void	Init(int val = 0);

	// Access the bits like an array.
	CBitVecAccessor	operator[](int i);

	// Do NOT override bitwise operators (see note in header)
	void	And(const CBitVecT &andStr, CBitVecT *out) const;
	void	Or(const CBitVecT &orStr, CBitVecT *out) const;
	void	Xor(const CBitVecT &orStr, CBitVecT *out) const;
	
	void	Not(CBitVecT *out) const;
	
	void	CopyTo(CBitVecT *out) const;
	void	Copy( const CBitVecT<BASE_OPS> &other, int nBits=-1 );
	bool	Compare( const CBitVecT<BASE_OPS> &other, int nBits=-1 ) const;

	bool	IsAllClear(void) const;		// Are all bits zero?
	bool	IsAllSet(void) const;		// Are all bits one?

	uint32_t	Get( uint32_t bitNum ) const;
	bool 	IsBitSet( int bitNum ) const;
	void 	Set( int bitNum );
	void 	Set( int bitNum, bool bNewVal );
	void 	Clear(int bitNum);

	bool	TestAndSet(int bitNum);

	void 	Set( uint32_t offset, uint32_t mask );
	void 	Clear( uint32_t offset, uint32_t mask );
	uint32_t	Get( uint32_t offset, uint32_t mask );

	void	SetAll(void);			// Sets all bits
	void	ClearAll(void);			// Clears all bits

	uint32_t	GetDWord(int i) const;
	void	SetDWord(int i, uint32_t val);

	CBitVecT<BASE_OPS>&	operator=(const CBitVecT<BASE_OPS> &other)	{ other.CopyTo( this ); return *this; }
	bool			operator==(const CBitVecT<BASE_OPS> &other)		{ return Compare( other ); }
	bool			operator!=(const CBitVecT<BASE_OPS> &other)		{ return !operator==( other ); }

	static void GetOffsetMaskForBit( uint32_t bitNum, uint32_t *pOffset, uint32_t *pMask )	{ *pOffset = BitVec_Int( bitNum ); *pMask = BitVec_Bit( bitNum ); }
};

class CVarBitVecBase
{
public:
	bool	IsFixedSize() const;
	int		GetNumBits(void) const;
	void	Resize( int numBits, bool bClearAll = false );		// resizes bit array
	
	int 	GetNumDWords() const;
	uint32_t *Base();
	const uint32_t *Base() const;

	void Attach( uint32_t *pBits, int numBits );
	bool Detach( uint32_t **ppBits, int *pNumBits );

	int		FindNextSetBit(int iStartBit) const; // returns -1 if no set bit was found

protected:
	CVarBitVecBase();
	CVarBitVecBase(int numBits);
	CVarBitVecBase( const CVarBitVecBase &from );
	CVarBitVecBase &operator=( const CVarBitVecBase &from );
	~CVarBitVecBase(void);
	
	void 		ValidateOperand( const CVarBitVecBase &operand ) const;

	unsigned	GetEndMask() const;

private:

	unsigned short	m_numBits;					// Number of bits in the bitstring
	unsigned short	m_numInts;					// Number of ints to needed to store bitstring
	uint32_t			m_iBitStringStorage;		// If the bit string fits in one int, it goes here
	uint32_t *		m_pInt;					// Array of ints containing the bitstring

	void	AllocInts( int numInts );	// Free the allocated bits
	void	ReallocInts( int numInts );
	void	FreeInts( void );			// Free the allocated bits
};

template <int bits> struct BitCountToEndMask_t { };
template <> struct BitCountToEndMask_t< 0> { enum { MASK = 0xffffffff }; };
template <> struct BitCountToEndMask_t< 1> { enum { MASK = 0x00000001 }; };
template <> struct BitCountToEndMask_t< 2> { enum { MASK = 0x00000003 }; };
template <> struct BitCountToEndMask_t< 3> { enum { MASK = 0x00000007 }; };
template <> struct BitCountToEndMask_t< 4> { enum { MASK = 0x0000000f }; };
template <> struct BitCountToEndMask_t< 5> { enum { MASK = 0x0000001f }; };
template <> struct BitCountToEndMask_t< 6> { enum { MASK = 0x0000003f }; };
template <> struct BitCountToEndMask_t< 7> { enum { MASK = 0x0000007f }; };
template <> struct BitCountToEndMask_t< 8> { enum { MASK = 0x000000ff }; };
template <> struct BitCountToEndMask_t< 9> { enum { MASK = 0x000001ff }; };
template <> struct BitCountToEndMask_t<10> { enum { MASK = 0x000003ff }; };
template <> struct BitCountToEndMask_t<11> { enum { MASK = 0x000007ff }; };
template <> struct BitCountToEndMask_t<12> { enum { MASK = 0x00000fff }; };
template <> struct BitCountToEndMask_t<13> { enum { MASK = 0x00001fff }; };
template <> struct BitCountToEndMask_t<14> { enum { MASK = 0x00003fff }; };
template <> struct BitCountToEndMask_t<15> { enum { MASK = 0x00007fff }; };
template <> struct BitCountToEndMask_t<16> { enum { MASK = 0x0000ffff }; };
template <> struct BitCountToEndMask_t<17> { enum { MASK = 0x0001ffff }; };
template <> struct BitCountToEndMask_t<18> { enum { MASK = 0x0003ffff }; };
template <> struct BitCountToEndMask_t<19> { enum { MASK = 0x0007ffff }; };
template <> struct BitCountToEndMask_t<20> { enum { MASK = 0x000fffff }; };
template <> struct BitCountToEndMask_t<21> { enum { MASK = 0x001fffff }; };
template <> struct BitCountToEndMask_t<22> { enum { MASK = 0x003fffff }; };
template <> struct BitCountToEndMask_t<23> { enum { MASK = 0x007fffff }; };
template <> struct BitCountToEndMask_t<24> { enum { MASK = 0x00ffffff }; };
template <> struct BitCountToEndMask_t<25> { enum { MASK = 0x01ffffff }; };
template <> struct BitCountToEndMask_t<26> { enum { MASK = 0x03ffffff }; };
template <> struct BitCountToEndMask_t<27> { enum { MASK = 0x07ffffff }; };
template <> struct BitCountToEndMask_t<28> { enum { MASK = 0x0fffffff }; };
template <> struct BitCountToEndMask_t<29> { enum { MASK = 0x1fffffff }; };
template <> struct BitCountToEndMask_t<30> { enum { MASK = 0x3fffffff }; };
template <> struct BitCountToEndMask_t<31> { enum { MASK = 0x7fffffff }; };

template <int NUM_BITS>
class CFixedBitVecBase
{
public:
	bool	IsFixedSize() const								{ return true; }
	int		GetNumBits(void) const							{ return NUM_BITS; }
	void	Resize( int numBits, bool bClearAll = false )	{ Assert(numBits == NUM_BITS); if ( bClearAll ) memset( m_Ints, 0, NUM_INTS * sizeof(uint32_t) ); }// for syntatic consistency (for when using templates)
	
	int 			GetNumDWords() const					{ return NUM_INTS; }
	uint32_t *		Base()									{ return m_Ints;	}
	const uint32_t *	Base() const							{ return m_Ints;	}

	int		FindNextSetBit(int iStartBit) const; // returns -1 if no set bit was found

protected:
	CFixedBitVecBase()				{}
	CFixedBitVecBase(int numBits)	{ Assert( numBits == NUM_BITS ); } // doesn't make sense, really. Supported to simplify templates & allow easy replacement of variable 
	
	void 		ValidateOperand( const CFixedBitVecBase<NUM_BITS> &operand ) const	{ } // no need, compiler does so statically

public: // for test code
	uint32_t	GetEndMask() const		{ return static_cast<uint32_t>( BitCountToEndMask_t<NUM_BITS % BITS_PER_INT>::MASK ); }

private:
	enum
	{
		NUM_INTS = (NUM_BITS + (BITS_PER_INT-1)) / BITS_PER_INT
	};

	uint32_t m_Ints[(NUM_BITS + (BITS_PER_INT-1)) / BITS_PER_INT];
};

class CVarBitVec : public CBitVecT<CVarBitVecBase>
{
public:
	CVarBitVec();
	
	CVarBitVec(int numBits);
};

template < int NUM_BITS >
class CBitVec : public CBitVecT< CFixedBitVecBase<NUM_BITS> >
{
public:
	CBitVec()
	{
	}
	
	CBitVec(int numBits)
	 : CBitVecT< CFixedBitVecBase<NUM_BITS> >(numBits)
	{
	}
};

template < int NUM_BITS >
class CTypedBitVec : public CBitVecT< CFixedBitVecBase<NUM_BITS> >
{
public:
	CTypedBitVec()
	{
	}

	CTypedBitVec(int numBits)
	 : CBitVecT< CFixedBitVecBase<NUM_BITS> >(numBits)
	{
	}
};

typedef CBitVec<32> CDWordBitVec;
typedef CBitVec<64> CQWordBitVec;
typedef CQWordBitVec CPlayerBitVec;

template <class BASE_OPS>
inline CBitVecT<BASE_OPS>::CBitVecT()
{
	static_assert( sizeof(int)==4 );
	ClearAll();
}

template <class BASE_OPS>
inline CBitVecT<BASE_OPS>::CBitVecT(int numBits)
 : BASE_OPS( numBits )
{
	static_assert( sizeof(int)==4 );
	ClearAll();
}

template <class BASE_OPS>
inline CBitVecAccessor CBitVecT<BASE_OPS>::operator[](int i)	
{
	Assert(i >= 0 && i < this->GetNumBits());
	return CBitVecAccessor(this->Base(), i);
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Init( int val )
{
	if ( this->Base() )
		memset( this->Base(), ( val ) ? 0xff : 0, this->GetNumDWords() * sizeof(int) );
}

template <class BASE_OPS>
inline uint32_t CBitVecT<BASE_OPS>::Get( uint32_t bitNum ) const
{
	Assert( bitNum < (uint32_t)this->GetNumBits() );
	const uint32_t *pInt = this->Base() + BitVec_Int( bitNum );
	return ( *pInt & BitVec_Bit( bitNum ) );
}

template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::IsBitSet( int bitNum ) const
{
	Assert( bitNum >= 0 && bitNum < this->GetNumBits() );
	const uint32_t *pInt = this->Base() + BitVec_Int( bitNum );
	return ( ( *pInt & BitVec_Bit( bitNum ) ) != 0 );
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Set( int bitNum )			
{
	Assert( bitNum >= 0 && bitNum < this->GetNumBits() );
	uint32_t *pInt = this->Base() + BitVec_Int( bitNum );
	*pInt |= BitVec_Bit( bitNum );
}

template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::TestAndSet(int bitNum)
{
	Assert( bitNum >= 0 && bitNum < this->GetNumBits() );
	uint32_t bitVecBit = BitVec_Bit( bitNum );
	uint32_t *pInt = this->Base() + BitVec_Int( bitNum );
	bool bResult = ( ( *pInt & bitVecBit) != 0 );
	*pInt |= bitVecBit;
	return bResult;
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Clear(int bitNum)		
{
	Assert( bitNum >= 0 && bitNum < this->GetNumBits() );
	uint32_t *pInt = this->Base() + BitVec_Int( bitNum );
	*pInt &= ~BitVec_Bit( bitNum );
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Set( int bitNum, bool bNewVal )
{
	uint32_t *pInt = this->Base() + BitVec_Int( bitNum );
	uint32_t bitMask = BitVec_Bit( bitNum );
	if ( bNewVal )
	{
		*pInt |= bitMask;
	}
	else
	{
		*pInt &= ~bitMask;
	}
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Set( uint32_t offset, uint32_t mask )
{
	uint32_t *pInt = this->Base() + offset;
	*pInt |= mask;
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Clear( uint32_t offset, uint32_t mask )
{
	uint32_t *pInt = this->Base() + offset;
	*pInt &= ~mask;
}

template <class BASE_OPS>
inline uint32_t CBitVecT<BASE_OPS>::Get( uint32_t offset, uint32_t mask )
{
	uint32_t *pInt = this->Base() + offset;
	return ( *pInt & mask );
}

template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::And(const CBitVecT &addStr, CBitVecT *out) const
{
	this->ValidateOperand( addStr );
	this->ValidateOperand( *out );
	
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= this->Base();
	const uint32_t *pOperand2	= addStr.Base();

	for (int i = this->GetNumDWords() - 1; i >= 0 ; --i) 
	{
		pDest[i] = pOperand1[i] & pOperand2[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Or(const CBitVecT &orStr, CBitVecT *out) const
{
	this->ValidateOperand( orStr );
	this->ValidateOperand( *out );

	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= this->Base();
	const uint32_t *pOperand2	= orStr.Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i) 
	{
		pDest[i] = pOperand1[i] | pOperand2[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Xor(const CBitVecT &xorStr, CBitVecT *out) const
{
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= this->Base();
	const uint32_t *pOperand2	= xorStr.Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i) 
	{
		pDest[i] = pOperand1[i] ^ pOperand2[i];
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Not(CBitVecT *out) const
{
	this->ValidateOperand( *out );

	uint32_t *	   pDest	= out->Base();
	const uint32_t *pOperand	= this->Base();

	for (int i = this->GetNumDWords() - 1; i >= 0; --i) 
	{
		pDest[i] = ~(pOperand[i]);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Copy a bit string
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::CopyTo(CBitVecT *out) const
{
	out->Resize( this->GetNumBits() );

	this->ValidateOperand( *out );
	Assert( out != this );
	
	memcpy( out->Base(), this->Base(), this->GetNumDWords() * sizeof( int ) );
}

//-----------------------------------------------------------------------------
// Purpose: Are all bits zero?
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::IsAllClear(void) const
{
	// Number of available bits may be more than the number
	// actually used, so make sure to mask out unused bits
	// before testing for zero
	(const_cast<CBitVecT *>(this))->Base()[this->GetNumDWords()-1] &= CBitVecT<BASE_OPS>::GetEndMask(); // external semantics of const retained

	for (int i = this->GetNumDWords() - 1; i >= 0; --i) 
	{
		if ( this->Base()[i] !=0 ) 
		{
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Are all bits set?
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::IsAllSet(void) const
{
	// Number of available bits may be more than the number
	// actually used, so make sure to mask out unused bits
	// before testing for set bits
	(const_cast<CBitVecT *>(this))->Base()[this->GetNumDWords()-1] |= ~CBitVecT<BASE_OPS>::GetEndMask();  // external semantics of const retained

	for (int i = this->GetNumDWords() - 1; i >= 0; --i) 
	{
		if ( this->Base()[i] != ~0u ) 
		{
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Sets all bits
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::SetAll(void)		
{
	if ( this->Base() )
		memset( this->Base(), 0xff, this->GetNumDWords() * sizeof(int) );
}

//-----------------------------------------------------------------------------
// Purpose: Clears all bits
// Input  :
// Output :
//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::ClearAll(void)		
{
	if ( this->Base() )
		memset( this->Base(), 0, this->GetNumDWords() * sizeof(int) );
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::Copy( const CBitVecT<BASE_OPS> &other, int nBits )
{
	if ( nBits == - 1 )
	{
		nBits = other.GetNumBits();
	}

	this->Resize( nBits );

	this->ValidateOperand( other );
	Assert( &other != this );

	memcpy( this->Base(), other.Base(), this->GetNumDWords() * sizeof( uint32_t ) );
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline bool CBitVecT<BASE_OPS>::Compare( const CBitVecT<BASE_OPS> &other, int nBits ) const
{
	if ( nBits == - 1 )
	{
		if ( other.GetNumBits() != this->GetNumBits() )
		{
			return false;
		}

		nBits = other.GetNumBits();
	}

	if ( nBits > other.GetNumBits() || nBits > this->GetNumBits() )
	{
		return false;
	}

	(const_cast<CBitVecT *>(this))->Base()[this->GetNumDWords()-1] &= CBitVecT<BASE_OPS>::GetEndMask(); // external semantics of const retained
	(const_cast<CBitVecT *>(&other))->Base()[this->GetNumDWords()-1] &= other.CBitVecT<BASE_OPS>::GetEndMask(); // external semantics of const retained

	int nBytes = PAD_NUMBER( nBits, 8 ) >> 3;

	return ( memcmp( this->Base(), other.Base(), nBytes ) == 0 );
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline uint32_t CBitVecT<BASE_OPS>::GetDWord(int i) const
{
	Assert(i >= 0 && i < this->GetNumDWords());
	return this->Base()[i];
}

//-----------------------------------------------------------------------------
template <class BASE_OPS>
inline void CBitVecT<BASE_OPS>::SetDWord(int i, uint32_t val)
{
	Assert(i >= 0 && i < this->GetNumDWords());
	this->Base()[i] = val;
}

uint32_t GetStartBitMask( int startBit );

template <int NUM_BITS>
inline int CFixedBitVecBase<NUM_BITS>::FindNextSetBit( int startBit ) const
{
	if ( startBit < NUM_BITS )
	{
		int wordIndex = BitVec_Int(startBit);
		unsigned int startMask = GetStartBitMask( startBit );

		// handle non dword lengths
		if ( (NUM_BITS % BITS_PER_INT) != 0 )
		{
			unsigned int elem = Base()[wordIndex];
			elem &= startMask;
			if ( wordIndex == NUM_INTS-1)
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
				for ( int i = wordIndex+1; i < NUM_INTS-1; i++ )
				{
					elem = Base()[i];
					if ( elem )
						return FirstBitInWord(elem, i << 5);
				}
				elem = Base()[NUM_INTS-1] & GetEndMask();
				if ( elem )
					return FirstBitInWord(elem, (NUM_INTS-1) << 5);
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
			} while( wordIndex <= NUM_INTS-1);
		}

	}

	return -1;
}

template<> 
inline void CBitVecT< CFixedBitVecBase<256> >::And(const CBitVecT &addStr, CBitVecT *out) const
{
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= Base();
	const uint32_t *pOperand2	= addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
	pDest[2] = pOperand1[2] & pOperand2[2];
	pDest[3] = pOperand1[3] & pOperand2[3];
	pDest[4] = pOperand1[4] & pOperand2[4];
	pDest[5] = pOperand1[5] & pOperand2[5];
	pDest[6] = pOperand1[6] & pOperand2[6];
	pDest[7] = pOperand1[7] & pOperand2[7];
}

template<> 
inline  bool CBitVecT< CFixedBitVecBase<256> >::IsAllClear(void) const
{
	const uint32_t *pInts = Base();
	return ( pInts[0] == 0 && pInts[1] == 0 && pInts[2] == 0 && pInts[3] == 0 && pInts[4] == 0 && pInts[5] == 0 && pInts[6] == 0 && pInts[7] == 0 );
}

template<> 
inline  void CBitVecT< CFixedBitVecBase<256> >::CopyTo(CBitVecT *out) const
{
	uint32_t *	   pDest = out->Base();
	const uint32_t *pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
	pDest[2] = pInts[2];
	pDest[3] = pInts[3];
	pDest[4] = pInts[4];
	pDest[5] = pInts[5];
	pDest[6] = pInts[6];
	pDest[7] = pInts[7];
}

template<> 
inline  void CBitVecT< CFixedBitVecBase<128> >::And(const CBitVecT &addStr, CBitVecT *out) const
{
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= Base();
	const uint32_t *pOperand2	= addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
	pDest[2] = pOperand1[2] & pOperand2[2];
	pDest[3] = pOperand1[3] & pOperand2[3];
}

template<> 
inline  bool CBitVecT< CFixedBitVecBase<128> >::IsAllClear(void) const
{
	const uint32_t *pInts = Base();
	return ( pInts[0] == 0 && pInts[1] == 0 && pInts[2] == 0 && pInts[3] == 0 );
}

template<> 
inline  void CBitVecT< CFixedBitVecBase<128> >::CopyTo(CBitVecT *out) const
{
	uint32_t *	   pDest = out->Base();
	const uint32_t *pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
	pDest[2] = pInts[2];
	pDest[3] = pInts[3];
}

template<> 
inline void CBitVecT< CFixedBitVecBase<96> >::And(const CBitVecT &addStr, CBitVecT *out) const
{
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= Base();
	const uint32_t *pOperand2	= addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
	pDest[2] = pOperand1[2] & pOperand2[2];
}

template<> 
inline bool CBitVecT< CFixedBitVecBase<96> >::IsAllClear(void) const
{
	const uint32_t *pInts = Base();
	return ( pInts[0] == 0 && pInts[1] == 0 && pInts[2] == 0 );
}

template<> 
inline void CBitVecT< CFixedBitVecBase<96> >::CopyTo(CBitVecT *out) const
{
	uint32_t *	   pDest = out->Base();
	const uint32_t *pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
	pDest[2] = pInts[2];
}

template<> 
inline void CBitVecT< CFixedBitVecBase<64> >::And(const CBitVecT &addStr, CBitVecT *out) const
{
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= Base();
	const uint32_t *pOperand2	= addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
	pDest[1] = pOperand1[1] & pOperand2[1];
}

template<> 
inline bool CBitVecT< CFixedBitVecBase<64> >::IsAllClear(void) const
{
	const uint32_t *pInts = Base();
	return ( pInts[0] == 0 && pInts[1] == 0 );
}

template<> 
inline void CBitVecT< CFixedBitVecBase<64> >::CopyTo(CBitVecT *out) const
{
	uint32_t *	   pDest = out->Base();
	const uint32_t *pInts = Base();

	pDest[0] = pInts[0];
	pDest[1] = pInts[1];
}

template<> 
inline void CBitVecT< CFixedBitVecBase<32> >::And(const CBitVecT &addStr, CBitVecT *out) const
{
	uint32_t *	   pDest		= out->Base();
	const uint32_t *pOperand1	= Base();
	const uint32_t *pOperand2	= addStr.Base();

	pDest[0] = pOperand1[0] & pOperand2[0];
}

template<> 
inline bool CBitVecT< CFixedBitVecBase<32> >::IsAllClear(void) const
{
	const uint32_t *pInts = Base();

	return ( pInts[0] == 0 );
}

template<> 
inline void CBitVecT< CFixedBitVecBase<32> >::CopyTo(CBitVecT *out) const
{
	uint32_t *	   pDest = out->Base();
	const uint32_t *pInts = Base();

	pDest[0] = pInts[0];
}

template <>
inline uint32_t CBitVecT< CFixedBitVecBase<32> >::Get( uint32_t bitNum ) const
{
	return ( *Base() & BitVec_Bit( bitNum ) );
}

//-----------------------------------------------------------------------------

template <>
inline bool CBitVecT< CFixedBitVecBase<32> >::IsBitSet( int bitNum ) const
{
	return ( ( *Base() & BitVec_Bit( bitNum ) ) != 0 );
}

//-----------------------------------------------------------------------------

template <>
inline void CBitVecT< CFixedBitVecBase<32> >::Set( int bitNum )			
{
	*Base() |= BitVec_Bit( bitNum );
}

//-----------------------------------------------------------------------------

template <>
inline void CBitVecT< CFixedBitVecBase<32> >::Clear(int bitNum)		
{
	*Base() &= ~BitVec_Bit( bitNum );
}

//-----------------------------------------------------------------------------

template <>
inline void CBitVecT< CFixedBitVecBase<32> >::Set( int bitNum, bool bNewVal )
{
	uint32_t bitMask = BitVec_Bit( bitNum );
	if ( bNewVal )
	{
		*Base() |= bitMask;
	}
	else
	{
		*Base() &= ~bitMask;
	}
}


#endif