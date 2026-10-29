//====== Copyright 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef VECTOR_H
#define VECTOR_H
#ifdef _WIN32
#pragma once
#endif


#ifndef VEC_T_DEFINED
#define VEC_T_DEFINED
typedef float vec_t;
#endif

#ifndef RESTRICT
#define RESTRICT __restrict
#endif

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?

// For rand(). We really need a library!
#include "public/tier0/stdlib.h"

// For MMX intrinsics
#include <xmmintrin.h>

#ifndef ALIGN16_POST
#define ALIGN16_POST
#endif

#include "public/tier0/platform.h"
#include "public/tier0/threadtools.h"
#include "public/mathlib/vector2d.h"
#include "public/mathlib/math_pfns.h"
#include "public/tier0/memalloc.h"

// Uncomment this to add extra Asserts to check for NANs, uninitialized vecs, etc.
//#define VECTOR_PARANOIA	1

// Uncomment this to make sure we don't do anything slow with our vectors
//#define VECTOR_NO_SLOW_OPERATIONS 1


// Used to make certain code easier to read.
#define X_INDEX	0
#define Y_INDEX	1
#define Z_INDEX	2


#ifdef VECTOR_PARANOIA
#define CHECK_VALID( _v)	Assert( (_v).IsValid() )
#else
#ifdef GNUC
#define CHECK_VALID( _v)
#else
#define CHECK_VALID( _v)	0
#endif
#endif

#define VecToString(v)	(static_cast<const char *>(CFmtStr("(%f, %f, %f)", (v).x, (v).y, (v).z))) // ** Note: this generates a temporary, don't hold reference!

class VectorByValue;

//=========================================================
// 3D Vector
//=========================================================
class Vector					
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction:
	Vector(void); 
	Vector(vec_t X, vec_t Y, vec_t Z);

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f);
	 // TODO (Ilya): Should there be an init that takes a single float for consistency?

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Cast to Vector2D...
	Vector2D& AsVector2D();
	const Vector2D& AsVector2D() const;

	// Initialization methods
	void Random( vec_t minVal, vec_t maxVal );
	void Zero(); ///< zero out a vector

	// equality
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;	

	// arithmetic operations
	Vector&	operator+=(const Vector &v);			
	Vector&	operator-=(const Vector &v);		
	Vector&	operator*=(const Vector &v);			
	Vector&	operator*=(float s);
	Vector&	operator/=(const Vector &v);		
	Vector&	operator/=(float s);	
	Vector&	operator+=(float fl) ; ///< broadcast add
	Vector&	operator-=(float fl) ; ///< broadcast sub			

// negate the vector components
	void	Negate(); 

	// Get the vector's magnitude.
	vec_t	Length() const;

	// Get the vector's magnitude squared.
	vec_t LengthSqr(void) const;

	// Get one over the vector's length
	// via fast hardware approximation
	vec_t LengthRecipFast(void) const;

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero( float tolerance = 0.01f ) const;


	// return true if this vector is exactly (0,0,0) -- only fast if vector is coming from memory, not registers
	bool IsZeroFast( ) const RESTRICT;

	vec_t	NormalizeInPlace();
	Vector	Normalized() const;
	bool	IsLengthGreaterThan( float val ) const;
	bool	IsLengthLessThan( float val ) const;

	// check if a vector is within the box defined by two other vectors
	bool WithinAABox( Vector const &boxmin, Vector const &boxmax);
 
	// Get the distance from this vector to the other one.
	vec_t	DistTo(const Vector &vOther) const;

	// Get the distance from this vector to the other one squared.
	// NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' inline.  
	// may be able to tidy this up after switching to VC7
	vec_t DistToSqr(const Vector &vOther) const;

	// Copy
	void	CopyToArray(float* rgfl) const;	

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual vector equation (because it's done per-component
	// rather than per-vector).
	void	MulAdd(const Vector& a, const Vector& b, float scalar);	

	// Dot product.
	vec_t	Dot(const Vector& vOther) const;			

	// assignment
	Vector& operator=(const Vector &vOther) = default;

	// returns 0, 1, 2 corresponding to the component with the largest absolute value
	int LargestComponent() const;

	// 2d
	vec_t	Length2D(void) const;					
	vec_t	Length2DSqr(void) const;		

	/// get the component of this vector parallel to some other given vector
	Vector  ProjectOnto( const Vector& onto );

	operator VectorByValue &();
	operator const VectorByValue &() const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
//	Vector(const Vector &vOther);

	// arithmetic operations
	Vector	operator-(void) const;
				
	Vector	operator+(const Vector& v) const;	
	Vector	operator-(const Vector& v) const;	
	Vector	operator*(const Vector& v) const;	
	Vector	operator/(const Vector& v) const;	
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;			
	
	// Cross product between two vectors.
	Vector	Cross(const Vector &vOther) const;		

	// Returns a vector with the min or max in X, Y, and Z.
	Vector	Min(const Vector &vOther) const;
	Vector	Max(const Vector &vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector(const Vector& vOther);
#endif
};


//=========================================================
// 4D Short Vector (aligned on 8-byte boundary)
//=========================================================
class ALIGN8 ShortVector
{
public:

	short x, y, z, w;

	// Initialization
	void Init(short ix = 0, short iy = 0, short iz = 0, short iw = 0 );

	// Setter
	void Set( const ShortVector& vOther );
	void Set( const short ix, const short iy, const short iz, const short iw );

	// array access...
	short operator[](int i) const;
	short& operator[](int i);

	// Base address...
	short* Base();
	short const* Base() const;

	// equality
	bool operator==(const ShortVector& v) const;
	bool operator!=(const ShortVector& v) const;	

	// Arithmetic operations
	ShortVector& operator+=(const ShortVector &v);			
	ShortVector& operator-=(const ShortVector &v);		
	ShortVector& operator*=(const ShortVector &v);			
	ShortVector& operator*=(float s);
	ShortVector& operator/=(const ShortVector &v);		
	ShortVector& operator/=(float s);					
	ShortVector operator*(float fl) const;

private:

	// No copy constructors allowed if we're in optimal mode
//	ShortVector(ShortVector const& vOther);

	// No assignment operators either...
//	ShortVector& operator=( ShortVector const& src );

} ALIGN8_POST;






//=========================================================
// 4D Integer Vector
//=========================================================
class IntVector4D
{
public:

	int x, y, z, w;

	// Initialization
	void Init(int ix = 0, int iy = 0, int iz = 0, int iw = 0 );

	// Setter
	void Set( const IntVector4D& vOther );
	void Set( const int ix, const int iy, const int iz, const int iw );

	// array access...
	int operator[](int i) const;
	int& operator[](int i);

	// Base address...
	int* Base();
	int const* Base() const;

	// equality
	bool operator==(const IntVector4D& v) const;
	bool operator!=(const IntVector4D& v) const;	

	// Arithmetic operations
	IntVector4D& operator+=(const IntVector4D &v);			
	IntVector4D& operator-=(const IntVector4D &v);		
	IntVector4D& operator*=(const IntVector4D &v);			
	IntVector4D& operator*=(float s);
	IntVector4D& operator/=(const IntVector4D &v);		
	IntVector4D& operator/=(float s);					
	IntVector4D operator*(float fl) const;

private:

	// No copy constructors allowed if we're in optimal mode
	//	IntVector4D(IntVector4D const& vOther);

	// No assignment operators either...
	//	IntVector4D& operator=( IntVector4D const& src );

};



//-----------------------------------------------------------------------------
// Allows us to specifically pass the vector by value when we need to
//-----------------------------------------------------------------------------
class VectorByValue : public Vector
{
public:
	// Construction/destruction:
	VectorByValue(void);
	VectorByValue(vec_t X, vec_t Y, vec_t Z);
	VectorByValue(const VectorByValue& vOther);
};


//-----------------------------------------------------------------------------
// Utility to simplify table construction. No constructor means can use
// traditional C-style initialization
//-----------------------------------------------------------------------------
class TableVector
{
public:
	vec_t x, y, z;

	operator Vector &();
	operator const Vector &() const;

	// array access...
	vec_t& operator[](int i);

	vec_t operator[](int i) const;
};


//-----------------------------------------------------------------------------
// Vector related operations
//-----------------------------------------------------------------------------

// Vector clear
void VectorClear( Vector& a );

// Copy
void VectorCopy( const Vector& src, Vector& dst );

// Vector arithmetic
void VectorAdd( const Vector& a, const Vector& b, Vector& result );
void VectorSubtract( const Vector& a, const Vector& b, Vector& result );
void VectorMultiply( const Vector& a, vec_t b, Vector& result );
void VectorMultiply( const Vector& a, const Vector& b, Vector& result );
void VectorDivide( const Vector& a, vec_t b, Vector& result );
void VectorDivide( const Vector& a, const Vector& b, Vector& result );
void VectorScale ( const Vector& in, vec_t scale, Vector& result );
void VectorMA( const Vector& start, float scale, const Vector& direction, Vector& dest );

// Vector equality with tolerance
bool VectorsAreEqual( const Vector& src1, const Vector& src2, float tolerance = 0.0f );

#define VectorExpand(v) (v).x, (v).y, (v).z


// Normalization
// FIXME: Can't use quite yet
//vec_t VectorNormalize( Vector& v );

// Length
vec_t VectorLength( const Vector& v );

// Dot Product
vec_t DotProduct(const Vector& a, const Vector& b);

// Cross product
void CrossProduct(const Vector& a, const Vector& b, Vector& result );

// Store the min or max of each of x, y, and z into the result.
void VectorMin( const Vector &a, const Vector &b, Vector &result );
void VectorMax( const Vector &a, const Vector &b, Vector &result );

// Linearly interpolate between two vectors
void VectorLerp(const Vector& src1, const Vector& src2, vec_t t, Vector& dest );
Vector VectorLerp(const Vector& src1, const Vector& src2, vec_t t );

Vector ReplicateToVector( float x );

// check if a point is in the field of a view of an object.
bool PointWithinViewAngle( Vector const &vecSrcPosition, Vector const &vecTargetPosition, Vector const &vecLookDirection, float flCosHalfFOV );


#ifndef VECTOR_NO_SLOW_OPERATIONS

// Cross product
Vector CrossProduct( const Vector& a, const Vector& b );

// Random vector creation
Vector RandomVector( vec_t minVal, vec_t maxVal );

#endif

float RandomVectorInUnitSphere( Vector *pVector );
float RandomVectorInUnitCircle( Vector2D *pVector );


//-----------------------------------------------------------------------------
//
// Inlined Vector methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------


//inline Vector::Vector(const float *pFloat)					
//{
//	Assert( pFloat );
//	x = pFloat[0]; y = pFloat[1]; z = pFloat[2];	
//	CHECK_VALID(*this);
//} 

#if 0
//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

#endif

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
// Cast to Vector2D...
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Invalidate
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
// #pragma message("TODO: these should be SSE")






// this ought to be an opcode.







// get the component of this vector parallel to some other given vector


//-----------------------------------------------------------------------------
//
// Inlined Short Vector methods
//
//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------







void ShortVectorMultiply( const ShortVector& src, float fl, ShortVector& res );







//-----------------------------------------------------------------------------
//
// Inlined Integer Vector methods
//
//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------







void IntVector4DMultiply( const IntVector4D& src, float fl, IntVector4D& res );




// =======================


void VectorAdd( const Vector& a, const Vector& b, Vector& c );

void VectorSubtract( const Vector& a, const Vector& b, Vector& c );

void VectorMultiply( const Vector& a, vec_t b, Vector& c );

void VectorMultiply( const Vector& a, const Vector& b, Vector& c );

// for backwards compatability


void VectorDivide( const Vector& a, vec_t b, Vector& c );

void VectorDivide( const Vector& a, const Vector& b, Vector& c );

// FIXME: Remove
// For backwards compatability



//-----------------------------------------------------------------------------
// Temporary storage for vector results so const Vector& results can be returned
//-----------------------------------------------------------------------------
Vector &AllocTempVector();



//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------

// for backwards compatability



vec_t DotProductAbs( const Vector &v0, const Vector &v1 );

vec_t DotProductAbs( const Vector &v0, const float *v1 );

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------

/*
// FIXME: Can't use until we're un-macroed in mathlib.h
inline vec_t VectorNormalize( Vector& v )
{
	Assert( v.IsValid() );
	vec_t l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		// FIXME: 
		// Just copying the existing implemenation; shouldn't res.z == 0?
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}
*/


// check a point against a box

//-----------------------------------------------------------------------------
// Get the distance from this vector to the other one 
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Vector equality with tolerance
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint( const Vector& vecStart, float flMaxDist, const Vector& vecTarget, Vector *pResult );


//-----------------------------------------------------------------------------
// Takes the absolute value of a vector
//-----------------------------------------------------------------------------
void VectorAbs( const Vector& src, Vector& dst );


//-----------------------------------------------------------------------------
//
// Slow methods
//
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS

//-----------------------------------------------------------------------------
// Returns a vector with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------








Vector operator*(float fl, const Vector& v);

//-----------------------------------------------------------------------------
// cross product
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// 2D
//-----------------------------------------------------------------------------






// and when you want to return the vector rather than cause a LHS with it...
Vector VectorMin( const Vector &a, const Vector &b );

Vector VectorMax( const Vector &a, const Vector &b );

float ComputeVolume( const Vector &vecMins, const Vector &vecMaxs );

// Get a random vector.
Vector RandomVector( float minVal, float maxVal );

#endif //slow

//-----------------------------------------------------------------------------
// Helper debugging stuff....
//-----------------------------------------------------------------------------

bool operator==( float const* f, const Vector& v );

bool operator==( const Vector& v, float const* f );

bool operator!=( float const* f, const Vector& v );

bool operator!=( const Vector& v, float const* f );


// return a vector perpendicular to another, with smooth variation. The difference between this and
// something like VectorVectors is that there are now discontinuities. _unlike_ VectorVectors,
// you won't get an "u
void VectorPerpendicularToVector( Vector const &in, Vector *pvecOut );

//-----------------------------------------------------------------------------
// AngularImpulse
//-----------------------------------------------------------------------------
// AngularImpulse are exponetial maps (an axis scaled by a "twist" angle in degrees)
typedef Vector AngularImpulse;

#ifndef VECTOR_NO_SLOW_OPERATIONS

AngularImpulse RandomAngularImpulse( float minVal, float maxVal );

#endif


//-----------------------------------------------------------------------------
// Quaternion
//-----------------------------------------------------------------------------

class RadianEuler;

class ALIGN16 Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	Quaternion(void);
	Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw);
	Quaternion(RadianEuler const &angle);	// evil auto type promotion!!!

	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f);

	bool IsValid() const;
	void Invalidate();

	bool operator==( const Quaternion &src ) const;
	bool operator!=( const Quaternion &src ) const;

	vec_t* Base();
	const vec_t* Base() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z, w;
} ALIGN16_POST;


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Equality test
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Quaternion equality with tolerance
//-----------------------------------------------------------------------------
bool QuaternionsAreEqual( const Quaternion& src1, const Quaternion& src2, float tolerance );


//-----------------------------------------------------------------------------
// Radian Euler angle aligned to axis (NOT ROLL/PITCH/YAW)
//-----------------------------------------------------------------------------
class QAngle;
class RadianEuler
{
public:
	RadianEuler(void);
	RadianEuler(vec_t X, vec_t Y, vec_t Z);
	RadianEuler(Quaternion const &q);	// evil auto type promotion!!!
	RadianEuler(QAngle const &angles);	// evil auto type promotion!!!

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f);

	//	conversion to qangle
	QAngle ToQAngle( void ) const;
	bool IsValid() const;
	void Invalidate();

	vec_t *Base();
	const vec_t *Base() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z;
};


extern void AngleQuaternion( RadianEuler const &angles, Quaternion &qt );
extern void QuaternionAngles( Quaternion const &q, RadianEuler &angles );




void VectorCopy( RadianEuler const& src, RadianEuler &dst );

void VectorScale( RadianEuler const& src, float b, RadianEuler &dst );




//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Degree Euler QAngle pitch, yaw, roll
//-----------------------------------------------------------------------------
class QAngleByValue;

class QAngle					
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction
	QAngle(void);
	QAngle(vec_t X, vec_t Y, vec_t Z);
//	QAngle(RadianEuler const &angles);	// evil auto type promotion!!!

	// Allow pass-by-value
	operator QAngleByValue &();
	operator const QAngleByValue &() const;

	// Initialization
	void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f);
	void Random( vec_t minVal, vec_t maxVal );

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;
	
	// equality
	bool operator==(const QAngle& v) const;
	bool operator!=(const QAngle& v) const;	

	// arithmetic operations
	QAngle&	operator+=(const QAngle &v);
	QAngle&	operator-=(const QAngle &v);
	QAngle&	operator*=(float s);
	QAngle&	operator/=(float s);

	// Get the vector's magnitude.
	vec_t	Length() const;
	vec_t	LengthSqr() const;

	// negate the QAngle components
	//void	Negate(); 

	// No assignment operators either...
	QAngle& operator=( const QAngle& src ) = default;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors

	// arithmetic operations
	QAngle	operator-(void) const;
	
	QAngle	operator+(const QAngle& v) const;
	QAngle	operator-(const QAngle& v) const;
	QAngle	operator*(float fl) const;
	QAngle	operator/(float fl) const;
#else

private:
	// No copy constructors allowed if we're in optimal mode
	QAngle(const QAngle& vOther);

#endif
};

//-----------------------------------------------------------------------------
// Allows us to specifically pass the vector by value when we need to
//-----------------------------------------------------------------------------
class QAngleByValue : public QAngle
{
public:
	// Construction/destruction:
	QAngleByValue(void);
	QAngleByValue(vec_t X, vec_t Y, vec_t Z);
	QAngleByValue(const QAngleByValue& vOther);
};


void VectorAdd( const QAngle& a, const QAngle& b, QAngle& result );

void VectorMA( const QAngle &start, float scale, const QAngle &direction, QAngle &dest );


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------


#ifndef VECTOR_NO_SLOW_OPERATIONS

QAngle RandomAngle( float minVal, float maxVal );

#endif








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
// Invalidate
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
void VectorCopy( const QAngle& src, QAngle& dst );


//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------


	

//-----------------------------------------------------------------------------
// Vector equality with tolerance
//-----------------------------------------------------------------------------
bool QAnglesAreEqual( const QAngle& src1, const QAngle& src2, float tolerance = 0.0f );


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS






QAngle operator*(float fl, const QAngle& v);

#endif // VECTOR_NO_SLOW_OPERATIONS


//-----------------------------------------------------------------------------
// NOTE: These are not completely correct.  The representations are not equivalent
// unless the QAngle represents a rotational impulse along a coordinate axis (x,y,z)
void QAngleToAngularImpulse( const QAngle &angles, AngularImpulse &impulse );

void AngularImpulseToQAngle( const AngularImpulse &impulse, QAngle &angles );

vec_t InvRSquared( const float* v );

vec_t InvRSquared( const Vector &v );

// FIXME: Change this back to a #define once we get rid of the vec_t version
float VectorNormalize( Vector& v );

// FIXME: Obsolete version of VectorNormalize, once we remove all the friggin float*s
float VectorNormalize( float * v );

void VectorNormalizeFast (Vector& vec);





#endif
