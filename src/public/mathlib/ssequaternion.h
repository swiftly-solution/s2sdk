//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: - defines SIMD "structure of arrays" classes and functions.
//
//===========================================================================//
#ifndef SSEQUATMATH_H
#define SSEQUATMATH_H

#ifdef _WIN32
#pragma once
#endif

#include "public/mathlib/ssemath.h"
#include "public/mathlib/quaternionaligned.h"

// Use this #define to allow SSE versions of Quaternion math
// to exist on PC.
// On PC, certain horizontal vector operations are not supported.
// This causes the SSE implementation of quaternion math to mix the
// vector and scalar floating point units, which is extremely 
// performance negative if you don't compile to native SSE2 (which 
// we don't as of Sept 1, 2007). So, it's best not to allow these
// functions to exist at all. It's not good enough to simply replace
// the contents of the functions with scalar math, because each call
// to LoadAligned and StoreAligned will result in an unnecssary copy
// of the quaternion, and several moves to and from the XMM registers.
//
// Basically, the problem you run into is that for efficient SIMD code,
// you need to load the quaternions and vectors into SIMD registers and
// keep them there as long as possible while doing only SIMD math,
// whereas for efficient scalar code, each time you copy onto or ever
// use a fltx4, it hoses your pipeline. So the difference has to be
// in the management of temporary variables in the calling function,
// not inside the math functions.
//
// If you compile assuming the presence of SSE2, the MSVC will abandon
// the traditional x87 FPU operations altogether and make everything use
// the SSE2 registers, which lessens this problem a little.

//---------------------------------------------------------------------
// Load/store quaternions
//---------------------------------------------------------------------
// Using STDC or SSE
fltx4 LoadAlignedSIMD( const QuaternionAligned & pSIMD );

fltx4 LoadAlignedSIMD( const QuaternionAligned * RESTRICT pSIMD );

void StoreAlignedSIMD( QuaternionAligned * RESTRICT pSIMD, const fltx4 & a );

#if ALLOW_SIMD_QUATERNION_MATH
//---------------------------------------------------------------------
// Make sure quaternions are within 180 degrees of one another, if not, reverse q
//---------------------------------------------------------------------
fltx4 QuaternionAlignSIMD( const fltx4 &p, const fltx4 &q );

//---------------------------------------------------------------------
// Normalize Quaternion
//---------------------------------------------------------------------

// SSE implementation
fltx4 QuaternionNormalizeSIMD( const fltx4 &q );

//---------------------------------------------------------------------
// 0.0 returns p, 1.0 return q.
//---------------------------------------------------------------------
fltx4 QuaternionBlendNoAlignSIMD( const fltx4 &p, const fltx4 &q, float t );

//---------------------------------------------------------------------
// Blend Quaternions
//---------------------------------------------------------------------
fltx4 QuaternionBlendSIMD( const fltx4 &p, const fltx4 &q, float t );

//---------------------------------------------------------------------
// Multiply Quaternions
//---------------------------------------------------------------------

// SSE and STDC
fltx4 QuaternionMultSIMD( const fltx4 &p, const fltx4 &q );

//---------------------------------------------------------------------
// Quaternion scale
//---------------------------------------------------------------------

// SSE and STDC
fltx4 QuaternionScaleSIMD( const fltx4 &p, float t );

//-----------------------------------------------------------------------------
// Quaternion sphereical linear interpolation
//-----------------------------------------------------------------------------

// SSE and STDC
fltx4 QuaternionSlerpNoAlignSIMD( const fltx4 &p, const fltx4 &q, float t );

fltx4 QuaternionSlerpSIMD( const fltx4 &p, const fltx4 &q, float t );

#endif // ALLOW_SIMD_QUATERNION_MATH

/// class FourVectors stores 4 independent vectors for use in SIMD processing. These vectors are
/// stored in the format x x x x y y y y z z z z so that they can be efficiently SIMD-accelerated.
class ALIGN16 FourQuaternions
{
public:
	fltx4 x,y,z,w;

	FourQuaternions(void);

	FourQuaternions( const fltx4 &_x, 
					 const fltx4 &_y, 
					 const fltx4 &_z, 
					 const fltx4 &_w );

	FourQuaternions( FourQuaternions const &src );

	void operator=( FourQuaternions const &src );

	/// this = this * q;
	FourQuaternions Mul( FourQuaternions const &q ) const;

	/// negate the vector part
	FourQuaternions Conjugate() const;

	/// for a quaternion representing a rotation of angle theta, return
	/// one of angle s*theta
	/// scale is four floats -- one for each quat
	FourQuaternions ScaleAngle( const fltx4 &scale ) const;

	/// ret = this * ( s * q ) 
	/// In other words, for a quaternion representing a rotation of angle theta, return
	/// one of angle s*theta
	/// s is four floats in a fltx4 -- one for each quaternion
	FourQuaternions MulAc( const fltx4 &s, const FourQuaternions &q ) const;

	/// ret = ( s * this ) * q
	FourQuaternions ScaleMul( const fltx4 &s, const FourQuaternions &q ) const;

	/// Slerp four quaternions at once, FROM me TO the specified out.
	FourQuaternions Slerp( const FourQuaternions &to, const fltx4 &t );

	FourQuaternions SlerpNoAlign( const FourQuaternions &originalto, const fltx4 &t );

	/// LoadAndSwizzleAligned - load 4 QuaternionAligneds into a FourQuaternions, performing transpose op.
	/// all 4 vectors must be 128 bit boundary
	void LoadAndSwizzleAligned(const float *RESTRICT a, const float *RESTRICT b, const float *RESTRICT c, const float *RESTRICT d);

	void LoadAndSwizzleAligned(const QuaternionAligned * RESTRICT a, 
		const QuaternionAligned * RESTRICT b, 
		const QuaternionAligned * RESTRICT c, 
		const QuaternionAligned * RESTRICT d);

	/// LoadAndSwizzleAligned - load 4 consecutive QuaternionAligneds into a FourQuaternions, 
	/// performing transpose op.
	/// all 4 vectors must be 128 bit boundary
	void LoadAndSwizzleAligned(const QuaternionAligned *qs);

	// Store the FourQuaternions out to four nonconsecutive ordinary quaternions in memory.
	void SwizzleAndStoreAligned(QuaternionAligned *a, QuaternionAligned *b, QuaternionAligned *c, QuaternionAligned *d);

	// Store the FourQuaternions out to four consecutive ordinary quaternions in memory.
	void SwizzleAndStoreAligned(QuaternionAligned *qs);

	// Store the FourQuaternions out to four consecutive ordinary quaternions in memory.
	// The mask specifies which of the quaternions are actually written out -- each	
	// word in the fltx4 should be all binary ones or zeros. Ones means the corresponding
	// quat will be written.
	void SwizzleAndStoreAlignedMasked(QuaternionAligned * RESTRICT qs, const fltx4 &controlMask);
};

const fltx4 Dot(const FourQuaternions &a, const FourQuaternions &b);

const FourQuaternions Madd(const FourQuaternions &a, const fltx4 &scale, const FourQuaternions &c);

const FourQuaternions Mul(const FourQuaternions &a, const fltx4 &scale);

const FourQuaternions Add(const FourQuaternions &a,const FourQuaternions &b);

const FourQuaternions Sub(const FourQuaternions &a,const FourQuaternions &b);

const FourQuaternions Neg(const FourQuaternions &q);

const FourQuaternions MaskedAssign(const fltx4 &mask, const FourQuaternions &a, const FourQuaternions &b);

FourQuaternions QuaternionAlign( const FourQuaternions &p, const FourQuaternions &q );

const FourQuaternions QuaternionNormalize( const FourQuaternions &q );

/// this = this * q;

/*

void QuaternionScale( const Quaternion &p, float t, Quaternion &q );

*/

//-----------------------------------------------------------------------------
// Purpose: return = this * ( s * q )
// In other words, for a quaternion representing a rotation of angle theta, return
// one of angle s*theta
// s is four floats in a fltx4 -- one for each quaternion
//-----------------------------------------------------------------------------
 

#endif // SSEQUATMATH_H
