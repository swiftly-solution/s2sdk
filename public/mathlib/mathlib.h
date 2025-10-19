//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef MATH_LIB_H
#define MATH_LIB_H

#include <math.h>
#include "tier0/basetypes.h"
#include "mathlib/vector.h"
#include "mathlib/vector2d.h"
#include "tier0/dbg.h"


void AngleVectors(const QAngle & angles, Vector * forward, Vector * right, Vector * up);

template <class T>
void V_swap(T& x, T& y)
{
	T temp = x;
	x = y;
	y = temp;
}


enum MatrixAxisType_t
{
	FORWARD_AXIS = 0,
	LEFT_AXIS = 1,
	UP_AXIS = 2,

	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2,

	ORIGIN = 3,
	PROJECTIVE = 3,
};

class matrix3x4a_t;

struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}


	float *operator[]( int i )				{ Assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	const float *operator[]( int i ) const	{ Assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	float *Base()							{ return &m_flMatVal[0][0]; }
	const float *Base() const				{ return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

class ALIGN16 matrix3x4a_t : public matrix3x4_t
{
public:
	/*
	matrix3x4a_t() { if (((size_t)Base()) % 16 != 0) { Error( "matrix3x4a_t missaligned" ); } }
	*/
	matrix3x4a_t& operator=( const matrix3x4_t& src ) { memcpy( Base(), src.Base(), sizeof( float ) * 3 * 4 ); return *this; };
} ALIGN16_POST;

#ifndef M_PI
	#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
	#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
	#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

// Used to represent sides of things like planes.
#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define SIDE_CROSS  -2      // necessary for polylib.c

#define ON_VIS_EPSILON  0.01    // necessary for vvis (flow.c) -- again look into moving later!
#define	EQUAL_EPSILON	0.001   // necessary for vbsp (faces.c) -- should look into moving it there?

extern bool s_bMathlibInitialized;

extern  const Vector vec3_origin;
extern  const QAngle vec3_angle;
extern	const Quaternion quat_identity;
extern const Vector vec3_invalid;
extern	const int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)




// Math routines done in optimized assembly math package routines
void inline SinCos( float radians, float *sine, float *cosine )
{
#if defined( _X360 )
	XMScalarSinCos( sine, cosine, radians );
#elif defined( PLATFORM_WINDOWS_PC32 )
	_asm
	{
		fld		DWORD PTR [radians]
		fsincos

		mov edx, DWORD PTR [cosine]
		mov eax, DWORD PTR [sine]

		fstp DWORD PTR [edx]
		fstp DWORD PTR [eax]
	}
#elif defined( PLATFORM_WINDOWS_PC64 )
	*sine = sin( radians );
	*cosine = cos( radians );
#elif defined( POSIX )
	double __cosr, __sinr;
	__asm ("fsincos" : "=t" (__cosr), "=u" (__sinr) : "0" (radians));

  	*sine = __sinr;
  	*cosine = __cosr;
#endif
}

#define SIN_TABLE_SIZE	256
#define FTOIBIAS		12582912.f
extern float SinCosTable[SIN_TABLE_SIZE];

inline float TableCos( float theta )
{
	union
	{
		int i;
		float f;
	} ftmp;

	// ideally, the following should compile down to: theta * constant + constant, changing any of these constants from defines sometimes fubars this.
	ftmp.f = theta * ( float )( SIN_TABLE_SIZE / ( 2.0f * M_PI ) ) + ( FTOIBIAS + ( SIN_TABLE_SIZE / 4 ) );
	return SinCosTable[ ftmp.i & ( SIN_TABLE_SIZE - 1 ) ];
}

inline float TableSin( float theta )
{
	union
	{
		int i;
		float f;
	} ftmp;

	// ideally, the following should compile down to: theta * constant + constant
	ftmp.f = theta * ( float )( SIN_TABLE_SIZE / ( 2.0f * M_PI ) ) + FTOIBIAS;
	return SinCosTable[ ftmp.i & ( SIN_TABLE_SIZE - 1 ) ];
}

template<class T>
FORCEINLINE T Square( T const &a )
{
	return a * a;
}


// return the smallest power of two >= x.
// returns 0 if x == 0 or x > 0x80000000 (ie numbers that would be negative if x was signed)
// NOTE: the old code took an int, and if you pass in an int of 0x80000000 casted to a uint,
//       you'll get 0x80000000, which is correct for uints, instead of 0, which was correct for ints
FORCEINLINE uint SmallestPowerOfTwoGreaterOrEqual( uint x )
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

// return the largest power of two <= x. Will return 0 if passed 0
FORCEINLINE uint LargestPowerOfTwoLessThanOrEqual( uint x )
{
	if ( x >= 0x80000000 )
		return 0x80000000;

	return SmallestPowerOfTwoGreaterOrEqual( x + 1 ) >> 1;
}




#endif	// MATH_BASE_H

