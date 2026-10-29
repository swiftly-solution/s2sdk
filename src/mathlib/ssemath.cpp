#include "public/mathlib/ssemath.h"

int & intx4::operator[](int which)
{
	return m_i32[which];
}

const int & intx4::operator[](int which) const
{
	return m_i32[which];
}

int32_t *intx4::Base()
{
	return m_i32;
}

const int32_t *intx4::Base() const
{
	return m_i32;
}

const bool intx4::operator==(const intx4 &other) const
{
	return m_i32[0] == other.m_i32[0] &&
		m_i32[1] == other.m_i32[1] &&
		m_i32[2] == other.m_i32[2] &&
		m_i32[3] == other.m_i32[3] 	;
}

void StoreAlignedSIMD( float * RESTRICT pSIMD, const fltx4 & a )
{
	_mm_store_ps( pSIMD, a );
}

void StoreUnalignedSIMD( float * RESTRICT pSIMD, const fltx4 & a )
{
	_mm_storeu_ps( pSIMD, a );
}

void StoreUnaligned3SIMD( float *pSIMD, const fltx4 & a )
{
	_mm_store_ss(pSIMD, a);
	_mm_store_ss(pSIMD+1, RotateLeft(a));
	_mm_store_ss(pSIMD+2, RotateLeft2(a));
}

void StoreAligned3SIMD( VectorAligned * RESTRICT pSIMD, const fltx4 & a )
{
	StoreAlignedSIMD( pSIMD->Base(),a );
}

void StoreFourUnalignedVector3SIMD( fltx4 a, fltx4 b, fltx4	c, FLTX4 d,
											   Vector * const pDestination )
{
	StoreUnaligned3SIMD( pDestination->Base(), a );
	StoreUnaligned3SIMD( (pDestination+1)->Base(), b );
	StoreUnaligned3SIMD( (pDestination+2)->Base(), c );
	StoreUnaligned3SIMD( (pDestination+3)->Base(), d );
}

void StoreFourAlignedVector3SIMD( fltx4 a, fltx4 b, fltx4	c, FLTX4 d,
											 Vector * const pDestination )
{
	StoreUnaligned3SIMD( pDestination->Base(), a );
	StoreUnaligned3SIMD( (pDestination+1)->Base(), b );
	StoreUnaligned3SIMD( (pDestination+2)->Base(), c );
	StoreUnaligned3SIMD( (pDestination+3)->Base(), d );
}

fltx4 LoadAlignedSIMD( const void *pSIMD )
{
	return _mm_load_ps( reinterpret_cast< const float *> ( pSIMD ) );
}

fltx4 AndSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_and_ps( a, b );
}

fltx4 AndNotSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_andnot_ps( a, b );
}

fltx4 XorSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_xor_ps( a, b );
}

fltx4 OrSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_or_ps( a, b );
}

fltx4 SetWToZeroSIMD( const fltx4 & a )
{
	return AndSIMD( a, LoadAlignedSIMD( g_SIMD_clear_wmask ) );
}

fltx4 LoadAlignedSIMD( const VectorAligned & pSIMD )
{
	return SetWToZeroSIMD( LoadAlignedSIMD(pSIMD.Base()) );
}

fltx4 LoadUnalignedSIMD( const void *pSIMD )
{
	return _mm_loadu_ps( reinterpret_cast<const float *>( pSIMD ) );
}

fltx4 LoadUnaligned3SIMD( const void *pSIMD )
{
	return _mm_loadu_ps( reinterpret_cast<const float *>( pSIMD ) );
}

fltx4 LoadUnalignedFloatSIMD( const float *pFlt )
{
	return _mm_load_ss(pFlt);
}

fltx4 ReplicateIX4( int i )
{
	fltx4 value = _mm_set_ss( BitsToFloat( i ) );
	return _mm_shuffle_ps( value, value, 0);
}

fltx4 ReplicateX4( float flValue )
{
	__m128 value = _mm_set_ss( flValue );
	return _mm_shuffle_ps( value, value, 0 );
}

float SubFloat( const fltx4 & a, int idx )
{
	// NOTE: if the output goes into a register, this causes a Load-Hit-Store stall (don't mix fpu/vpu math!)
#ifndef POSIX
	return a.m128_f32[ idx ];
#else
	return (reinterpret_cast<float const *>(&a))[idx];
#endif
}

float & SubFloat( fltx4 & a, int idx )
{
#ifndef POSIX
	return a.m128_f32[ idx ];
#else
	return (reinterpret_cast<float *>(&a))[idx];
#endif
}

uint32_t SubFloatConvertToInt( const fltx4 & a, int idx )
{
	return (uint32_t)SubFloat(a,idx);
}

uint32_t SubInt( const fltx4 & a, int idx )
{
#ifndef POSIX
	return a.m128_u32[idx];
#else
	return (reinterpret_cast<uint32_t const *>(&a))[idx];
#endif
}

uint32_t & SubInt( fltx4 & a, int idx )
{
#ifndef POSIX
	return a.m128_u32[idx];
#else
	return (reinterpret_cast<uint32_t *>(&a))[idx];
#endif
}

fltx4 LoadZeroSIMD( void )
{
	return Four_Zeros;
}

fltx4 LoadOneSIMD( void )
{
	return Four_Ones;
}

fltx4 MaskedAssign( const fltx4 & ReplacementMask, const fltx4 & NewValue, const fltx4 & OldValue )
{
	return OrSIMD(
		AndSIMD( ReplacementMask, NewValue ),
		AndNotSIMD( ReplacementMask, OldValue ) );
}

fltx4 SplatXSIMD( fltx4 const & a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
}

fltx4 SplatYSIMD( fltx4 const &a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 1, 1, 1, 1 ) );
}

fltx4 SplatZSIMD( fltx4 const &a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 2, 2, 2 ) );
}

fltx4 SplatWSIMD( fltx4 const &a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 3, 3, 3, 3 ) );
}

fltx4 SetXSIMD( const fltx4& a, const fltx4& x )
{
	fltx4 result = MaskedAssign( LoadAlignedSIMD( g_SIMD_ComponentMask[0] ), x, a );
	return result;
}

fltx4 SetYSIMD( const fltx4& a, const fltx4& y )
{
	fltx4 result = MaskedAssign( LoadAlignedSIMD( g_SIMD_ComponentMask[1] ), y, a );
	return result;
}

fltx4 SetZSIMD( const fltx4& a, const fltx4& z )
{
	fltx4 result = MaskedAssign( LoadAlignedSIMD( g_SIMD_ComponentMask[2] ), z, a );
	return result;
}

fltx4 SetWSIMD( const fltx4& a, const fltx4& w )
{
	fltx4 result = MaskedAssign( LoadAlignedSIMD( g_SIMD_ComponentMask[3] ), w, a );
	return result;
}

fltx4 SetComponentSIMD( const fltx4& a, int nComponent, float flValue )
{
	fltx4 val = ReplicateX4( flValue );
	fltx4 result = MaskedAssign( LoadAlignedSIMD( g_SIMD_ComponentMask[nComponent] ), val, a );
	return result;
}

fltx4 RotateLeft( const fltx4 & a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 1, 2, 3, 0 ) );
}

fltx4 RotateLeft2( const fltx4 & a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 3, 0, 1 ) );
}

fltx4 RotateRight( const fltx4 & a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 3, 0, 1, 2 ) );
}

fltx4 RotateRight2( const fltx4 & a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 3, 0, 1 ) );
}

fltx4 AddSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_add_ps( a, b );
}

fltx4 SubSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_sub_ps( a, b );
}

fltx4 MulSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_mul_ps( a, b );
}

fltx4 DivSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_div_ps( a, b );
}

fltx4 MaddSIMD( const fltx4 & a, const fltx4 & b, const fltx4 & c )
{
	return AddSIMD( MulSIMD(a,b), c );
}

fltx4 MsubSIMD( const fltx4 & a, const fltx4 & b, const fltx4 & c )
{
	return SubSIMD( c, MulSIMD(a,b) );
}

fltx4 Dot3SIMD( const fltx4 &a, const fltx4 &b )
{
	fltx4 m = MulSIMD( a, b );
	float flDot = SubFloat( m, 0 ) + SubFloat( m, 1 ) + SubFloat( m, 2 );
	return ReplicateX4( flDot );
}

fltx4 Dot4SIMD( const fltx4 &a, const fltx4 &b )
{
	fltx4 m = MulSIMD( a, b );
	float flDot = SubFloat( m, 0 ) + SubFloat( m, 1 ) + SubFloat( m, 2 ) + SubFloat( m, 3 );
	return ReplicateX4( flDot );
}

fltx4 SinSIMD( const fltx4 &radians )
{
	fltx4 result;
	SubFloat( result, 0 ) = sin( SubFloat( radians, 0 ) );
	SubFloat( result, 1 ) = sin( SubFloat( radians, 1 ) );
	SubFloat( result, 2 ) = sin( SubFloat( radians, 2 ) );
	SubFloat( result, 3 ) = sin( SubFloat( radians, 3 ) );
	return result;
}

void SinCos3SIMD( fltx4 &sine, fltx4 &cosine, const fltx4 &radians )
{
	// FIXME: Make a fast SSE version
	SinCos( SubFloat( radians, 0 ), &SubFloat( sine, 0 ), &SubFloat( cosine, 0 ) );
	SinCos( SubFloat( radians, 1 ), &SubFloat( sine, 1 ), &SubFloat( cosine, 1 ) );
	SinCos( SubFloat( radians, 2 ), &SubFloat( sine, 2 ), &SubFloat( cosine, 2 ) );
}

void SinCosSIMD( fltx4 &sine, fltx4 &cosine, const fltx4 &radians )
{
	// FIXME: Make a fast SSE version
	SinCos( SubFloat( radians, 0 ), &SubFloat( sine, 0 ), &SubFloat( cosine, 0 ) );
	SinCos( SubFloat( radians, 1 ), &SubFloat( sine, 1 ), &SubFloat( cosine, 1 ) );
	SinCos( SubFloat( radians, 2 ), &SubFloat( sine, 2 ), &SubFloat( cosine, 2 ) );
	SinCos( SubFloat( radians, 3 ), &SubFloat( sine, 3 ), &SubFloat( cosine, 3 ) );
}

fltx4 ArcSinSIMD( const fltx4 &sine )
{
	// FIXME: Make a fast SSE version
	fltx4 result;
	SubFloat( result, 0 ) = asin( SubFloat( sine, 0 ) );
	SubFloat( result, 1 ) = asin( SubFloat( sine, 1 ) );
	SubFloat( result, 2 ) = asin( SubFloat( sine, 2 ) );
	SubFloat( result, 3 ) = asin( SubFloat( sine, 3 ) );
	return result;
}

fltx4 ArcCosSIMD( const fltx4 &cs )
{
	fltx4 result;
	SubFloat( result, 0 ) = acos( SubFloat( cs, 0 ) );
	SubFloat( result, 1 ) = acos( SubFloat( cs, 1 ) );
	SubFloat( result, 2 ) = acos( SubFloat( cs, 2 ) );
	SubFloat( result, 3 ) = acos( SubFloat( cs, 3 ) );
	return result;
}

fltx4 ArcTan2SIMD( const fltx4 &a, const fltx4 &b )
{
	fltx4 result;
	SubFloat( result, 0 ) = atan2( SubFloat( a, 0 ), SubFloat( b, 0 ) );
	SubFloat( result, 1 ) = atan2( SubFloat( a, 1 ), SubFloat( b, 1 ) );
	SubFloat( result, 2 ) = atan2( SubFloat( a, 2 ), SubFloat( b, 2 ) );
	SubFloat( result, 3 ) = atan2( SubFloat( a, 3 ), SubFloat( b, 3 ) );
	return result;
}

fltx4 NegSIMD(const fltx4 &a)
{
	return SubSIMD(LoadZeroSIMD(),a);
}

int TestSignSIMD( const fltx4 & a )
{
	return _mm_movemask_ps( a );
}

bool IsAnyNegative( const fltx4 & a )
{
	return (0 != TestSignSIMD( a ));
}

fltx4 CmpEqSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_cmpeq_ps( a, b );
}

fltx4 CmpGtSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_cmpgt_ps( a, b );
}

fltx4 CmpGeSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_cmpge_ps( a, b );
}

fltx4 CmpLtSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_cmplt_ps( a, b );
}

fltx4 CmpLeSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_cmple_ps( a, b );
}

bool IsAllGreaterThan( const fltx4 &a, const fltx4 &b )
{
	return	TestSignSIMD( CmpLeSIMD( a, b ) ) == 0;
}

bool IsAllGreaterThanOrEq( const fltx4 &a, const fltx4 &b )
{
	return	TestSignSIMD( CmpLtSIMD( a, b ) ) == 0;
}

bool IsAllEqual( const fltx4 & a, const fltx4 & b )
{
	return	TestSignSIMD( CmpEqSIMD( a, b ) ) == 0xf;
}

fltx4 CmpInBoundsSIMD( const fltx4 & a, const fltx4 & b )
{
	return AndSIMD( CmpLeSIMD(a,b), CmpGeSIMD(a, NegSIMD(b)) );
}

fltx4 MinSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_min_ps( a, b );
}

fltx4 MaxSIMD( const fltx4 & a, const fltx4 & b )
{
	return _mm_max_ps( a, b );
}

fltx4 CeilSIMD( const fltx4 &a )
{
	fltx4 retVal;
	SubFloat( retVal, 0 ) = ceil( SubFloat( a, 0 ) );
	SubFloat( retVal, 1 ) = ceil( SubFloat( a, 1 ) );
	SubFloat( retVal, 2 ) = ceil( SubFloat( a, 2 ) );
	SubFloat( retVal, 3 ) = ceil( SubFloat( a, 3 ) );
	return retVal;

}

fltx4 FloorSIMD( const fltx4 &val )
{
	fltx4 fl4Abs = fabs( val );
	fltx4 ival = SubSIMD( AddSIMD( fl4Abs, Four_2ToThe23s ), Four_2ToThe23s );
	ival = MaskedAssign( CmpGtSIMD( ival, fl4Abs ), SubSIMD( ival, Four_Ones ), ival );
	return XorSIMD( ival, XorSIMD( val, fl4Abs ) );			// restore sign bits
}

bool IsAnyZeros( const fltx4 & a )
{
	return TestSignSIMD( CmpEqSIMD( a, Four_Zeros ) ) != 0;
}

bool IsAllZeros( const fltx4 & var )
{
	return TestSignSIMD( CmpEqSIMD( var, Four_Zeros ) ) == 0xF;
}

fltx4 SqrtEstSIMD( const fltx4 & a )
{
	return _mm_sqrt_ps( a );
}

fltx4 SqrtSIMD( const fltx4 & a )
{
	return _mm_sqrt_ps( a );
}

fltx4 ReciprocalSqrtEstSIMD( const fltx4 & a )
{
	return _mm_rsqrt_ps( a );
}

fltx4 ReciprocalSqrtEstSaturateSIMD( const fltx4 & a )
{
	fltx4 zero_mask = CmpEqSIMD( a, Four_Zeros );
	fltx4 ret = OrSIMD( a, AndSIMD( Four_Epsilons, zero_mask ) );
	ret = ReciprocalSqrtEstSIMD( ret );
	return ret;
}

fltx4 ReciprocalSqrtSIMD( const fltx4 & a )
{
	fltx4 guess = ReciprocalSqrtEstSIMD( a );
	// newton iteration for 1/sqrt(a) : y(n+1) = 1/2 (y(n)*(3-a*y(n)^2));
	guess = MulSIMD( guess, SubSIMD( Four_Threes, MulSIMD( a, MulSIMD( guess, guess ))));
	guess = MulSIMD( Four_PointFives, guess);
	return guess;
}

fltx4 ReciprocalEstSIMD( const fltx4 & a )
{
	return _mm_rcp_ps( a );
}

fltx4 ReciprocalEstSaturateSIMD( const fltx4 & a )
{
	fltx4 zero_mask = CmpEqSIMD( a, Four_Zeros );
	fltx4 ret = OrSIMD( a, AndSIMD( Four_Epsilons, zero_mask ) );
	ret = ReciprocalEstSIMD( ret );
	return ret;
}

fltx4 ReciprocalSIMD( const fltx4 & a )
{
	fltx4 ret = ReciprocalEstSIMD( a );
	// newton iteration is: Y(n+1) = 2*Y(n)-a*Y(n)^2
	ret = SubSIMD( AddSIMD( ret, ret ), MulSIMD( a, MulSIMD( ret, ret ) ) );
	return ret;
}

fltx4 ReciprocalSaturateSIMD( const fltx4 & a )
{
	fltx4 zero_mask = CmpEqSIMD( a, Four_Zeros );
	fltx4 ret = OrSIMD( a, AndSIMD( Four_Epsilons, zero_mask ) );
	ret = ReciprocalSIMD( ret );
	return ret;
}

fltx4 ExpSIMD( const fltx4 &toPower )
{
	fltx4 retval;
	SubFloat( retval, 0 ) = powf( 2, SubFloat(toPower, 0) );
	SubFloat( retval, 1 ) = powf( 2, SubFloat(toPower, 1) );
	SubFloat( retval, 2 ) = powf( 2, SubFloat(toPower, 2) );
	SubFloat( retval, 3 ) = powf( 2, SubFloat(toPower, 3) );

	return retval;
}

fltx4 ClampVectorSIMD( FLTX4 in, FLTX4 min, FLTX4 max)
{
	return MaxSIMD( min, MinSIMD( max, in ) );
}

void TransposeSIMD( fltx4 & x, fltx4 & y, fltx4 & z, fltx4 & w)
{
	_MM_TRANSPOSE4_PS( x, y, z, w );
}

fltx4 FindLowestSIMD3( const fltx4 &a )
{
	// a is [x,y,z,G] (where G is garbage)
	// rotate left by one 
	fltx4 compareOne = RotateLeft( a );
	// compareOne is [y,z,G,x]
	fltx4 retval = MinSIMD( a, compareOne );
	// retVal is [min(x,y), ... ]
	compareOne = RotateLeft2( a );
	// compareOne is [z, G, x, y]
	retval = MinSIMD( retval, compareOne );
	// retVal = [ min(min(x,y),z)..]
	// splat the x component out to the whole vector and return
	return SplatXSIMD( retval );
	
}

fltx4 FindHighestSIMD3( const fltx4 &a )
{
	// a is [x,y,z,G] (where G is garbage)
	// rotate left by one 
	fltx4 compareOne = RotateLeft( a );
	// compareOne is [y,z,G,x]
	fltx4 retval = MaxSIMD( a, compareOne );
	// retVal is [max(x,y), ... ]
	compareOne = RotateLeft2( a );
	// compareOne is [z, G, x, y]
	retval = MaxSIMD( retval, compareOne );
	// retVal = [ max(max(x,y),z)..]
	// splat the x component out to the whole vector and return
	return SplatXSIMD( retval );
	
}

i32x4 LoadAlignedIntSIMD( const void * RESTRICT pSIMD)
{
	return _mm_load_ps( reinterpret_cast<const float *>(pSIMD) );
}

i32x4 LoadUnalignedIntSIMD( const void * RESTRICT pSIMD)
{
	return _mm_loadu_ps( reinterpret_cast<const float *>(pSIMD) );
}

void StoreAlignedIntSIMD( int32_t * RESTRICT pSIMD, const fltx4 & a )
{
	_mm_store_ps( reinterpret_cast<float *>(pSIMD), a );
}

void StoreAlignedIntSIMD( intx4 &pSIMD, const fltx4 & a )
{
	_mm_store_ps( reinterpret_cast<float *>(pSIMD.Base()), a );
}

void StoreUnalignedIntSIMD( int32_t * RESTRICT pSIMD, const fltx4 & a )
{
	_mm_storeu_ps( reinterpret_cast<float *>(pSIMD), a );
}

fltx4 CompressSIMD( fltx4 const & a, fltx4 const &b )
{
	return _mm_shuffle_ps( a, b, MM_SHUFFLE_REV( 0, 2, 0, 2 ) );
}

fltx4 LoadAndConvertUint16SIMD( const uint16_t *pInts )
{
#ifdef POSIX
	fltx4 retval;
	SubFloat( retval, 0 ) = pInts[0];
	SubFloat( retval, 1 ) = pInts[1];
	SubFloat( retval, 2 ) = pInts[2];
	SubFloat( retval, 3 ) = pInts[3];
	return retval;
#else
	__m128i inA = _mm_loadl_epi64( (__m128i const*) pInts); // Load the lower 64 bits of the value pointed to by p into the lower 64 bits of the result, zeroing the upper 64 bits of the result.
	inA = _mm_unpacklo_epi16( inA, _mm_setzero_si128() ); // unpack unsigned 16's to signed 32's
	return _mm_cvtepi32_ps(inA);
#endif
}

fltx4 Compress4SIMD( fltx4 const a, fltx4 const &b, fltx4 const &c, fltx4 const &d )
{
	fltx4 aacc = _mm_shuffle_ps( a, c, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
	fltx4 bbdd = _mm_shuffle_ps( b, d, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
	return MaskedAssign( LoadAlignedSIMD( g_SIMD_EveryOtherMask ), bbdd, aacc );
}

void ExpandSIMD( fltx4 const &a, fltx4 &fl4OutA, fltx4 &fl4OutB )
{
	fl4OutA = _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 0, 0, 1, 1 ) );
	fl4OutB = _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 2, 3, 3 ) );

}

fltx4 UnsignedIntConvertToFltSIMD( const u32x4 &vSrcA )
{
	fltx4 retval;
	SubFloat( retval, 0 ) = ( (float) SubInt( retval, 0 ) );
	SubFloat( retval, 1 ) = ( (float) SubInt( retval, 1 ) );
	SubFloat( retval, 2 ) = ( (float) SubInt( retval, 2 ) );
	SubFloat( retval, 3 ) = ( (float) SubInt( retval, 3 ) );
	return retval;
}

fltx4 SignedIntConvertToFltSIMD( const i32x4 &vSrcA )
{
	fltx4 retval;
	SubFloat( retval, 0 ) = ( (float) (reinterpret_cast<const int32_t *>(&vSrcA)[0]));
	SubFloat( retval, 1 ) = ( (float) (reinterpret_cast<const int32_t *>(&vSrcA)[1]));
	SubFloat( retval, 2 ) = ( (float) (reinterpret_cast<const int32_t *>(&vSrcA)[2]));
	SubFloat( retval, 3 ) = ( (float) (reinterpret_cast<const int32_t *>(&vSrcA)[3]));
	return retval;
}

i32x4 IntShiftLeftWordSIMD(const i32x4 &vSrcA, const i32x4 &vSrcB)
{
	i32x4 retval;
	SubInt(retval, 0) = SubInt(vSrcA, 0) << SubInt(vSrcB, 0);
	SubInt(retval, 1) = SubInt(vSrcA, 1) << SubInt(vSrcB, 1);
	SubInt(retval, 2) = SubInt(vSrcA, 2) << SubInt(vSrcB, 2);
	SubInt(retval, 3) = SubInt(vSrcA, 3) << SubInt(vSrcB, 3);

	return retval;
}

void ConvertStoreAsIntsSIMD(intx4 * RESTRICT pDest, const fltx4 &vSrc)
{
#if defined(_MSC_VER) && _MSC_VER >= 1900 && defined(COMPILER_MSVC64)
	(*pDest)[0] = (int)SubFloat(vSrc, 0);
	(*pDest)[1] = (int)SubFloat(vSrc, 1);
	(*pDest)[2] = (int)SubFloat(vSrc, 2);
	(*pDest)[3] = (int)SubFloat(vSrc, 3);
#else
	__m64 bottom = _mm_cvttps_pi32(vSrc);
	__m64 top = _mm_cvttps_pi32(_mm_movehl_ps(vSrc, vSrc));

	*reinterpret_cast<__m64 *>(&(*pDest)[0]) = bottom;
	*reinterpret_cast<__m64 *>(&(*pDest)[2]) = top;

	_mm_empty();
#endif
}

void RotateLeftDoubleSIMD( fltx4 &a, fltx4 &b )
{
	a = SetWSIMD( RotateLeft( a ), SplatXSIMD( b ) );
	b = RotateLeft( b );
}

#if !defined( POSIX )
fltx4 operator+( FLTX4 a, FLTX4 b )
{
	return AddSIMD( a, b );
}

fltx4 operator-( FLTX4 a, FLTX4 b )
{
	return SubSIMD( a, b );
}

fltx4 operator*( FLTX4 a, FLTX4 b )
{
	return MulSIMD( a, b );
}

fltx4 operator&( FLTX4 a, FLTX4 b )
{
	return AndSIMD( a ,b );
}

fltx4 operator|( FLTX4 a, FLTX4 b )
{
	return OrSIMD( a, b );
}

fltx4 operator^( FLTX4 a, FLTX4 b )
{
	return XorSIMD( a, b );
}

fltx4 operator-( FLTX4 a )
{
	return NegSIMD( a );
}
#endif

FourVectors::FourVectors(void)
{
}

FourVectors::FourVectors( FourVectors const &src )
{
	x=src.x;
	y=src.y;
	z=src.z;
}

FourVectors::FourVectors( float a )
{
	fltx4 aReplicated = ReplicateX4( a );
	x = y = z = aReplicated;
}

FourVectors::FourVectors(Vector const &a, Vector const &b, Vector const &c, Vector const &d)
{
	LoadAndSwizzle(a,b,c,d);
}

FourVectors::FourVectors(VectorAligned const &a, VectorAligned const &b, VectorAligned const &c, VectorAligned const &d)
{
	LoadAndSwizzleAligned(a,b,c,d);
}

FourVectors::FourVectors( const float *xs , const float *ys, const float *zs ) :
	x( LoadAlignedSIMD(xs) ), y( LoadAlignedSIMD(ys) ), z( LoadAlignedSIMD(zs) ) 
{
}

void FourVectors::DuplicateVector(Vector const &v)
{
	x=ReplicateX4(v.x);
	y=ReplicateX4(v.y);
	z=ReplicateX4(v.z);
}

fltx4 const & FourVectors::operator[](int idx) const
{
	return *((&x)+idx);
}

fltx4 & FourVectors::operator[](int idx)
{
	return *((&x)+idx);
}

void FourVectors::operator+=(FourVectors const &b)
{
	x=AddSIMD(x,b.x);
	y=AddSIMD(y,b.y);
	z=AddSIMD(z,b.z);
}

FourVectors FourVectors::operator+(FourVectors const &b)
{
	FourVectors result;
	result.x=AddSIMD(x,b.x);
	result.y=AddSIMD(y,b.y);
	result.z=AddSIMD(z,b.z);
	return result;
}

void FourVectors::operator-=(FourVectors const &b)
{
	x=SubSIMD(x,b.x);
	y=SubSIMD(y,b.y);
	z=SubSIMD(z,b.z);
}

FourVectors FourVectors::operator-(FourVectors const &b)
{
	FourVectors result;
	result.x=SubSIMD(x,b.x);
	result.y=SubSIMD(y,b.y);
	result.z=SubSIMD(z,b.z);
	return result;
}

void FourVectors::operator*=(FourVectors const &b)
{
	x=MulSIMD(x,b.x);
	y=MulSIMD(y,b.y);
	z=MulSIMD(z,b.z);
}

void FourVectors::operator*=(const fltx4 & scale)
{
	x=MulSIMD(x,scale);
	y=MulSIMD(y,scale);
	z=MulSIMD(z,scale);
}

void FourVectors::operator*=(float scale)
{
	fltx4 scalepacked = ReplicateX4(scale);
	*this *= scalepacked;
}

fltx4 FourVectors::operator*(FourVectors const &b) const
{
	fltx4 dot=MulSIMD(x,b.x);
	dot=MaddSIMD(y,b.y,dot);
	dot=MaddSIMD(z,b.z,dot);
	return dot;
}

fltx4 FourVectors::operator*(Vector const &b) const
{
	fltx4 dot=MulSIMD(x,ReplicateX4(b.x));
	dot=MaddSIMD(y,ReplicateX4(b.y), dot);
	dot=MaddSIMD(z,ReplicateX4(b.z), dot);
	return dot;
}

FourVectors FourVectors::operator*(float b) const
{
	fltx4 scalepacked = ReplicateX4( b );
	FourVectors res;
	res.x = MulSIMD( x, scalepacked );
	res.y = MulSIMD( y, scalepacked );
	res.z = MulSIMD( z, scalepacked );
	return res;
}

void FourVectors::VProduct(FourVectors const &b)
{
	x=MulSIMD(x,b.x);
	y=MulSIMD(y,b.y);
	z=MulSIMD(z,b.z);
}

void FourVectors::MakeReciprocal(void)
{
	x=ReciprocalSIMD(x);
	y=ReciprocalSIMD(y);
	z=ReciprocalSIMD(z);
}

void FourVectors::MakeReciprocalSaturate(void)
{
	x=ReciprocalSaturateSIMD(x);
	y=ReciprocalSaturateSIMD(y);
	z=ReciprocalSaturateSIMD(z);
}

const float & FourVectors::X(int idx) const
{
	// NOTE: if the output goes into a register, this causes a Load-Hit-Store stall (don't mix fpu/vpu math!)
	return SubFloat( (fltx4 &)x, idx );
}

const float & FourVectors::Y(int idx) const
{
	return SubFloat( (fltx4 &)y, idx );
}

const float & FourVectors::Z(int idx) const
{
	return SubFloat( (fltx4 &)z, idx );
}

float & FourVectors::X(int idx)
{
	return SubFloat( x, idx );
}

float & FourVectors::Y(int idx)
{
	return SubFloat( y, idx );
}

float & FourVectors::Z(int idx)
{
	return SubFloat( z, idx );
}

Vector FourVectors::Vec(int idx) const
{
	return Vector( X(idx), Y(idx), Z(idx) );
}

void FourVectors::operator=( FourVectors const &src )
{
	x=src.x;
	y=src.y;
	z=src.z;
}

void FourVectors::LoadAndSwizzle(Vector const &a, Vector const &b, Vector const &c, Vector const &d)
{
	// use an unfolded implementation here
	x		= LoadUnalignedSIMD( &( a.x ));
	y		= LoadUnalignedSIMD( &( b.x ));
	z		= LoadUnalignedSIMD( &( c.x ));
	fltx4 w = LoadUnalignedSIMD( &( d.x ));
	// now, matrix is:
	// x y z ?
	// x y z ?
	// x y z ?
	// x y z ?
	TransposeSIMD(x, y, z, w);
}

void FourVectors::LoadAndSwizzle(Vector const &a)
{
	LoadAndSwizzle( a, a, a, a );
}

void FourVectors::Load( const float &a, const float &b, const float &c, const float &d )
{
	ALIGN16 float temp[4];
	temp[0] = a; temp[1] = b; temp[2] = c; temp[3] = d;
	fltx4 v = LoadAlignedSIMD( temp );
	x = v;
	y = v;
	z = v;
}

void FourVectors::LoadAndSwizzle( FLTX4 a, FLTX4 b, FLTX4 c, FLTX4 d  )
{
	x		= a;
	y		= b;
	z		= c;
	fltx4 w = d;
	// now, matrix is:
	// x y z ?
	// x y z ?
	// x y z ?
	// x y z ?
	TransposeSIMD(x, y, z, w);
}

void FourVectors::LoadAndSwizzleAligned(const float *RESTRICT a, const float *RESTRICT b, const float *RESTRICT c, const float *RESTRICT d)
{
	x		= LoadAlignedSIMD( a );
	y		= LoadAlignedSIMD( b );
	z		= LoadAlignedSIMD( c );
	fltx4 w = LoadAlignedSIMD( d );
	// now, matrix is:
	// x y z ?
	// x y z ?
	// x y z ?
	// x y z ?
	TransposeSIMD( x, y, z, w );
}

void FourVectors::LoadAndSwizzleAligned(Vector const &a, Vector const &b, Vector const &c, Vector const &d)
{
	LoadAndSwizzleAligned( &a.x, &b.x, &c.x, &d.x );
}

void FourVectors::TransposeOnto( fltx4 &out0,  fltx4 &out1,  fltx4 &out2,  fltx4 &out3, FLTX4 w) const
{
	// use an unfolded implementation here
	out0 = x;
	out1 = y;
	out2 = z;
	out3 = w;

	TransposeSIMD(out0, out1, out2, out3);
}

void FourVectors::StoreUnalignedContigVector3SIMD( Vector * RESTRICT pDestination )
{
	fltx4 a,b,c,d;
	TransposeOnto(a,b,c,d);
	StoreFourUnalignedVector3SIMD( a, b, c, d, pDestination );
}

void FourVectors::StoreAlignedContigVector3SIMD( Vector * RESTRICT pDestination )
{
	fltx4 a,b,c,d;
	TransposeOnto(a,b,c,d);
	StoreFourAlignedVector3SIMD( a, b, c, d, pDestination );
}

void FourVectors::StoreAlignedContigVectorASIMD( VectorAligned * RESTRICT pDestination )
{
	StoreAlignedVectorSIMD( pDestination, pDestination + 1, pDestination + 2, pDestination + 3 );
}

fltx4 FourVectors::length2(void) const
{
	return (*this)*(*this);
}

fltx4 FourVectors::length(void) const
{
	return SqrtEstSIMD(length2());
}

void FourVectors::VectorNormalizeFast(void)
{
	fltx4 mag_sq=(*this)*(*this);						// length^2
	(*this) *= ReciprocalSqrtEstSIMD(mag_sq);			// *(1.0/sqrt(length^2))
}

void FourVectors::VectorNormalize(void)
{
	fltx4 mag_sq=(*this)*(*this);						// length^2
	(*this) *= ReciprocalSqrtSIMD(mag_sq);				// *(1.0/sqrt(length^2))
}

fltx4 FourVectors::DistToSqr( FourVectors const &pnt )
{
	fltx4 fl4dX = SubSIMD( pnt.x, x );
	fltx4 fl4dY = SubSIMD( pnt.y, y );
	fltx4 fl4dZ = SubSIMD( pnt.z, z );
	return AddSIMD( MulSIMD( fl4dX, fl4dX), AddSIMD( MulSIMD( fl4dY, fl4dY ), MulSIMD( fl4dZ, fl4dZ ) ) );

}

fltx4 FourVectors::TValueOfClosestPointOnLine( FourVectors const &p0, FourVectors const &p1 ) const
{
	FourVectors lineDelta = p1;
	lineDelta -= p0;
	fltx4 OOlineDirDotlineDir = ReciprocalSIMD( p1 * p1 );
	FourVectors v4OurPnt = *this;
	v4OurPnt -= p0;
	return MulSIMD( OOlineDirDotlineDir, v4OurPnt * lineDelta );
}

fltx4 FourVectors::DistSqrToLineSegment( FourVectors const &p0, FourVectors const &p1 ) const
{
	FourVectors lineDelta = p1;
	FourVectors v4OurPnt = *this;
	v4OurPnt -= p0;
	lineDelta -= p0;

	fltx4 OOlineDirDotlineDir = ReciprocalSIMD( lineDelta * lineDelta );

	fltx4 fl4T = MulSIMD( OOlineDirDotlineDir, v4OurPnt * lineDelta );

	fl4T = MinSIMD( fl4T, Four_Ones );
	fl4T = MaxSIMD( fl4T, Four_Zeros );
	lineDelta *= fl4T;
	return v4OurPnt.DistToSqr( lineDelta );
}

FourVectors Mul( const FourVectors &a, const fltx4 &b )
{
	FourVectors ret;
	ret.x = MulSIMD( a.x, b );
	ret.y = MulSIMD( a.y, b );
	ret.z = MulSIMD( a.z, b );
	return ret;
}

FourVectors Mul( const FourVectors &a, const FourVectors &b )
{
	FourVectors ret;
	ret.x = MulSIMD( a.x, b.x );
	ret.y = MulSIMD( a.y, b.y );
	ret.z = MulSIMD( a.z, b.z );
	return ret;
}

FourVectors Madd( const FourVectors &a, const fltx4 &b, const FourVectors &c )
{
	FourVectors ret;
	ret.x = MaddSIMD( a.x, b, c.x );
	ret.y = MaddSIMD( a.y, b, c.y );
	ret.z = MaddSIMD( a.z, b, c.z );
	return ret;
}

FourVectors operator ^(const FourVectors &a, const FourVectors &b)
{
	FourVectors ret;
	ret.x=SubSIMD(MulSIMD(a.y,b.z),MulSIMD(a.z,b.y));
	ret.y=SubSIMD(MulSIMD(a.z,b.x),MulSIMD(a.x,b.z));
	ret.z=SubSIMD(MulSIMD(a.x,b.y),MulSIMD(a.y,b.x));
	return ret;
}

FourVectors operator-(const FourVectors &a, const FourVectors &b)
{
	FourVectors ret;
	ret.x=SubSIMD(a.x,b.x);
	ret.y=SubSIMD(a.y,b.y);
	ret.z=SubSIMD(a.z,b.z);
	return ret;
}

FourVectors maximum(const FourVectors &a, const FourVectors &b)
{
	FourVectors ret;
	ret.x=MaxSIMD(a.x,b.x);
	ret.y=MaxSIMD(a.y,b.y);
	ret.z=MaxSIMD(a.z,b.z);
	return ret;
}

FourVectors minimum(const FourVectors &a, const FourVectors &b)
{
	FourVectors ret;
	ret.x=MinSIMD(a.x,b.x);
	ret.y=MinSIMD(a.y,b.y);
	ret.z=MinSIMD(a.z,b.z);
	return ret;
}

FourVectors RotateLeft( const FourVectors &src )
{
	FourVectors ret;
	ret.x = RotateLeft( src.x );
	ret.y = RotateLeft( src.y );
	ret.z = RotateLeft( src.z );
	return ret;
}

FourVectors RotateRight( const FourVectors &src )
{
	FourVectors ret;
	ret.x = RotateRight( src.x );
	ret.y = RotateRight( src.y );
	ret.z = RotateRight( src.z );
	return ret;
}

FourVectors MaskedAssign( const fltx4 & ReplacementMask, const FourVectors & NewValue, const FourVectors & OldValue )
{
	FourVectors ret;
	ret.x = MaskedAssign( ReplacementMask, NewValue.x, OldValue.x );
	ret.y = MaskedAssign( ReplacementMask, NewValue.y, OldValue.y );
	ret.z = MaskedAssign( ReplacementMask, NewValue.z, OldValue.z );
	return ret;
}

FourVectors VectorReflect( const FourVectors &incident, const FourVectors &normal )
{
	FourVectors ret = incident;
	fltx4 iDotNx2 = incident * normal;
	iDotNx2 = AddSIMD( iDotNx2, iDotNx2 );
	FourVectors nPart = normal;
	nPart *= iDotNx2;
	ret -= nPart;											// i-2(n*i)n
	return ret;
}

FourVectors VectorSlide( const FourVectors &incident, const FourVectors &normal )
{
	FourVectors ret = incident;
	fltx4 iDotN = incident * normal;
	FourVectors nPart = normal;
	nPart *= iDotN;
	ret -= nPart;											// i-(n*i)n
	return ret;
}

FourVectors VectorNormalizeFast( const FourVectors &src )
{
	fltx4 mag_sq = ReciprocalSqrtEstSIMD( src * src );					// *(1.0/sqrt(length^2))
	FourVectors result;
	result.x = MulSIMD( src.x, mag_sq );			
	result.y = MulSIMD( src.y, mag_sq );			
	result.z = MulSIMD( src.z, mag_sq );			
	return result;
}

void FourVectors::StoreUnalignedVector3SIMD( Vector * RESTRICT out0, Vector * RESTRICT out1, Vector * RESTRICT out2, Vector * RESTRICT out3 ) const
{
	fltx4 a,b,c,d;
	TransposeOnto(a,b,c,d);
	StoreUnaligned3SIMD( out0->Base(), a );
	StoreUnaligned3SIMD( out1->Base(), b );
	StoreUnaligned3SIMD( out2->Base(), c );
	StoreUnaligned3SIMD( out3->Base(), d );
}

void FourVectors::StoreAlignedVectorSIMD( VectorAligned * RESTRICT out0, VectorAligned * RESTRICT out1, VectorAligned * RESTRICT out2, VectorAligned * RESTRICT out3 ) const
{
	fltx4 a,b,c,d;
	TransposeOnto(a,b,c,d);
	StoreAligned3SIMD( out0, a );
	StoreAligned3SIMD( out1, b );
	StoreAligned3SIMD( out2, c );
	StoreAligned3SIMD( out3, d );

}

void FourVectors::RotateBy(const matrix3x4_t& matrix)
{
	// Splat out each of the entries in the matrix to a fltx4. Do this
	// in the order that we will need them, to hide latency. I'm
	// avoiding making an array of them, so that they'll remain in 
	// registers.
	fltx4 matSplat00, matSplat01, matSplat02,
		matSplat10, matSplat11, matSplat12,
		matSplat20, matSplat21, matSplat22;

	// Load the matrix into local vectors. Sadly, matrix3x4_ts are 
	// often unaligned. The w components will be the tranpose row of
	// the matrix, but we don't really care about that.
	fltx4 matCol0 = LoadUnalignedSIMD( matrix[0] );
	fltx4 matCol1 = LoadUnalignedSIMD( matrix[1] );
	fltx4 matCol2 = LoadUnalignedSIMD( matrix[2] );
	
	matSplat00 = SplatXSIMD( matCol0 );
	matSplat01 = SplatYSIMD( matCol0 );
	matSplat02 = SplatZSIMD( matCol0 );

	matSplat10 = SplatXSIMD( matCol1 );
	matSplat11 = SplatYSIMD( matCol1 );
	matSplat12 = SplatZSIMD( matCol1 );
	
	matSplat20 = SplatXSIMD( matCol2 );
	matSplat21 = SplatYSIMD( matCol2 );
	matSplat22 = SplatZSIMD( matCol2 );

	// Trust in the compiler to schedule these operations correctly:
	fltx4 outX, outY, outZ;
	outX = AddSIMD( AddSIMD( MulSIMD( x, matSplat00 ), MulSIMD( y, matSplat01 ) ), MulSIMD( z, matSplat02 ) );
	outY = AddSIMD( AddSIMD( MulSIMD( x, matSplat10 ), MulSIMD( y, matSplat11 ) ), MulSIMD( z, matSplat12 ) );
	outZ = AddSIMD( AddSIMD( MulSIMD( x, matSplat20 ), MulSIMD( y, matSplat21 ) ), MulSIMD( z, matSplat22 ) );
	
	x = outX;
	y = outY;
	z = outZ;
}

void FourVectors::TransformBy(const matrix3x4_t& matrix)
{
	// Splat out each of the entries in the matrix to a fltx4. Do this
	// in the order that we will need them, to hide latency. I'm
	// avoiding making an array of them, so that they'll remain in 
	// registers.
	fltx4 matSplat00, matSplat01, matSplat02,
		matSplat10, matSplat11, matSplat12,
		matSplat20, matSplat21, matSplat22;

	// Load the matrix into local vectors. Sadly, matrix3x4_ts are 
	// often unaligned. The w components will be the tranpose row of
	// the matrix, but we don't really care about that.
	fltx4 matCol0 = LoadUnalignedSIMD( matrix[0] );
	fltx4 matCol1 = LoadUnalignedSIMD( matrix[1] );
	fltx4 matCol2 = LoadUnalignedSIMD( matrix[2] );
	
	matSplat00 = SplatXSIMD( matCol0 );
	matSplat01 = SplatYSIMD( matCol0 );
	matSplat02 = SplatZSIMD( matCol0 );
	
	matSplat10 = SplatXSIMD( matCol1 );
	matSplat11 = SplatYSIMD( matCol1 );
	matSplat12 = SplatZSIMD( matCol1 );
	
	matSplat20 = SplatXSIMD( matCol2 );
	matSplat21 = SplatYSIMD( matCol2 );
	matSplat22 = SplatZSIMD( matCol2 );
	
	// Trust in the compiler to schedule these operations correctly:
	fltx4 outX, outY, outZ;
	
	outX = MaddSIMD( z, matSplat02, AddSIMD( MulSIMD( x, matSplat00 ), MulSIMD( y, matSplat01 ) ) );
	outY = MaddSIMD( z, matSplat12, AddSIMD( MulSIMD( x, matSplat10 ), MulSIMD( y, matSplat11 ) ) );
	outZ = MaddSIMD( z, matSplat22, AddSIMD( MulSIMD( x, matSplat20 ), MulSIMD( y, matSplat21 ) ) );
	
	x = AddSIMD( outX, ReplicateX4( matrix[0][3] ));
	y = AddSIMD( outY, ReplicateX4( matrix[1][3] ));
	 z = AddSIMD( outZ, ReplicateX4( matrix[2][3] ));
}

fltx4 fabs( const fltx4 & x )
{
	return AndSIMD( x, LoadAlignedSIMD( g_SIMD_clear_signmask ) );
}

fltx4 fnegate( const fltx4 & x )
{
	return XorSIMD( x, LoadAlignedSIMD( g_SIMD_signmask ) );
}

fltx4 PowSIMD( const fltx4 & x, float exponent )
{
	return Pow_FixedPoint_Exponent_SIMD(x,(int) (4.0*exponent));
}

fltx4 RandSignedSIMD( void )
{
	return SubSIMD( MulSIMD( Four_Twos, RandSIMD() ), Four_Ones );
}

fltx4 LerpSIMD ( const fltx4 &percent, const fltx4 &a, const fltx4 &b)
{
	return AddSIMD( a, MulSIMD( SubSIMD( b, a ), percent ) );
}

fltx4 RemapValClampedSIMD(const fltx4 &val, const fltx4 &a, const fltx4 &b, const fltx4 &c, const fltx4 &d)
{
	fltx4 range = MaskedAssign( CmpEqSIMD( a, b ), Four_Ones, SubSIMD( b, a ) ); //make sure range > 0
	fltx4 cVal = MaxSIMD( Four_Zeros, MinSIMD( Four_Ones, DivSIMD( SubSIMD( val, a ), range ) ) ); //saturate
	return LerpSIMD( cVal, c, d );
}

fltx4 SimpleSpline( const fltx4 & value )
{
	// Arranged to avoid a data dependency between these two MULs:
	fltx4 valueDoubled = MulSIMD( value, Four_Twos );
	fltx4 valueSquared = MulSIMD( value, value );

	// Nice little ease-in, ease-out spline-like curve
	return SubSIMD(
		MulSIMD( Four_Threes,  valueSquared ),
		MulSIMD( valueDoubled, valueSquared ) );
}

fltx4 SimpleSplineRemapValWithDeltas( const fltx4 & val,
											 const fltx4 & A, const fltx4 & BMinusA,
											 const fltx4 & OneOverBMinusA, const fltx4 & C,
											 const fltx4 & DMinusC )
{
// 	if ( A == B )
// 		return val >= B ? D : C;
	fltx4 cVal = MulSIMD( SubSIMD( val, A), OneOverBMinusA );
	return AddSIMD( C, MulSIMD( DMinusC, SimpleSpline( cVal ) ) );
}

fltx4 SimpleSplineRemapValWithDeltasClamped( const fltx4 & val,
													const fltx4 & A, const fltx4 & BMinusA,
													const fltx4 & OneOverBMinusA, const fltx4 & C,
													const fltx4 & DMinusC )
{
// 	if ( A == B )
// 		return val >= B ? D : C;
	fltx4 cVal = MulSIMD( SubSIMD( val, A), OneOverBMinusA );
	cVal = MinSIMD( Four_Ones, MaxSIMD( Four_Zeros, cVal ) );
	return AddSIMD( C, MulSIMD( DMinusC, SimpleSpline( cVal ) ) );
}

fltx4 FracSIMD( const fltx4 &val )
{
	fltx4 fl4Abs = fabs( val );
	fltx4 ival = SubSIMD( AddSIMD( fl4Abs, Four_2ToThe23s ), Four_2ToThe23s );
	ival = MaskedAssign( CmpGtSIMD( ival, fl4Abs ), SubSIMD( ival, Four_Ones ), ival );
	return XorSIMD( SubSIMD( fl4Abs, ival ), XorSIMD( val, fl4Abs ) );			// restore sign bits
}

fltx4 Mod2SIMD( const fltx4 &val )
{
	fltx4 fl4Abs = fabs( val );
	fltx4 ival = SubSIMD( AndSIMD( LoadAlignedSIMD( (float *) g_SIMD_lsbmask ), AddSIMD( fl4Abs, Four_2ToThe23s ) ), Four_2ToThe23s );
	ival = MaskedAssign( CmpGtSIMD( ival, fl4Abs ), SubSIMD( ival, Four_Twos ), ival );
	return XorSIMD( SubSIMD( fl4Abs, ival ), XorSIMD( val, fl4Abs ) );			// restore sign bits
}

fltx4 Mod2SIMDPositiveInput( const fltx4 &val )
{
	fltx4 ival = SubSIMD( AndSIMD( LoadAlignedSIMD( g_SIMD_lsbmask ), AddSIMD( val, Four_2ToThe23s ) ), Four_2ToThe23s );
	ival = MaskedAssign( CmpGtSIMD( ival, val ), SubSIMD( ival, Four_Twos ), ival );
	return SubSIMD( val, ival );
}

fltx4 _SinEst01SIMD( const fltx4 &val )
{
	// really rough approximation - x*(4-x*4) - a parabola. s(0) = 0, s(.5) = 1, s(1)=0, smooth in-between.
	// sufficient for simple oscillation.
	return MulSIMD( val, SubSIMD( Four_Fours, MulSIMD( val, Four_Fours ) ) );
}

fltx4 _Sin01SIMD( const fltx4 &val )
{
	// not a bad approximation : parabola always over-estimates. Squared parabola always
	// underestimates. So lets blend between them:  goodsin = badsin + .225*( badsin^2-badsin)
	fltx4 fl4BadEst = MulSIMD( val, SubSIMD( Four_Fours, MulSIMD( val, Four_Fours ) ) );
	return AddSIMD( MulSIMD( Four_Point225s, SubSIMD( MulSIMD( fl4BadEst, fl4BadEst ), fl4BadEst ) ), fl4BadEst );
}

fltx4 SinEst01SIMD( const fltx4 &val )
{
	fltx4 fl4Abs = fabs( val );
	fltx4 fl4Reduced2 = Mod2SIMDPositiveInput( fl4Abs );
	fltx4 fl4OddMask = CmpGeSIMD( fl4Reduced2, Four_Ones );
	fltx4 fl4val = SubSIMD( fl4Reduced2, AndSIMD( Four_Ones, fl4OddMask ) );
	fltx4 fl4Sin = _SinEst01SIMD( fl4val );
	fl4Sin = XorSIMD( fl4Sin, AndSIMD( LoadAlignedSIMD( g_SIMD_signmask ), XorSIMD( val, fl4OddMask ) ) );
	return fl4Sin;

}

fltx4 Sin01SIMD( const fltx4 &val )
{
	fltx4 fl4Abs = fabs( val );
	fltx4 fl4Reduced2 = Mod2SIMDPositiveInput( fl4Abs );
	fltx4 fl4OddMask = CmpGeSIMD( fl4Reduced2, Four_Ones );
	fltx4 fl4val = SubSIMD( fl4Reduced2, AndSIMD( Four_Ones, fl4OddMask ) );
	fltx4 fl4Sin = _Sin01SIMD( fl4val );
	fl4Sin = XorSIMD( fl4Sin, AndSIMD( LoadAlignedSIMD( g_SIMD_signmask ), XorSIMD( val, fl4OddMask ) ) );
	return fl4Sin;

}

fltx4 NatExpSIMD( const fltx4 &val )
{
	// need to write this. just stub with normal float implementation for now
	fltx4 fl4Result;
	SubFloat( fl4Result, 0 ) = exp( SubFloat( val, 0 ) );
	SubFloat( fl4Result, 1 ) = exp( SubFloat( val, 1 ) );
	SubFloat( fl4Result, 2 ) = exp( SubFloat( val, 2 ) );
	SubFloat( fl4Result, 3 ) = exp( SubFloat( val, 3 ) );
	return fl4Result;
}

fltx4 PreCalcBiasParameter( const fltx4 &bias_parameter )
{
	// convert perlin-style-bias parameter to the value right for the approximation
	return SubSIMD( ReciprocalSIMD( bias_parameter ), Four_Twos );
}

fltx4 BiasSIMD( const fltx4 &val, const fltx4 &precalc_param )
{
	// similar to bias function except pass precalced bias value from calling PreCalcBiasParameter.

	//!!speed!! use reciprocal est?
	//!!speed!! could save one op by precalcing _2_ values
	return DivSIMD( val, AddSIMD( MulSIMD( precalc_param, SubSIMD( Four_Ones, val ) ), Four_Ones ) );
}

int BoxOnPlaneSideSIMD( const fltx4& emins, const fltx4& emaxs, const cplane_t *p, float tolerance)
{
	fltx4 corners[2];
	fltx4 normal = LoadUnalignedSIMD( p->normal.Base() );
	fltx4 dist = ReplicateX4( -p->dist );
	normal = SetWSIMD( normal, dist );
	fltx4 t4 = ReplicateX4( tolerance );
	fltx4 negt4 = ReplicateX4( -tolerance );
	fltx4 cmp = CmpGeSIMD( normal, Four_Zeros );
	corners[0] = MaskedAssign( cmp, emaxs, emins );
	corners[1] = MaskedAssign( cmp, emins, emaxs );
	fltx4 dot1 = Dot4SIMD( normal, corners[0] );
	fltx4 dot2 = Dot4SIMD( normal, corners[1] );
	cmp = CmpGeSIMD( dot1, t4 );
	fltx4 cmp2 = CmpGtSIMD( negt4, dot2 );
	fltx4 result = MaskedAssign( cmp, Four_Ones, Four_Zeros );
	fltx4 result2 = MaskedAssign( cmp2, Four_Twos, Four_Zeros );
	result = AddSIMD( result, result2 );
	intx4 sides;
	ConvertStoreAsIntsSIMD( &sides, result );
	return sides[0];
}

void KDop32_t::Init( void )
{
	for( int i = 0; i < (int)Q_ARRAYSIZE( m_Mins ); i++ )
	{
		m_Mins[i] = Four_FLT_MAX;
		m_Maxes[i] = Four_Negative_FLT_MAX;
	}
}

void KDop32_t::operator|=( KDop32_t const & other )
{
	m_Mins[0] = MinSIMD( m_Mins[0], other.m_Mins[0] );
	m_Mins[1] = MinSIMD( m_Mins[1], other.m_Mins[1] );
	m_Mins[2] = MinSIMD( m_Mins[2], other.m_Mins[2] );
	m_Mins[3] = MinSIMD( m_Mins[3], other.m_Mins[3] );

	m_Maxes[0] = MaxSIMD( m_Maxes[0], other.m_Maxes[0] );
	m_Maxes[1] = MaxSIMD( m_Maxes[1], other.m_Maxes[1] );
	m_Maxes[2] = MaxSIMD( m_Maxes[2], other.m_Maxes[2] );
	m_Maxes[3] = MaxSIMD( m_Maxes[3], other.m_Maxes[3] );

}

bool KDop32_t::Intersects( KDop32_t const &other ) const
{
	fltx4 c00 = CmpLeSIMD( m_Mins[0], other.m_Maxes[0] );
	fltx4 c01 = CmpLeSIMD( m_Mins[1], other.m_Maxes[1] );
	fltx4 c02 = CmpLeSIMD( m_Mins[2], other.m_Maxes[2] );
	fltx4 c03 = CmpLeSIMD( m_Mins[3], other.m_Maxes[3] );

	fltx4 c10 = CmpGeSIMD( m_Maxes[0], other.m_Mins[0] );
	fltx4 c11 = CmpGeSIMD( m_Maxes[1], other.m_Mins[1] );
	fltx4 c12 = CmpGeSIMD( m_Maxes[2], other.m_Mins[2] );
	fltx4 c13 = CmpGeSIMD( m_Maxes[3], other.m_Mins[3] );
	
	fltx4 a0 = AndSIMD( AndSIMD( c00, c01 ), AndSIMD( c02, c03 ) );
	fltx4 a1 = AndSIMD( AndSIMD( c10, c11 ), AndSIMD( c12, c13 ) );
	
	return ! ( IsAnyZeros( AndSIMD( a1, a0 ) ) );
}

bool KDop32_t::IsEmpty( void ) const
{
	fltx4 c00 = CmpLtSIMD( m_Maxes[0], m_Mins[0] );
	fltx4 c01 = CmpLtSIMD( m_Maxes[1], m_Mins[1] );
	fltx4 c02 = CmpLtSIMD( m_Maxes[2], m_Mins[2] );
	fltx4 c03 = CmpLtSIMD( m_Maxes[3], m_Mins[3] );

	return IsAnyNegative( OrSIMD( OrSIMD( c00, c01 ), OrSIMD( c02, c03 ) ) );
}
