//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: - defines SIMD "structure of arrays" classes and functions.
//
//===========================================================================//
#ifndef SSEMATH_H
#define SSEMATH_H
#ifdef _WIN32
#pragma once
#endif

#include <xmmintrin.h>
#ifndef _LINUX
#include <emmintrin.h>
#endif

#include "public/tier0/platform.h"
#include "public/common.h"
#include "public/mathlib/vector.h"
#include "public/mathlib/vectoraligned.h"
#include "public/mathlib/mathlib.h"
#include "public/mathlib/compressed_vector.h"

#ifndef RESTRICT
#define RESTRICT __restrict
#endif

#ifndef Q_ARRAYSIZE
#define Q_ARRAYSIZE( p ) ( sizeof( p ) / sizeof( ( p )[0] ) )
#endif

#if defined(GNUC)
#define USE_STDC_FOR_SIMD 0
#else
#define USE_STDC_FOR_SIMD 0
#endif

#if ( USE_STDC_FOR_SIMD == 0 )
#define _SSE1 1
#endif

// I thought about defining a class/union for the SIMD packed floats instead of using fltx4,
// but decided against it because (a) the nature of SIMD code which includes comparisons is to blur
// the relationship between packed floats and packed integer types and (b) not sure that the
// compiler would handle generating good code for the intrinsics.

#if USE_STDC_FOR_SIMD

#error "hello"
typedef union
{
	float  m128_f32[4];
	uint32_t m128_u32[4];
} fltx4;

typedef fltx4 i32x4;
typedef fltx4 u32x4;

#else

typedef __m128 fltx4;
typedef __m128 i32x4;
typedef __m128 u32x4;

#endif

// The FLTX4 type is a fltx4 used as a parameter to a function, passed by const reference.
typedef const fltx4 & FLTX4;

// A 16-byte aligned int32_t datastructure
// (for use when writing out fltx4's as SIGNED
// ints).
struct ALIGN16 intx4
{
	int32_t m_i32[4];

	int & operator[](int which);

	const int & operator[](int which) const;

	int32_t *Base();

	const int32_t *Base() const;

	const bool operator==(const intx4 &other) const;
} ALIGN16_POST;

// useful constants in SIMD packed float format:
extern const fltx4 Four_Zeros;									// 0 0 0 0
extern const fltx4 Four_Ones;									// 1 1 1 1
extern const fltx4 Four_Twos;									// 2 2 2 2
extern const fltx4 Four_Threes;									// 3 3 3 3
extern const fltx4 Four_Fours;									// guess.
extern const fltx4 Four_Point225s;								// .225 .225 .225 .225
extern const fltx4 Four_PointFives;								// .5 .5 .5 .5
extern const fltx4 Four_Thirds;									// 1/3
extern const fltx4 Four_TwoThirds;								// 2/3
extern const fltx4 Four_Epsilons;								// FLT_EPSILON FLT_EPSILON FLT_EPSILON FLT_EPSILON
extern const fltx4 Four_2ToThe21s;								// (1<<21)..
extern const fltx4 Four_2ToThe22s;								// (1<<22)..
extern const fltx4 Four_2ToThe23s;								// (1<<23)..
extern const fltx4 Four_2ToThe24s;								// (1<<24)..
extern const fltx4 Four_Origin;									// 0 0 0 1 (origin point, like vr0 on the PS2)
extern const fltx4 Four_NegativeOnes;							// -1 -1 -1 -1 
extern const fltx4 Four_FLT_MAX;								// FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
extern const fltx4 Four_Negative_FLT_MAX;						// -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX
extern const fltx4 g_SIMD_0123;									// 0 1 2 3 as float

// external aligned integer constants
#ifndef ALIGN16_POST
#define ALIGN16_POST
#endif
extern const ALIGN16 uint32_t g_SIMD_clear_signmask[] ALIGN16_POST;			// 0x7fffffff x 4
extern const ALIGN16 uint32_t g_SIMD_signmask[] ALIGN16_POST;				// 0x80000000 x 4
extern const ALIGN16 uint32_t g_SIMD_lsbmask[] ALIGN16_POST;				// 0xfffffffe x 4
extern const ALIGN16 uint32_t g_SIMD_clear_wmask[] ALIGN16_POST;			// -1 -1 -1 0
extern const ALIGN16 uint32_t g_SIMD_ComponentMask[4][4] ALIGN16_POST;		// [0xFFFFFFFF 0 0 0], [0 0xFFFFFFFF 0 0], [0 0 0xFFFFFFFF 0], [0 0 0 0xFFFFFFFF]
extern const ALIGN16 uint32_t g_SIMD_AllOnesMask[] ALIGN16_POST;			// ~0,~0,~0,~0
extern const ALIGN16 uint32_t g_SIMD_Low16BitsMask[] ALIGN16_POST;			// 0xffff x 4

// this mask is used for skipping the tail of things. If you have N elements in an array, and wish
// to mask out the tail, g_SIMD_SkipTailMask[N & 3] what you want to use for the last iteration.
extern const uint32_t ALIGN16 g_SIMD_SkipTailMask[4][4] ALIGN16_POST;

extern const uint32_t ALIGN16 g_SIMD_EveryOtherMask[];				// 0, ~0, 0, ~0
// Here's a handy function to align a pointer to the next
// sixteen byte boundary -- it'll round it up to the nearest
// multiple of 16. This is useful if you're subdividing
// big swaths of allocated memory, but in that case, remember
// to leave yourself the necessary slack in the allocation.
template<class T>
inline T *AlignPointer(void * ptr)
{
	uintptr_t temp = (uintptr_t)ptr;
	temp = ALIGN_VALUE(temp, sizeof(T));
	return (T *)temp;
}

//---------------------------------------------------------------------
// Intel/SSE implementation
//---------------------------------------------------------------------

void StoreAlignedSIMD( float * RESTRICT pSIMD, const fltx4 & a );

void StoreUnalignedSIMD( float * RESTRICT pSIMD, const fltx4 & a );

fltx4 RotateLeft( const fltx4 & a );
fltx4 RotateLeft2( const fltx4 & a );

void StoreUnaligned3SIMD( float *pSIMD, const fltx4 & a );

// strongly typed -- syntactic castor oil used for typechecking as we transition to SIMD
void StoreAligned3SIMD( VectorAligned * RESTRICT pSIMD, const fltx4 & a );

// Store the x,y,z components of the four FLTX4 parameters
// into the four consecutive Vectors:
//    pDestination[0],  pDestination[1],  pDestination[2],  pDestination[3]
// The Vectors are assumed to be unaligned.
void StoreFourUnalignedVector3SIMD( fltx4 a, fltx4 b, fltx4	c, FLTX4 d, // first three passed by copy (deliberate)
											   Vector * const pDestination );

// Store the x,y,z components of the four FLTX4 parameters
// into the four consecutive Vectors:
//    pDestination ,  pDestination + 1,  pDestination + 2,  pDestination + 3
// The Vectors are assumed to start on an ALIGNED address, that is, 
// pDestination is 16-byte aligned (thhough obviously pDestination+1 is not).
void StoreFourAlignedVector3SIMD( fltx4 a, fltx4 b, fltx4	c, FLTX4 d, // first three passed by copy (deliberate)
											 Vector * const pDestination );

fltx4 LoadAlignedSIMD( const void *pSIMD );

fltx4 AndSIMD( const fltx4 & a, const fltx4 & b ); // a & b

fltx4 AndNotSIMD( const fltx4 & a, const fltx4 & b ); // a & ~b

fltx4 XorSIMD( const fltx4 & a, const fltx4 & b ); // a ^ b

fltx4 OrSIMD( const fltx4 & a, const fltx4 & b ); // a | b

// Squelch the w component of a vector to +0.0.
// Most efficient when you say a = SetWToZeroSIMD(a) (avoids a copy)
fltx4 SetWToZeroSIMD( const fltx4 & a );

// for the transitional class -- load a 3-by VectorAligned and squash its w component
fltx4 LoadAlignedSIMD( const VectorAligned & pSIMD );

fltx4 LoadUnalignedSIMD( const void *pSIMD );

fltx4 LoadUnaligned3SIMD( const void *pSIMD );

// load a single unaligned float into the x component of a SIMD word
fltx4 LoadUnalignedFloatSIMD( const float *pFlt );

/// replicate a single 32 bit integer value to all 4 components of an m128
fltx4 ReplicateIX4( int i );

fltx4 ReplicateX4( float flValue );

float SubFloat( const fltx4 & a, int idx );

float & SubFloat( fltx4 & a, int idx );

uint32_t SubFloatConvertToInt( const fltx4 & a, int idx );

uint32_t SubInt( const fltx4 & a, int idx );

uint32_t & SubInt( fltx4 & a, int idx );

// Return one in the fastest way.
fltx4 LoadZeroSIMD( void );

// Return one in the fastest way.
fltx4 LoadOneSIMD( void );

fltx4 MaskedAssign( const fltx4 & ReplacementMask, const fltx4 & NewValue, const fltx4 & OldValue );

// remember, the SSE numbers its words 3 2 1 0
// The way we want to specify shuffles is backwards from the default
// MM_SHUFFLE_REV is in array index order (default is reversed)
#define MM_SHUFFLE_REV(a,b,c,d) _MM_SHUFFLE(d,c,b,a)

fltx4 SplatXSIMD( fltx4 const & a );

fltx4 SplatYSIMD( fltx4 const &a );

fltx4 SplatZSIMD( fltx4 const &a );

fltx4 SplatWSIMD( fltx4 const &a );

fltx4 SetXSIMD( const fltx4& a, const fltx4& x );

fltx4 SetYSIMD( const fltx4& a, const fltx4& y );

fltx4 SetZSIMD( const fltx4& a, const fltx4& z );

fltx4 SetWSIMD( const fltx4& a, const fltx4& w );

fltx4 SetComponentSIMD( const fltx4& a, int nComponent, float flValue );

// a b c d -> b c d a
fltx4 RotateLeft( const fltx4 & a );

// a b c d -> c d a b
fltx4 RotateLeft2( const fltx4 & a );

// a b c d -> d a b c
fltx4 RotateRight( const fltx4 & a );

// a b c d -> c d a b
fltx4 RotateRight2( const fltx4 & a );

fltx4 AddSIMD( const fltx4 & a, const fltx4 & b ); // a+b

fltx4 SubSIMD( const fltx4 & a, const fltx4 & b ); // a-b

fltx4 MulSIMD( const fltx4 & a, const fltx4 & b ); // a*b

fltx4 DivSIMD( const fltx4 & a, const fltx4 & b ); // a/b

fltx4 MaddSIMD( const fltx4 & a, const fltx4 & b, const fltx4 & c ); // a*b + c

fltx4 MsubSIMD( const fltx4 & a, const fltx4 & b, const fltx4 & c ); // c - a*b

fltx4 Dot3SIMD( const fltx4 &a, const fltx4 &b );

fltx4 Dot4SIMD( const fltx4 &a, const fltx4 &b );

//TODO: implement as four-way Taylor series
fltx4 SinSIMD( const fltx4 &radians );

void SinCos3SIMD( fltx4 &sine, fltx4 &cosine, const fltx4 &radians );

void SinCosSIMD( fltx4 &sine, fltx4 &cosine, const fltx4 &radians ); // a*b + c

//TODO: implement as four-way Taylor series
fltx4 ArcSinSIMD( const fltx4 &sine );

fltx4 ArcCosSIMD( const fltx4 &cs );

// tan^1(a/b) .. ie, pass sin in as a and cos in as b
fltx4 ArcTan2SIMD( const fltx4 &a, const fltx4 &b );

fltx4 NegSIMD(const fltx4 &a); // negate: -a

int TestSignSIMD( const fltx4 & a ); // mask of which floats have the high bit set

bool IsAnyNegative( const fltx4 & a ); // (a.x < 0) || (a.y < 0) || (a.z < 0) || (a.w < 0)

fltx4 CmpEqSIMD( const fltx4 & a, const fltx4 & b ); // (a==b) ? ~0:0

fltx4 CmpGtSIMD( const fltx4 & a, const fltx4 & b ); // (a>b) ? ~0:0

fltx4 CmpGeSIMD( const fltx4 & a, const fltx4 & b ); // (a>=b) ? ~0:0

fltx4 CmpLtSIMD( const fltx4 & a, const fltx4 & b ); // (a<b) ? ~0:0

fltx4 CmpLeSIMD( const fltx4 & a, const fltx4 & b ); // (a<=b) ? ~0:0

// for branching when a.xyzw > b.xyzw
bool IsAllGreaterThan( const fltx4 &a, const fltx4 &b );

// for branching when a.xyzw >= b.xyzw
bool IsAllGreaterThanOrEq( const fltx4 &a, const fltx4 &b );

// For branching if all a.xyzw == b.xyzw
bool IsAllEqual( const fltx4 & a, const fltx4 & b );

fltx4 CmpInBoundsSIMD( const fltx4 & a, const fltx4 & b ); // (a <= b && a >= -b) ? ~0 : 0

fltx4 MinSIMD( const fltx4 & a, const fltx4 & b ); // min(a,b)

fltx4 MaxSIMD( const fltx4 & a, const fltx4 & b ); // max(a,b)

// SSE lacks rounding operations. 
// Really.
// You can emulate them by setting the rounding mode for the 
// whole processor and then converting to int, and then back again.
// But every time you set the rounding mode, you clear out the
// entire pipeline. So, I can't do them per operation. You
// have to do it once, before the loop that would call these.
// Round towards positive infinity
fltx4 CeilSIMD( const fltx4 &a );

fltx4 fabs( const fltx4 & x );
// Round towards negative infinity
// This is the implementation that was here before; it assumes
// you are in round-to-floor mode, which I guess is usually the
// case for us vis-a-vis SSE.
fltx4 FloorSIMD( const fltx4 &val );

bool IsAnyZeros( const fltx4 & a ); // any floats are zero?

bool IsAllZeros( const fltx4 & var );

fltx4 SqrtEstSIMD( const fltx4 & a ); // sqrt(a), more or less

fltx4 SqrtSIMD( const fltx4 & a ); // sqrt(a)

fltx4 ReciprocalSqrtEstSIMD( const fltx4 & a ); // 1/sqrt(a), more or less

fltx4 ReciprocalSqrtEstSaturateSIMD( const fltx4 & a );

/// uses newton iteration for higher precision results than ReciprocalSqrtEstSIMD
fltx4 ReciprocalSqrtSIMD( const fltx4 & a ); // 1/sqrt(a)

fltx4 ReciprocalEstSIMD( const fltx4 & a ); // 1/a, more or less

/// 1/x for all 4 values, more or less
/// 1/0 will result in a big but NOT infinite result
fltx4 ReciprocalEstSaturateSIMD( const fltx4 & a );

/// 1/x for all 4 values. uses reciprocal approximation instruction plus newton iteration.
/// No error checking!
fltx4 ReciprocalSIMD( const fltx4 & a ); // 1/a

/// 1/x for all 4 values.
/// 1/0 will result in a big but NOT infinite result
fltx4 ReciprocalSaturateSIMD( const fltx4 & a );

// CHRISG: is it worth doing integer bitfiddling for this?
// 2^x for all values (the antilog)
fltx4 ExpSIMD( const fltx4 &toPower );

// Clamps the components of a vector to a specified minimum and maximum range.
fltx4 ClampVectorSIMD( FLTX4 in, FLTX4 min, FLTX4 max);

void TransposeSIMD( fltx4 & x, fltx4 & y, fltx4 & z, fltx4 & w);

fltx4 FindLowestSIMD3( const fltx4 &a );

fltx4 FindHighestSIMD3( const fltx4 &a );

// ------------------------------------
// INTEGER SIMD OPERATIONS.
// ------------------------------------

// Load 4 aligned words into a SIMD register
i32x4 LoadAlignedIntSIMD( const void * RESTRICT pSIMD);

// Load 4 unaligned words into a SIMD register
i32x4 LoadUnalignedIntSIMD( const void * RESTRICT pSIMD);

// save into four words, 16-byte aligned
void StoreAlignedIntSIMD( int32_t * RESTRICT pSIMD, const fltx4 & a );

void StoreAlignedIntSIMD( intx4 &pSIMD, const fltx4 & a );

void StoreUnalignedIntSIMD( int32_t * RESTRICT pSIMD, const fltx4 & a );

// a={ a.x, a.z, b.x, b.z }
// combine two fltx4s by throwing away every other field.
fltx4 CompressSIMD( fltx4 const & a, fltx4 const &b );

// Load four consecutive uint16_t's, and turn them into floating point numbers.
// This function isn't especially fast and could be made faster if anyone is
// using it heavily.
fltx4 LoadAndConvertUint16SIMD( const uint16_t *pInts );

// a={ a.x, b.x, c.x, d.x }
// combine 4 fltx4s by throwing away 3/4s of the fields
fltx4 Compress4SIMD( fltx4 const a, fltx4 const &b, fltx4 const &c, fltx4 const &d );

// outa={a.x, a.x, a.y, a.y}, outb = a.z, a.z, a.w, a.w }
void ExpandSIMD( fltx4 const &a, fltx4 &fl4OutA, fltx4 &fl4OutB );

// CHRISG: the conversion functions all seem to operate on m64's only...
// how do we make them work here?

// Take a fltx4 containing fixed-point uints and 
// return them as single precision floats. No
// fixed point conversion is done.
fltx4 UnsignedIntConvertToFltSIMD( const u32x4 &vSrcA );

// Take a fltx4 containing fixed-point sints and 
// return them as single precision floats. No 
// fixed point conversion is done.
fltx4 SignedIntConvertToFltSIMD( const i32x4 &vSrcA );

/*
  works on fltx4's as if they are four uints.
  the first parameter contains the words to be shifted,
  the second contains the amount to shift by AS INTS

  for i = 0 to 3
  shift = vSrcB_i*32:(i*32)+4
  vReturned_i*32:(i*32)+31 = vSrcA_i*32:(i*32)+31 << shift
*/
i32x4 IntShiftLeftWordSIMD(const i32x4 &vSrcA, const i32x4 &vSrcB);

// Fixed-point conversion and save as SIGNED INTS.
// pDest->x = Int (vSrc.x)
// note: some architectures have means of doing 
// fixed point conversion when the fix depth is
// specified as an immediate.. but there is no way 
// to guarantee an immediate as a parameter to function
// like this.
void ConvertStoreAsIntsSIMD(intx4 * RESTRICT pDest, const fltx4 &vSrc);

// a={a.y, a.z, a.w, b.x } b={b.y, b.z, b.w, b.x }
void RotateLeftDoubleSIMD( fltx4 &a, fltx4 &b );

// // Some convenience operator overloads, which are just aliasing the functions above.
#if !defined( POSIX )
#if 1  // TODO: verify generation of non-bad code. 
// Componentwise add
fltx4 operator+( FLTX4 a, FLTX4 b );

// Componentwise subtract
fltx4 operator-( FLTX4 a, FLTX4 b );

// Componentwise multiply
fltx4 operator*( FLTX4 a, FLTX4 b );

// No divide. You need to think carefully about whether you want a reciprocal
// or a reciprocal estimate.

// bitwise and
fltx4 operator&( FLTX4 a, FLTX4 b );

// bitwise or
fltx4 operator|( FLTX4 a, FLTX4 b );

// bitwise xor
fltx4 operator^( FLTX4 a, FLTX4 b );

// unary negate
fltx4 operator-( FLTX4 a );
#endif // 0
#endif

struct ALIGN16 fourplanes_t
{
	fltx4		nX;
	fltx4		nY;
	fltx4		nZ;
	fltx4		dist;
	fltx4		xSign;
	fltx4		ySign;
	fltx4		zSign;
	fltx4		nXAbs;
	fltx4		nYAbs;
	fltx4		nZAbs;

	void ComputeSignbits();

	// fast SIMD loads
	void Set4Planes( const VPlane *pPlanes );
	void Set2Planes( const VPlane *pPlanes );
	void Get4Planes( VPlane *pPlanesOut );
	void Get2Planes( VPlane *pPlanesOut );
	// not-SIMD, much slower
	void GetPlane( int index, Vector *pNormal, float *pDist ) const;
	void SetPlane( int index, const Vector &vecNormal, float planeDist );
};

class ALIGN16 Frustum_t
{
public:
	Frustum_t();
	void SetPlane( int i, const Vector &vecNormal, float dist );
	void GetPlane( int i, Vector *pNormalOut, float *pDistOut ) const;
	void SetPlanes( const VPlane *pPlanes );
	void GetPlanes( VPlane *pPlanesOut );
	// returns false if the box is within the frustum, true if it is outside
	bool CullBox( const Vector &mins, const Vector &maxs ) const;
	bool CullBoxCenterExtents( const Vector &center, const Vector &extents ) const;

	bool CullBox( const fltx4 &fl4Mins, const fltx4 &fl4Maxs ) const;
	bool CullBoxCenterExtents( const fltx4 &fl4Center, const fltx4 &fl4Extents ) const;

	fourplanes_t	planes[2];
};

/// class FourVectors stores 4 independent vectors for use in SIMD processing. These vectors are
/// stored in the format x x x x y y y y z z z z so that they can be efficiently SIMD-accelerated.
class ALIGN16 FourVectors
{
public:
	fltx4 x, y, z;

	FourVectors(void);

	FourVectors( FourVectors const &src );

	FourVectors( float a );

	/// construct a FourVectors from 4 separate Vectors
	FourVectors(Vector const &a, Vector const &b, Vector const &c, Vector const &d);

	/// construct a FourVectors from 4 separate Vectors
	FourVectors(VectorAligned const &a, VectorAligned const &b, VectorAligned const &c, VectorAligned const &d);

	// construct from twelve floats; really only useful for static const constructors.
	// input arrays must be aligned, and in the fourvectors' native format
	// (eg in xxxx,yyyy,zzzz form) 
	// each pointer should be to an aligned array of four floats
	FourVectors( const float *xs , const float *ys, const float *zs );

	void DuplicateVector(Vector const &v); //< set all 4 vectors to the same vector value

	fltx4 const & operator[](int idx) const;

	fltx4 & operator[](int idx);

	void operator+=(FourVectors const &b); //< add 4 vectors to another 4 vectors

	FourVectors operator+(FourVectors const &b); //< add 4 vectors to another 4 vectors

	void operator-=(FourVectors const &b); //< subtract 4 vectors from another 4

	FourVectors operator-(FourVectors const &b); //< add 4 vectors to another 4 vectors

	void operator*=(FourVectors const &b); //< scale all four vectors per component scale

	void operator*=(const fltx4 & scale); //< scale 

	void operator*=(float scale); //< uniformly scale all 4 vectors

	fltx4 operator*(FourVectors const &b) const; //< 4 dot products

	fltx4 operator*(Vector const &b) const; //< dot product all 4 vectors with 1 vector

	FourVectors operator*(float b) const; //< scale

	void VProduct(FourVectors const &b); //< component by component mul
	void MakeReciprocal(void); //< (x,y,z)=(1/x,1/y,1/z)

	void MakeReciprocalSaturate(void); //< (x,y,z)=(1/x,1/y,1/z), 1/0=1.0e23

	// Assume the given matrix is a rotation, and rotate these vectors by it.
	// If you have a long list of FourVectors structures that you all want 
	// to rotate by the same matrix, use FourVectors::RotateManyBy() instead.
	void RotateBy(const matrix3x4_t& matrix);

	/// You can use this to rotate a long array of FourVectors all by the same
	/// matrix. The first parameter is the head of the array. The second is the
	/// number of vectors to rotate. The third is the matrix.
	static void RotateManyBy(FourVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix );

	static void RotateManyBy(FourVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix, FourVectors * RESTRICT pOut );

	/// Assume the vectors are points, and transform them in place by the matrix.
	void TransformBy(const matrix3x4_t& matrix);

	/// You can use this to Transform a long array of FourVectors all by the same
	/// matrix. The first parameter is the head of the array. The second is the
	/// number of vectors to rotate. The third is the matrix. The fourth is the 
	/// output buffer, which must not overlap the pVectors buffer. This is not 
	/// an in-place transformation.
	static void TransformManyBy(FourVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix, FourVectors * RESTRICT pOut );

	/// You can use this to Transform a long array of FourVectors all by the same
	/// matrix. The first parameter is the head of the array. The second is the
	/// number of vectors to rotate. The third is the matrix. The fourth is the 
	/// output buffer, which must not overlap the pVectors buffer. 
	/// This is an in-place transformation.
	static void TransformManyBy(FourVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix );

	static void CalcClosestPointOnLineSIMD( const FourVectors &P, const FourVectors &vLineA, const FourVectors &vLineB, FourVectors &vClosest, fltx4 *outT = 0);
	static fltx4 CalcClosestPointToLineTSIMD( const FourVectors &P, const FourVectors &vLineA, const FourVectors &vLineB, FourVectors &vDir );

	// X(),Y(),Z() - get at the desired component of the i'th (0..3) vector.
	const float & X(int idx) const;

	const float & Y(int idx) const;

	const float & Z(int idx) const;

	float & X(int idx);

	float & Y(int idx);

	float & Z(int idx);

	Vector Vec(int idx) const; //< unpack one of the vectors
	
	void operator=( FourVectors const &src );

	/// LoadAndSwizzle - load 4 Vectors into a FourVectors, performing transpose op
	void LoadAndSwizzle(Vector const &a, Vector const &b, Vector const &c, Vector const &d);

	void LoadAndSwizzle(Vector const &a);

	// Broadcasts a, b, c, and d into the four vectors
	// This is only performant if the floats are ALREADY IN MEMORY
	// and not on registers -- eg, 
	// .Load( &fltArrray[0], &fltArrray[1], &fltArrray[2], &fltArrray[3] ) is okay,
	// .Load( fltArrray[0] * 0.5f,  fltArrray[1] * 0.5f,  fltArrray[2] * 0.5f,  fltArrray[3] * 0.5f ) is not.
	void Load( const float &a, const float &b, const float &c, const float &d );

	// transform four horizontal vectors into the internal vertical ones
	void LoadAndSwizzle( FLTX4 a, FLTX4 b, FLTX4 c, FLTX4 d  );

	/// LoadAndSwizzleAligned - load 4 Vectors into a FourVectors, performing transpose op.
	/// all 4 vectors must be 128 bit boundary
	void LoadAndSwizzleAligned(const float *RESTRICT a, const float *RESTRICT b, const float *RESTRICT c, const float *RESTRICT d);

	void LoadAndSwizzleAligned(Vector const &a, Vector const &b, Vector const &c, Vector const &d);

	/// Unpack a FourVectors back into four horizontal fltx4s.
	/// Since the FourVectors doesn't store a w row, you can optionally
	/// specify your own; otherwise it will be 0.
	/// This function ABSOLUTELY MUST be inlined or the reference parameters will
	/// induce a severe load-hit-store.
	void TransposeOnto( fltx4 &out0,  fltx4 &out1,  fltx4 &out2,  fltx4 &out3, FLTX4 w = Four_Zeros ) const;

	/// Store a FourVectors into four NON-CONTIGUOUS Vector*'s. 
	void StoreUnalignedVector3SIMD( Vector * RESTRICT out0, Vector * RESTRICT out1, Vector * RESTRICT out2, Vector * RESTRICT out3 ) const;

	/// Store a FourVectors into four NON-CONTIGUOUS VectorAligned s. 
	void StoreAlignedVectorSIMD( VectorAligned * RESTRICT out0, VectorAligned * RESTRICT out1, VectorAligned * RESTRICT out2, VectorAligned * RESTRICT out3 ) const;

	/// Store a FourVectors into four CONSECUTIVE Vectors in memory,
	/// where the first vector IS NOT aligned on a 16-byte boundary. 
	void StoreUnalignedContigVector3SIMD( Vector * RESTRICT pDestination );

	/// Store a FourVectors into four CONSECUTIVE Vectors in memory,
	/// where the first vector IS aligned on a 16-byte boundary. 
	/// (since four Vector3s = 48 bytes, groups of four can be said
	///  to be 16-byte aligned though obviously the 2nd, 3d, and 4th
	///  vectors in the group individually are not)
	void StoreAlignedContigVector3SIMD( Vector * RESTRICT pDestination );

	/// Store a FourVectors into four CONSECUTIVE VectorAligneds in memory
	void StoreAlignedContigVectorASIMD( VectorAligned * RESTRICT pDestination );

	/// return the squared length of all 4 vectors
	fltx4 length2(void) const;

	/// return the approximate length of all 4 vectors. uses the sqrt approximation instruction
	fltx4 length(void) const;

	/// normalize all 4 vectors in place. not mega-accurate (uses reciprocal approximation instruction)
	void VectorNormalizeFast(void);

	/// normalize all 4 vectors in place.
	void VectorNormalize(void);

	fltx4 DistToSqr( FourVectors const &pnt );
	
	fltx4 TValueOfClosestPointOnLine( FourVectors const &p0, FourVectors const &p1 ) const;

	fltx4 DistSqrToLineSegment( FourVectors const &p0, FourVectors const &p1 ) const;
};

//
FourVectors Mul( const FourVectors &a, const fltx4 &b );

FourVectors Mul( const FourVectors &a, const FourVectors &b );

FourVectors Madd( const FourVectors &a, const fltx4 &b, const FourVectors &c ); // a*b + c

/// form 4 cross products
FourVectors operator ^(const FourVectors &a, const FourVectors &b);

FourVectors operator-(const FourVectors &a, const FourVectors &b);

/// component-by-componentwise MAX operator
FourVectors maximum(const FourVectors &a, const FourVectors &b);

/// component-by-componentwise MIN operator
FourVectors minimum(const FourVectors &a, const FourVectors &b);

FourVectors RotateLeft( const FourVectors &src );

FourVectors RotateRight( const FourVectors &src );
FourVectors MaskedAssign( const fltx4 & ReplacementMask, const FourVectors & NewValue, const FourVectors & OldValue );

/// calculate reflection vector. incident and normal dir assumed normalized
FourVectors VectorReflect( const FourVectors &incident, const FourVectors &normal );

/// calculate slide vector. removes all components of a vector which are perpendicular to a normal vector.
FourVectors VectorSlide( const FourVectors &incident, const FourVectors &normal );

/// normalize all 4 vectors in place. not mega-accurate (uses reciprocal approximation instruction)
FourVectors VectorNormalizeFast( const FourVectors &src );

/// quick, low quality perlin-style noise() function suitable for real time use.
/// return value is -1..1. Only reliable around +/- 1 million or so.
fltx4 NoiseSIMD( const fltx4 & x, const fltx4 & y, const fltx4 & z );
fltx4 NoiseSIMD( FourVectors const &v );

// vector valued noise direction
FourVectors DNoiseSIMD( FourVectors const &v );

// vector value "curl" noise function. see http://hyperphysics.phy-astr.gsu.edu/hbase/curl.html
FourVectors CurlNoiseSIMD( FourVectors const &v );

/// calculate the absolute value of a packed single
fltx4 fabs( const fltx4 & x );

/// negate all four components of a SIMD packed single
fltx4 fnegate( const fltx4 & x );

fltx4 Pow_FixedPoint_Exponent_SIMD( const fltx4 & x, int exponent);

// PowSIMD - raise a SIMD register to a power.  This is analogous to the C pow() function, with some
// restictions: fractional exponents are only handled with 2 bits of precision. Basically,
// fractions of 0,.25,.5, and .75 are handled. PowSIMD(x,.30) will be the same as PowSIMD(x,.25).
// negative and fractional powers are handled by the SIMD reciprocal and square root approximation
// instructions and so are not especially accurate ----Note that this routine does not raise
// numeric exceptions because it uses SIMD--- This routine is O(log2(exponent)).
fltx4 PowSIMD( const fltx4 & x, float exponent );

// random number generation - generate 4 random numbers quickly.

void SeedRandSIMD(uint32_t seed);								// seed the random # generator
fltx4 RandSIMD( int nContext = 0 );							// return 4 numbers in the 0..1 range

// for multithreaded, you need to use these and use the argument form of RandSIMD:
int GetSIMDRandContext( void );
void ReleaseSIMDRandContext( int nContext );

fltx4 RandSignedSIMD( void ); // -1..1

fltx4 LerpSIMD ( const fltx4 &percent, const fltx4 &a, const fltx4 &b);

fltx4 RemapValClampedSIMD(const fltx4 &val, const fltx4 &a, const fltx4 &b, const fltx4 &c, const fltx4 &d); // Remap val from clamped range between a and b to new range between c and d

// SIMD versions of mathlib simplespline functions
// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
fltx4 SimpleSpline( const fltx4 & value );

// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
fltx4 SimpleSplineRemapValWithDeltas( const fltx4 & val,
											 const fltx4 & A, const fltx4 & BMinusA,
											 const fltx4 & OneOverBMinusA, const fltx4 & C, 
											 const fltx4 & DMinusC );

fltx4 SimpleSplineRemapValWithDeltasClamped( const fltx4 & val,
													const fltx4 & A, const fltx4 & BMinusA,
													const fltx4 & OneOverBMinusA, const fltx4 & C, 
													const fltx4 & DMinusC );

fltx4 FracSIMD( const fltx4 &val );

fltx4 Mod2SIMD( const fltx4 &val );

fltx4 Mod2SIMDPositiveInput( const fltx4 &val );

// approximate sin of an angle, with -1..1 representing the whole sin wave period instead of -pi..pi.
// no range reduction is done - for values outside of 0..1 you won't like the results
fltx4 _SinEst01SIMD( const fltx4 &val );

fltx4 _Sin01SIMD( const fltx4 &val );

// full range useable implementations
fltx4 SinEst01SIMD( const fltx4 &val );

fltx4 Sin01SIMD( const fltx4 &val );

fltx4 NatExpSIMD( const fltx4 &val ); // why is ExpSimd( x ) defined to be 2^x?

// Schlick style Bias approximation see graphics gems 4 : bias(t,a)= t/( (1/a-2)*(1-t)+1)
 
fltx4 PreCalcBiasParameter( const fltx4 &bias_parameter );

fltx4 BiasSIMD( const fltx4 &val, const fltx4 &precalc_param );

//-----------------------------------------------------------------------------
// Box/plane test 
// NOTE: The w component of emins + emaxs must be 1 for this to work
//-----------------------------------------------------------------------------
int BoxOnPlaneSideSIMD( const fltx4& emins, const fltx4& emaxs, const cplane_t *p, float tolerance = 0.f );

// k-dop bounding volume. 26-dop bounds with 13 plane-pairs plus 3 other "arbitrary bounds". The arbitrary values could be used to hold type info, etc,
// which can compare against "for free"
class KDop32_t
{
public:
	fltx4 m_Mins[4];
	fltx4 m_Maxes[4];

	bool Intersects( KDop32_t const &other ) const;

	void operator|=( KDop32_t const & other );

	bool IsEmpty( void ) const;

	void Init( void );

	// given a set of points, expand the kdop to contain them
	void AddPointSet( Vector const *pPoints, int nPnts );

	void CreateFromPointSet( Vector const *pPoints, int nPnts );
};

extern const fltx4 g_KDop32XDirs[4];
extern const fltx4 g_KDop32YDirs[4];
extern const fltx4 g_KDop32ZDirs[4];

#endif // _ssemath_h
