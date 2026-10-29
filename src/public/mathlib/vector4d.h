//========= Copyright 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef VECTOR4D_H
#define VECTOR4D_H
#ifdef _WIN32
#pragma once
#endif


#include <math.h>
#include "public/tier0/stdlib.h"		// for rand(). we really need a library!
#include <float.h>
#include <xmmintrin.h>	// for sse
#include "public/mathlib/math_pfns.h"
#include "public/mathlib/vector.h"

// forward declarations
class Vector;
class Vector2D;

//=========================================================
// 4D Vector4D
//=========================================================

class Vector4D					
{
public:
	// Members
	vec_t x, y, z, w;

	// Construction/destruction
	Vector4D(void);
	Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W);
	Vector4D(const float *pFloat);

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f);
	void Init( const Vector& src, vec_t iw=0.0f );

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Cast to Vector and Vector2D...
	Vector& AsVector3D();
	Vector const& AsVector3D() const;

	Vector2D& AsVector2D();
	Vector2D const& AsVector2D() const;

	// Initialization methods
	void Random( vec_t minVal, vec_t maxVal );

	// equality
	bool operator==(const Vector4D& v) const;
	bool operator!=(const Vector4D& v) const;	

	// arithmetic operations
	Vector4D&	operator+=(const Vector4D &v);			
	Vector4D&	operator-=(const Vector4D &v);		
	Vector4D&	operator*=(const Vector4D &v);			
	Vector4D&	operator*=(float s);
	Vector4D&	operator/=(const Vector4D &v);		
	Vector4D&	operator/=(float s);					

	Vector4D	operator-( void ) const;
	Vector4D	operator*( float fl ) const;
	Vector4D	operator/( float fl ) const;
	Vector4D	operator*( const Vector4D& v ) const;
	Vector4D	operator+( const Vector4D& v ) const;
	Vector4D	operator-( const Vector4D& v ) const;

	// negate the Vector4D components
	void	Negate(); 

	// Get the Vector4D's magnitude.
	vec_t	Length() const;

	// Get the Vector4D's magnitude squared.
	vec_t	LengthSqr(void) const;

	// return true if this vector is (0,0,0,0) within tolerance
	bool IsZero( float tolerance = 0.01f ) const;

	// Get the distance from this Vector4D to the other one.
	vec_t	DistTo(const Vector4D &vOther) const;

	// Get the distance from this Vector4D to the other one squared.
	vec_t	DistToSqr(const Vector4D &vOther) const;		

	// Copy
	void	CopyToArray(float* rgfl) const;	

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector4D equation (because it's done per-component
	// rather than per-Vector4D).
	void	MulAdd(Vector4D const& a, Vector4D const& b, float scalar);	

	// Dot product.
	vec_t	Dot(Vector4D const& vOther) const;			

	// No copy constructors allowed if we're in optimal mode
#ifdef VECTOR_NO_SLOW_OPERATIONS
private:
#else
public:
#endif
	Vector4D(Vector4D const& vOther);

	// No assignment operators either...
	Vector4D& operator=( Vector4D const& src );
};

const Vector4D vec4_origin( 0.0f, 0.0f, 0.0f, 0.0f );
const Vector4D vec4_invalid( FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX );

//-----------------------------------------------------------------------------
// SSE optimized routines
//-----------------------------------------------------------------------------

class ALIGN16 Vector4DAligned : public Vector4D
{
public:
	Vector4DAligned(void);
	Vector4DAligned( vec_t X, vec_t Y, vec_t Z, vec_t W );

	void Set( vec_t X, vec_t Y, vec_t Z, vec_t W );
	void InitZero( void );

	__m128 &AsM128();
	const __m128 &AsM128() const;

private:
	// No copy constructors allowed if we're in optimal mode
	Vector4DAligned( Vector4DAligned const& vOther );

	// No assignment operators either...
	Vector4DAligned& operator=( Vector4DAligned const& src );
} ALIGN16_POST;

//-----------------------------------------------------------------------------
// Vector4D related operations
//-----------------------------------------------------------------------------

// Vector4D clear
void Vector4DClear( Vector4D& a );

// Copy
void Vector4DCopy( Vector4D const& src, Vector4D& dst );

// Vector4D arithmetic
void Vector4DAdd( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DSubtract( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DMultiply( Vector4D const& a, vec_t b, Vector4D& result );
void Vector4DMultiply( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DDivide( Vector4D const& a, vec_t b, Vector4D& result );
void Vector4DDivide( Vector4D const& a, Vector4D const& b, Vector4D& result );
void Vector4DMA( Vector4D const& start, float s, Vector4D const& dir, Vector4D& result );

// Vector4DAligned arithmetic
void Vector4DMultiplyAligned( Vector4DAligned const& a, vec_t b, Vector4DAligned& result );


#define Vector4DExpand( v ) (v).x, (v).y, (v).z, (v).w

// Normalization
vec_t Vector4DNormalize( Vector4D& v );

// Length
vec_t Vector4DLength( Vector4D const& v );

// Dot Product
vec_t DotProduct4D(Vector4D const& a, Vector4D const& b);

// Linearly interpolate between two vectors
void Vector4DLerp(Vector4D const& src1, Vector4D const& src2, vec_t t, Vector4D& dest );


//-----------------------------------------------------------------------------
//
// Inlined Vector4D methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Cast to Vector and Vector2D...
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------













Vector4D operator*( float fl, const Vector4D& v );



void Vector4DAdd( Vector4D const& a, Vector4D const& b, Vector4D& c );

void Vector4DSubtract( Vector4D const& a, Vector4D const& b, Vector4D& c );

void Vector4DMultiply( Vector4D const& a, vec_t b, Vector4D& c );

void Vector4DMultiply( Vector4D const& a, Vector4D const& b, Vector4D& c );

void Vector4DDivide( Vector4D const& a, vec_t b, Vector4D& c );

void Vector4DDivide( Vector4D const& a, Vector4D const& b, Vector4D& c );


// FIXME: Remove
// For backwards compatability

void Vector4DLerp(const Vector4D& src1, const Vector4D& src2, vec_t t, Vector4D& dest );

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------

vec_t DotProduct4D(const Vector4D& a, const Vector4D& b);

// for backwards compatability


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

// FIXME: Can't use until we're un-macroed in mathlib.h

//-----------------------------------------------------------------------------
// Get the distance from this Vector4D to the other one 
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Vector4DAligned routines
//-----------------------------------------------------------------------------




void Vector4DMultiplyAligned( Vector4DAligned const& a, Vector4DAligned const& b, Vector4DAligned& c );

void Vector4DWeightMAD( vec_t w, Vector4DAligned const& vInA, Vector4DAligned& vOutA, Vector4DAligned const& vInB, Vector4DAligned& vOutB );

void Vector4DWeightMADSSE( vec_t w, Vector4DAligned const& vInA, Vector4DAligned& vOutA, Vector4DAligned const& vInB, Vector4DAligned& vOutB );

#endif // VECTOR4D_H
