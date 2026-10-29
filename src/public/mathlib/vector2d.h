//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef VECTOR2D_H
#define VECTOR2D_H
#ifdef _WIN32
#pragma once
#endif


#ifndef VEC_T_DEFINED
#define VEC_T_DEFINED
typedef float vec_t;
#endif

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?

// For rand(). We really need a library!
#include "public/tier0/stdlib.h"

#include "public/mathlib/math_pfns.h"

//=========================================================
// 2D Vector2D
//=========================================================

class Vector2D					
{
public:
	// Members
	vec_t x, y;

	// Construction/destruction
	Vector2D(void);
	Vector2D(vec_t X, vec_t Y);
	Vector2D(const float *pFloat);

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f);

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Initialization methods
	void Random( float minVal, float maxVal );

	// equality
	bool operator==(const Vector2D& v) const;
	bool operator!=(const Vector2D& v) const;	

	// arithmetic operations
	Vector2D&	operator+=(const Vector2D &v);			
	Vector2D&	operator-=(const Vector2D &v);		
	Vector2D&	operator*=(const Vector2D &v);			
	Vector2D&	operator*=(float s);
	Vector2D&	operator/=(const Vector2D &v);		
	Vector2D&	operator/=(float s);					

	// negate the Vector2D components
	void	Negate(); 

	// Get the Vector2D's magnitude.
	vec_t	Length() const;

	// Get the Vector2D's magnitude squared.
	vec_t	LengthSqr(void) const;

	// return true if this vector is (0,0) within tolerance
	bool IsZero( float tolerance = 0.01f ) const;

	// Normalize in place and return the old length.
	vec_t	NormalizeInPlace();

	// Compare length.
	bool	IsLengthGreaterThan( float val ) const;
	bool	IsLengthLessThan( float val ) const;

	// Get the distance from this Vector2D to the other one.
	vec_t	DistTo(const Vector2D &vOther) const;

	// Get the distance from this Vector2D to the other one squared.
	vec_t	DistToSqr(const Vector2D &vOther) const;		

	// Copy
	void	CopyToArray(float* rgfl) const;	

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector2D equation (because it's done per-component
	// rather than per-Vector2D).
	void	MulAdd(const Vector2D& a, const Vector2D& b, float scalar);	

	// Dot product.
	vec_t	Dot(const Vector2D& vOther) const;			

	// assignment
	Vector2D& operator=(const Vector2D &vOther) = default;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
	Vector2D(const Vector2D &vOther) = default;

	// arithmetic operations
	Vector2D	operator-(void) const;
				
	Vector2D	operator+(const Vector2D& v) const;	
	Vector2D	operator-(const Vector2D& v) const;	
	Vector2D	operator*(const Vector2D& v) const;	
	Vector2D	operator/(const Vector2D& v) const;	
	Vector2D	operator*(float fl) const;
	Vector2D	operator/(float fl) const;			
	
	// Cross product between two vectors.
	Vector2D	Cross(const Vector2D &vOther) const;		

	// Returns a Vector2D with the min or max in X, Y, and Z.
	Vector2D	Min(const Vector2D &vOther) const;
	Vector2D	Max(const Vector2D &vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector2D(const Vector2D& vOther);
#endif
};

//-----------------------------------------------------------------------------

const Vector2D vec2_origin(0,0);
const Vector2D vec2_invalid( FLT_MAX, FLT_MAX );

//-----------------------------------------------------------------------------
// Vector2D related operations
//-----------------------------------------------------------------------------

// Vector2D clear
void Vector2DClear( Vector2D& a );

// Copy
void Vector2DCopy( const Vector2D& src, Vector2D& dst );

// Vector2D arithmetic
void Vector2DAdd( const Vector2D& a, const Vector2D& b, Vector2D& result );
void Vector2DSubtract( const Vector2D& a, const Vector2D& b, Vector2D& result );
void Vector2DMultiply( const Vector2D& a, vec_t b, Vector2D& result );
void Vector2DMultiply( const Vector2D& a, const Vector2D& b, Vector2D& result );
void Vector2DDivide( const Vector2D& a, vec_t b, Vector2D& result );
void Vector2DDivide( const Vector2D& a, const Vector2D& b, Vector2D& result );
void Vector2DMA( const Vector2D& start, float s, const Vector2D& dir, Vector2D& result );

// Store the min or max of each of x, y, and z into the result.
void Vector2DMin( const Vector2D &a, const Vector2D &b, Vector2D &result );
void Vector2DMax( const Vector2D &a, const Vector2D &b, Vector2D &result );

#define Vector2DExpand( v ) (v).x, (v).y

// Normalization
vec_t Vector2DNormalize( Vector2D& v );

// Length
vec_t Vector2DLength( const Vector2D& v );

// Dot Product
vec_t DotProduct2D(const Vector2D& a, const Vector2D& b);

// Linearly interpolate between two vectors
void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest );


//-----------------------------------------------------------------------------
//
// Inlined Vector2D methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Array access
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








void Vector2DAdd( const Vector2D& a, const Vector2D& b, Vector2D& c );

void Vector2DSubtract( const Vector2D& a, const Vector2D& b, Vector2D& c );

void Vector2DMultiply( const Vector2D& a, vec_t b, Vector2D& c );

void Vector2DMultiply( const Vector2D& a, const Vector2D& b, Vector2D& c );


void Vector2DDivide( const Vector2D& a, vec_t b, Vector2D& c );

void Vector2DDivide( const Vector2D& a, const Vector2D& b, Vector2D& c );


// FIXME: Remove
// For backwards compatability


//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------

// for backwards compatability


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------











//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Get the distance from this Vector2D to the other one 
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint2D( const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D *pResult );



//-----------------------------------------------------------------------------
//
// Slow methods
//
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------








Vector2D operator*(float fl, const Vector2D& v);

#endif //slow

#endif // VECTOR2D_H
