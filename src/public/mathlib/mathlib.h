//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef MATH_LIB_H
#define MATH_LIB_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>
#include "public/tier0/platform.h"
#include "public/common.h"
#include "public/mathlib/vector.h"
#include "public/mathlib/vectorws.h"
#include "public/mathlib/vector2d.h"

#include "public/mathlib/math_pfns.h"

// Fallbacks for basetypes.h / dbg.h, which this repo does not have.
#ifndef FASTCALL
	#ifdef _MSC_VER
		#define FASTCALL __fastcall
	#else
		#define FASTCALL
	#endif
#endif
#ifndef VALVE_RAND_MAX
	#define VALVE_RAND_MAX 0x7fff
#endif
#ifndef VEC_T_NAN
	#define VEC_T_NAN FLOAT32_NAN
#endif
#ifndef MAX
	#define MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )
#endif
#ifndef MIN
	#define MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )
#endif
#ifndef FALSE
	#define FALSE 0
#endif
#ifndef ARRAYSIZE
	#define ARRAYSIZE( p ) ( sizeof( p ) / sizeof( ( p )[0] ) )
#endif

template<class T, class Y, class X>
inline T clamp( T const &val, Y const &minVal, X const &maxVal )
{
	if ( val < minVal )
		return minVal;
	else if ( val > maxVal )
		return maxVal;
	else
		return val;
}

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
// FIXME: does the asm code even exist anymore?
// FIXME: this should move to a different file
struct cplane_t
{
	Vector	normal;
	float	dist;
	byte	type;			// for fast side tests
	byte	signbits;		// signx + (signy<<1) + (signz<<1)
	byte	pad[2];

#ifdef VECTOR_NO_SLOW_OPERATIONS
	cplane_t();

private:
	// No copy constructors allowed if we're in optimal mode
	cplane_t(const cplane_t& vOther);
#endif
};

// structure offset for asm code
#define CPLANE_NORMAL_X			0
#define CPLANE_NORMAL_Y			4
#define CPLANE_NORMAL_Z			8
#define CPLANE_DIST				12
#define CPLANE_TYPE				16
#define CPLANE_SIGNBITS			17
#define CPLANE_PAD0				18
#define CPLANE_PAD1				19

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5


//-----------------------------------------------------------------------------
// Frustum plane indices.
// WARNING: there is code that depends on these values
//-----------------------------------------------------------------------------

enum
{
	FRUSTUM_RIGHT		= 0,
	FRUSTUM_LEFT		= 1,
	FRUSTUM_TOP			= 2,
	FRUSTUM_BOTTOM		= 3,
	FRUSTUM_NEARZ		= 4,
	FRUSTUM_FARZ		= 5,
	FRUSTUM_NUMPLANES	= 6
};

extern int SignbitsForPlane( cplane_t *out );
class Frustum_t;

// Computes Y fov from an X fov and a screen aspect ratio + X from Y
float CalcFovY( float flFovX, float flScreenAspect );
float CalcFovX( float flFovY, float flScreenAspect );

// Generate a frustum based on perspective view parameters
// NOTE: FOV is specified in degrees, as the *full* view angle (not half-angle)
class VPlane;
void GeneratePerspectiveFrustum( const Vector& origin, const QAngle &angles, float flZNear, float flZFar, float flFovX, float flAspectRatio, Frustum_t &frustum );
void GeneratePerspectiveFrustum( const Vector& origin, const Vector &forward, const Vector &right, const Vector &up, float flZNear, float flZFar, float flFovX, float flFovY, VPlane *pPlanesOut );
// Cull the world-space bounding box to the specified frustum.
// bool R_CullBox( const Vector& mins, const Vector& maxs, const Frustum_t &frustum );
// bool R_CullBoxSkipNear( const Vector& mins, const Vector& maxs, const Frustum_t &frustum );
void GenerateOrthoFrustum( const Vector &origin, const Vector &forward, const Vector &right, const Vector &up, float flLeft, float flRight, float flBottom, float flTop, float flZNear, float flZFar, VPlane *pPlanesOut );

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
	matrix3x4_t();
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 );

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const Vector &xAxis, const Vector &yAxis, const Vector &zAxis, const Vector &vecOrigin );
	void Init( const QAngle &angs, const Vector &pos );
	void Init( const QAngle &angs );
	void Init( const RadianEuler &angs, const Vector &pos );
	void Init( const RadianEuler &angs );
	void Init( const Quaternion &quat, const Vector &pos );
	void Init( const Quaternion &quat );
	void Init( const Vector &fwd );
	void Init( const Vector &fwd, const Vector &pos );
	void Init( const Vector &axis_of_rotation, float degrees );
	void Init( const Vector &axis_of_rotation, float degrees, const Vector &pos );

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const Vector &xAxis, const Vector &yAxis, const Vector &zAxis, const Vector &vecOrigin );

	void SetToIdentity();

	void ConcatRotations( const matrix3x4_t &other );
	void ConcatTransforms( const matrix3x4_t &other );

	void ScaleBy( const float value );
	void ScaleByZero();

	void Multiply( const matrix3x4_t &other );
	void Transpose();

	QAngle ToQAngle() const;
	RadianEuler ToRadianEuler() const;
	Quaternion ToQuaternion() const;

	float RowDotProduct( int row, const Vector &in ) const;
	float ColumnDotProduct( MatrixAxisType_t column, const Vector &in ) const;

	Vector GetTranslation() const;
	void SetTranslation( const Vector &in );

	Vector GetColumn( MatrixAxisType_t column ) const;
	void SetColumn( const Vector &in, MatrixAxisType_t column );

	void SetOrigin( const Vector &pos );
	Vector GetOrigin() const;

	Vector GetForward() const;
	Vector GetLeft() const;
	Vector GetRight() const;
	Vector GetUp() const;

	void GetVectors( Vector *fwd, Vector *right, Vector *up ) const;

	matrix3x4_t GetInverse() const;
	matrix3x4_t GetInverseTranspose() const;

	Vector Transform( const Vector &in ) const;
	Vector TransformByInverse( const Vector &in ) const;

	void TransformAABB( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;
	void TransformAABBByInverse( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;
	
	QAngle TransformToLocalSpace( const QAngle &angles ) const;
	QAngle TransformToWorldSpace( const QAngle &angles ) const;

	Vector Rotate( const Vector &in ) const;
	Vector RotateByInverse( const Vector &in ) const;

	void RotateAABB( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;
	void RotateAABBByInverse( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;

	bool IsEqualEnough( const matrix3x4_t &other, float flTolerance = 1e-5 ) const;

	void Invalidate( void );

	matrix3x4_t &operator *=( const matrix3x4_t &other );
	matrix3x4_t &operator *=( float value );

	matrix3x4_t operator *( const matrix3x4_t &other ) const;
	matrix3x4_t operator *( float value ) const;

	float *operator[]( int i );
	const float *operator[]( int i ) const;
	float *Base();
	const float *Base() const;

	float m_flMatVal[3][4];
};

class ALIGN16 matrix3x4a_t : public matrix3x4_t
{
public:
	/*
	matrix3x4a_t() { if (((size_t)Base()) % 16 != 0) { Error( "matrix3x4a_t missaligned" ); } }
	*/
	matrix3x4a_t& operator=( const matrix3x4_t& src );
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

extern const Vector vec3_origin;
extern const VectorWS vec3ws_origin;
extern const QAngle vec3_angle;
extern const Quaternion quat_identity;
extern const QuaternionWS quatws_identity;
extern const Vector vec3_invalid;
extern const int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

vec_t DotProduct(const vec_t *v1, const vec_t *v2);
void VectorSubtract(const vec_t *a, const vec_t *b, vec_t *c);
void VectorAdd(const vec_t *a, const vec_t *b, vec_t *c);
void VectorCopy(const vec_t *a, vec_t *b);
void VectorClear(vec_t *a);

float VectorMaximum(const vec_t *v);

float VectorMaximum(const Vector& v);

void VectorScale (const float* in, vec_t scale, float* out);


// Cannot be forceinline as they have overloads:
void VectorFill(vec_t *a, float b);

void VectorNegate(vec_t *a);


//#define VectorMaximum(a)		( max( (a)[0], max( (a)[1], (a)[2] ) ) )
#define Vector2Clear(x)			{(x)[0]=(x)[1]=0;}
#define Vector2Negate(x)		{(x)[0]=-((x)[0]);(x)[1]=-((x)[1]);}
#define Vector2Copy(a,b)		{(b)[0]=(a)[0];(b)[1]=(a)[1];}
#define Vector2Subtract(a,b,c)	{(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];}
#define Vector2Add(a,b,c)		{(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];}
#define Vector2Scale(a,b,c)		{(c)[0]=(b)*(a)[0];(c)[1]=(b)*(a)[1];}

// NJS: Some functions in VBSP still need to use these for dealing with mixing vec4's and shorts with vec_t's.
// remove when no longer needed.
#define VECTOR_COPY( A, B ) do { (B)[0] = (A)[0]; (B)[1] = (A)[1]; (B)[2]=(A)[2]; } while(0)
#define DOT_PRODUCT( A, B ) ( (A)[0]*(B)[0] + (A)[1]*(B)[1] + (A)[2]*(B)[2] )

void VectorMAInline( const float* start, float scale, const float* direction, float* dest );

void VectorMAInline( const Vector& start, float scale, const Vector& direction, Vector& dest );

void VectorMA( const Vector& start, float scale, const Vector& direction, Vector& dest );

void VectorMA( const float * start, float scale, const float *direction, float *dest );


int VectorCompare (const float *v1, const float *v2);

float VectorLength(const float *v);

void CrossProduct (const float *v1, const float *v2, float *cross);

qboolean VectorsEqual( const float *v1, const float *v2 );

vec_t RoundInt (vec_t in);

size_t Q_log2( unsigned int val );

// Math routines done in optimized assembly math package routines
void SinCos( float radians, float *sine, float *cosine );

#define SIN_TABLE_SIZE	256
#define FTOIBIAS		12582912.f
extern float SinCosTable[SIN_TABLE_SIZE];

float TableCos( float theta );

float TableSin( float theta );

template<class T>
FORCEINLINE T Square( T const &a )
{
	return a * a;
}


// return the smallest power of two >= x.
// returns 0 if x == 0 or x > 0x80000000 (ie numbers that would be negative if x was signed)
// NOTE: the old code took an int, and if you pass in an int of 0x80000000 casted to a uint,
//       you'll get 0x80000000, which is correct for uints, instead of 0, which was correct for ints
uint32_t SmallestPowerOfTwoGreaterOrEqual( uint32_t x );

// return the largest power of two <= x. Will return 0 if passed 0
uint LargestPowerOfTwoLessThanOrEqual( uint x );


// Math routines for optimizing division
void FloorDivMod (double numer, double denom, int *quotient, int *rem);
int GreatestCommonDivisor (int i1, int i2);

// Test for FPU denormal mode
bool IsDenormal( const float &val );

// MOVEMENT INFO
enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

void MatrixAngles( const matrix3x4_t & matrix, float *angles ); // !!!!
void MatrixVectors( const matrix3x4_t &matrix, Vector* pForward, Vector *pRight, Vector *pUp );
void VectorTransform (const float *in1, const matrix3x4_t & in2, float *out);
void VectorITransform (const float *in1, const matrix3x4_t & in2, float *out);
void VectorRotate( const float *in1, const matrix3x4_t & in2, float *out);
void VectorRotate( const Vector &in1, const QAngle &in2, Vector &out );
void VectorRotate( const Vector &in1, const Quaternion &in2, Vector &out );
void VectorIRotate( const float *in1, const matrix3x4_t & in2, float *out);

#ifndef VECTOR_NO_SLOW_OPERATIONS

QAngle TransformAnglesToLocalSpace( const QAngle &angles, const matrix3x4_t &parentMatrix );
QAngle TransformAnglesToWorldSpace( const QAngle &angles, const matrix3x4_t &parentMatrix );

#endif

void MatrixInitialize( matrix3x4_t &mat, const Vector &vecOrigin, const Vector &vecXAxis, const Vector &vecYAxis, const Vector &vecZAxis );
void MatrixCopy( const matrix3x4_t &in, matrix3x4_t &out );
void MatrixInvert( const matrix3x4_t &in, matrix3x4_t &out );

// Matrix equality test
bool MatricesAreEqual( const matrix3x4_t &src1, const matrix3x4_t &src2, float flTolerance = 1e-5 );

void MatrixGetColumn( const matrix3x4_t &in, int column, Vector &out );
void MatrixSetColumn( const Vector &in, int column, matrix3x4_t &out );

void MatrixGetTranslation( const matrix3x4_t &in, Vector &out );

void MatrixSetTranslation( const Vector &in, matrix3x4_t &out );

void MatrixScaleBy ( const float flScale, matrix3x4_t &out );
void MatrixScaleByZero ( matrix3x4_t &out );

//void DecomposeRotation( const matrix3x4_t &mat, float *out );
void ConcatRotations (const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out);
void ConcatTransforms (const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out);
// faster version assumes m0, m1, out are 16-byte aligned addresses
void ConcatTransforms_Aligned( const matrix3x4a_t &m0, const matrix3x4a_t &m1, matrix3x4a_t &out );

// For identical interface w/ VMatrix
void MatrixMultiply ( const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out );

void QuaternionSlerp( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionSlerpNoAlign( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionBlend( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionBlendNoAlign( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionIdentityBlend( const Quaternion &p, float t, Quaternion &qt );
float QuaternionAngleDiff( const Quaternion &p, const Quaternion &q );
void QuaternionScale( const Quaternion &p, float t, Quaternion &q );
void QuaternionAlign( const Quaternion &p, const Quaternion &q, Quaternion &qt );
float QuaternionDotProduct( const Quaternion &p, const Quaternion &q );
void QuaternionConjugate( const Quaternion &p, Quaternion &q );
void QuaternionInvert( const Quaternion &p, Quaternion &q );
float QuaternionNormalize( Quaternion &q );
void QuaternionAdd( const Quaternion &p, const Quaternion &q, Quaternion &qt );
void QuaternionMult( const Quaternion &p, const Quaternion &q, Quaternion &qt );
void QuaternionMatrix( const Quaternion &q, matrix3x4_t &matrix );
void QuaternionMatrix( const Quaternion &q, const Vector &pos, matrix3x4_t &matrix );
void QuaternionAngles( const Quaternion &q, QAngle &angles );
void AngleQuaternion( const QAngle& angles, Quaternion &qt );
void QuaternionAngles( const Quaternion &q, RadianEuler &angles );
void AngleQuaternion( RadianEuler const &angles, Quaternion &qt );
void QuaternionAxisAngle( const Quaternion &q, Vector &axis, float &angle );
void AxisAngleQuaternion( const Vector &axis, float angle, Quaternion &q );
void BasisToQuaternion( const Vector &vecForward, const Vector &vecRight, const Vector &vecUp, Quaternion &q );
void MatrixQuaternion( const matrix3x4_t &mat, Quaternion &q );

// A couple methods to find the dot product of a vector with a matrix row or column...
float MatrixRowDotProduct( const matrix3x4_t &in1, int row, const Vector& in2 );

float MatrixColumnDotProduct( const matrix3x4_t &in1, int col, const Vector& in2 );

int __cdecl BoxOnPlaneSide (const float *emins, const float *emaxs, const cplane_t *plane);

float anglemod(float a);

// Remap a value in the range [A,B] to [C,D].
float RemapVal( float val, float A, float B, float C, float D);

float RemapValClamped( float val, float A, float B, float C, float D);

// Returns A + (B-A)*flPercent.
// float Lerp( float flPercent, float A, float B );
template <class T>
FORCEINLINE T Lerp( float flPercent, T const &A, T const &B )
{
	return A + (B - A) * flPercent;
}

float Sqr( float f );

// 5-argument floating point linear interpolation.
// FLerp(f1,f2,i1,i2,x)=
//    f1 at x=i1
//    f2 at x=i2
//   smooth lerp between f1 and f2 at x>i1 and x<i2
//   extrapolation for x<i1 or x>i2
//
//   If you know a function f(x)'s value (f1) at position i1, and its value (f2) at position i2,
//   the function can be linearly interpolated with FLerp(f1,f2,i1,i2,x)
//    i2=i1 will cause a divide by zero.
float FLerp(float f1, float f2, float i1, float i2, float x);


#ifndef VECTOR_NO_SLOW_OPERATIONS

// YWB:  Specialization for interpolating euler angles via quaternions...
template<> FORCEINLINE QAngle Lerp<QAngle>( float flPercent, const QAngle& q1, const QAngle& q2 )
{
	// Avoid precision errors
	if ( q1 == q2 )
		return q1;

	Quaternion src, dest;

	// Convert to quaternions
	AngleQuaternion( q1, src );
	AngleQuaternion( q2, dest );

	Quaternion result;

	// Slerp
	QuaternionSlerp( src, dest, flPercent, result );

	// Convert to euler
	QAngle output;
	QuaternionAngles( result, output );
	return output;
}

#else

#pragma error

// NOTE NOTE: I haven't tested this!! It may not work! Check out interpolatedvar.cpp in the client dll to try it
template<> FORCEINLINE QAngleByValue Lerp<QAngleByValue>( float flPercent, const QAngleByValue& q1, const QAngleByValue& q2 )
{
	// Avoid precision errors
	if ( q1 == q2 )
		return q1;

	Quaternion src, dest;

	// Convert to quaternions
	AngleQuaternion( q1, src );
	AngleQuaternion( q2, dest );

	Quaternion result;

	// Slerp
	QuaternionSlerp( src, dest, flPercent, result );

	// Convert to euler
	QAngleByValue output;
	QuaternionAngles( result, output );
	return output;
}

#endif // VECTOR_NO_SLOW_OPERATIONS



template <class T> FORCEINLINE T AVG(T a, T b)
{
	return (a+b)/2;
}

// number of elements in an array of static size
#define NELEMS(x) ARRAYSIZE(x)

// XYZ macro, for printf type functions - ex printf("%f %f %f",XYZ(myvector));
#define XYZ(v) (v).x,(v).y,(v).z


float Sign( float x );

//
// Clamps the input integer to the given array bounds.
// Equivalent to the following, but without using any branches:
//
// if( n < 0 ) return 0;
// else if ( n > maxindex ) return maxindex;
// else return n;
//
// This is not always a clear performance win, but when you have situations where a clamped 
// value is thrashing against a boundary this is a big win. (ie, valid, invalid, valid, invalid, ...)
//
// Note: This code has been run against all possible integers.
//
int ClampArrayBounds( int n, unsigned maxindex );


#define BOX_ON_PLANE_SIDE(emins, emaxs, p)	\
	(((p)->type < 3)?						\
	(										\
		((p)->dist <= (emins)[(p)->type])?	\
			1								\
		:									\
		(									\
			((p)->dist >= (emaxs)[(p)->type])?\
				2							\
			:								\
				3							\
		)									\
	)										\
	:										\
		BoxOnPlaneSide( (emins), (emaxs), (p)))

//-----------------------------------------------------------------------------
// FIXME: Vector versions.... the float versions will go away hopefully soon!
//-----------------------------------------------------------------------------

void AngleVectors (const QAngle& angles, Vector *forward);
void AngleVectors (const QAngle& angles, Vector *forward, Vector *right, Vector *up);
void AngleVectorsTranspose (const QAngle& angles, Vector *forward, Vector *right, Vector *up);
void AngleMatrix (const QAngle &angles, matrix3x4_t &mat );
void AngleMatrix( const QAngle &angles, const Vector &position, matrix3x4_t &mat );
void AngleMatrix (const RadianEuler &angles, matrix3x4_t &mat );
void AngleMatrix( RadianEuler const &angles, const Vector &position, matrix3x4_t &mat );
void AngleIMatrix (const QAngle &angles, matrix3x4_t &mat );
void AngleIMatrix (const QAngle &angles, const Vector &position, matrix3x4_t &mat );
void AngleIMatrix (const RadianEuler &angles, matrix3x4_t &mat );
void VectorAngles( const Vector &forward, QAngle &angles );
void VectorAngles( const Vector &forward, const Vector &pseudoup, QAngle &angles );
void VectorMatrix( const Vector &forward, matrix3x4_t &mat );
void VectorVectors( const Vector &forward, Vector &right, Vector &up );
void SetIdentityMatrix( matrix3x4_t &mat );
void SetScaleMatrix( float x, float y, float z, matrix3x4_t &dst );
void MatrixBuildRotationAboutAxis( const Vector &vAxisOfRot, float angleDegrees, matrix3x4_t &dst );

void SetScaleMatrix( float flScale, matrix3x4_t &dst );

void SetScaleMatrix( const Vector& scale, matrix3x4_t &dst );

// Computes the inverse transpose
void MatrixTranspose( matrix3x4_t& mat );
void MatrixTranspose( const matrix3x4_t& src, matrix3x4_t& dst );
void MatrixInverseTranspose( const matrix3x4_t& src, matrix3x4_t& dst );

void PositionMatrix( const Vector &position, matrix3x4_t &mat );

void MatrixPosition( const matrix3x4_t &matrix, Vector &position );

void VectorRotate( const Vector& in1, const matrix3x4_t &in2, Vector &out);

void VectorIRotate( const Vector& in1, const matrix3x4_t &in2, Vector &out);

void MatrixAngles( const matrix3x4_t &matrix, QAngle &angles );

void MatrixAngles( const matrix3x4_t &matrix, QAngle &angles, Vector &position );

void MatrixAngles( const matrix3x4_t &matrix, RadianEuler &angles );

void MatrixAngles( const matrix3x4_t &mat, RadianEuler &angles, Vector &position );

void MatrixAngles( const matrix3x4_t &mat, Quaternion &q, Vector &position );

int VectorCompare (const Vector& v1, const Vector& v2);

void VectorTransform (const Vector& in1, const matrix3x4_t &in2, Vector &out);

void VectorITransform (const Vector& in1, const matrix3x4_t &in2, Vector &out);

/*
void DecomposeRotation( const matrix3x4_t &mat, Vector &out );
*/

int BoxOnPlaneSide (const Vector& emins, const Vector& emaxs, const cplane_t *plane );

void VectorFill(Vector& a, float b);

void VectorNegate(Vector& a);

vec_t VectorAvg(Vector& a);

//-----------------------------------------------------------------------------
// Box/plane test (slow version)
//-----------------------------------------------------------------------------
int FASTCALL BoxOnPlaneSide2 (const Vector& emins, const Vector& emaxs, const cplane_t *p, float tolerance = 0.f );

//-----------------------------------------------------------------------------
// Helpers for bounding box construction
//-----------------------------------------------------------------------------

void ClearBounds (Vector& mins, Vector& maxs);
void AddPointToBounds (const Vector& v, Vector& mins, Vector& maxs);

//
// COLORSPACE/GAMMA CONVERSION STUFF
//
void BuildGammaTable( float gamma, float texGamma, float brightness, int overbright );

// convert texture to linear 0..1 value
float TexLightToLinear( int c, int exponent );


// convert texture to linear 0..1 value
int LinearToTexture( float f );
// converts 0..1 linear value to screen gamma (0..255)
int LinearToScreenGamma( float f );
float TextureToLinear( int c );

// compressed color format 
struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};

void ColorRGBExp32ToVector( const ColorRGBExp32& in, Vector& out );
void VectorToColorRGBExp32( const Vector& v, ColorRGBExp32 &c );

// solve for "x" where "a x^2 + b x + c = 0", return true if solution exists
bool SolveQuadratic( float a, float b, float c, float &root1, float &root2 );

// solves for "a, b, c" where "a x^2 + b x + c = y", return true if solution exists
bool SolveInverseQuadratic( float x1, float y1, float x2, float y2, float x3, float y3, float &a, float &b, float &c );

// solves for a,b,c specified as above, except that it always creates a monotonically increasing or
// decreasing curve if the data is monotonically increasing or decreasing. In order to enforce the
// monoticity condition, it is possible that the resulting quadratic will only approximate the data
// instead of interpolating it. This code is not especially fast.
bool SolveInverseQuadraticMonotonic( float x1, float y1, float x2, float y2, 
									 float x3, float y3, float &a, float &b, float &c );




// solves for "a, b, c" where "1/(a x^2 + b x + c ) = y", return true if solution exists
bool SolveInverseReciprocalQuadratic( float x1, float y1, float x2, float y2, float x3, float y3, float &a, float &b, float &c );

// rotate a vector around the Z axis (YAW)
void VectorYawRotate( const Vector& in, float flYaw, Vector &out);


// Bias takes an X value between 0 and 1 and returns another value between 0 and 1
// The curve is biased towards 0 or 1 based on biasAmt, which is between 0 and 1.
// Lower values of biasAmt bias the curve towards 0 and higher values bias it towards 1.
//
// For example, with biasAmt = 0.2, the curve looks like this:
//
// 1
// |				  *
// |				  *
// |			     *
// |			   **
// |			 **
// |	  	 ****
// |*********
// |___________________
// 0                   1
//
//
// With biasAmt = 0.8, the curve looks like this:
//
// 1
// | 	**************
// |  **
// | * 
// | *
// |* 
// |* 
// |*  
// |___________________
// 0                   1
//
// With a biasAmt of 0.5, Bias returns X.
float Bias( float x, float biasAmt );


// Gain is similar to Bias, but biasAmt biases towards or away from 0.5.
// Lower bias values bias towards 0.5 and higher bias values bias away from it.
//
// For example, with biasAmt = 0.2, the curve looks like this:
//
// 1
// | 				  *
// | 				 *
// | 				**
// |  ***************
// | **
// | *
// |*
// |___________________
// 0                   1
//
//
// With biasAmt = 0.8, the curve looks like this:
//
// 1
// |  		    *****
// |  		 ***
// |  		*
// | 		*
// | 		*
// |   	 ***
// |*****
// |___________________
// 0                   1
float Gain( float x, float biasAmt );


// SmoothCurve maps a 0-1 value into another 0-1 value based on a cosine wave
// where the derivatives of the function at 0 and 1 (and 0.5) are 0. This is useful for
// any fadein/fadeout effect where it should start and end smoothly.
//
// The curve looks like this:
//
// 1
// |  		**
// | 	   *  *
// | 	  *	   *
// | 	  *	   *
// | 	 *		*
// |   **		 **
// |***			   ***
// |___________________
// 0                   1
//
float SmoothCurve( float x );


// This works like SmoothCurve, with two changes:
//
// 1. Instead of the curve peaking at 0.5, it will peak at flPeakPos.
//    (So if you specify flPeakPos=0.2, then the peak will slide to the left).
//
// 2. flPeakSharpness is a 0-1 value controlling the sharpness of the peak.
//    Low values blunt the peak and high values sharpen the peak.
float SmoothCurve_Tweak( float x, float flPeakPos=0.5, float flPeakSharpness=0.5 );


//float ExponentialDecay( float halflife, float dt );
//float ExponentialDecay( float decayTo, float decayTime, float dt );

// halflife is time for value to reach 50%
float ExponentialDecay( float halflife, float dt );

// decayTo is factor the value should decay to in decayTime
float ExponentialDecay( float decayTo, float decayTime, float dt );

// Get the integrated distanced traveled
// decayTo is factor the value should decay to in decayTime
// dt is the time relative to the last velocity update
float ExponentialDecayIntegral( float decayTo, float decayTime, float dt  );

// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
float SimpleSpline( float value );

// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
float SimpleSplineRemapVal( float val, float A, float B, float C, float D);

// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
float SimpleSplineRemapValClamped( float val, float A, float B, float C, float D );

int RoundFloatToInt(float f);

unsigned char RoundFloatToByte(float f);

unsigned long RoundFloatToUnsignedLong(float f);

bool IsIntegralValue( float flValue, float flTolerance = 0.001f );

// Fast, accurate ftol:
int Float2Int( float a );

// Over 15x faster than: (int)floor(value)
int Floor2Int( float a );

//-----------------------------------------------------------------------------
// Fast color conversion from float to unsigned char
//-----------------------------------------------------------------------------
unsigned int FastFToC( float c );

//-----------------------------------------------------------------------------
// Fast conversion from float to integer with magnitude less than 2**22
//-----------------------------------------------------------------------------
int FastFloatToSmallInt( float c );

//-----------------------------------------------------------------------------
// Purpose: Bound input float to .001 (millisecond) boundary
// Input  : in - 
// Output : inline float
//-----------------------------------------------------------------------------
float ClampToMsec( float in );

// Over 15x faster than: (int)ceil(value)
int Ceil2Int( float a );


// Regular signed area of triangle
#define TriArea2D( A, B, C ) \
	( 0.5f * ( ( B.x - A.x ) * ( C.y - A.y ) - ( B.y - A.y ) * ( C.x - A.x ) ) )

// This version doesn't premultiply by 0.5f, so it's the area of the rectangle instead
#define TriArea2DTimesTwo( A, B, C ) \
	( ( ( B.x - A.x ) * ( C.y - A.y ) - ( B.y - A.y ) * ( C.x - A.x ) ) )


// Get the barycentric coordinates of "pt" in triangle [A,B,C].
void GetBarycentricCoords2D( 
	Vector2D const &A,
	Vector2D const &B,
	Vector2D const &C,
	Vector2D const &pt,
	float bcCoords[3] );


// Return true of the sphere might touch the box (the sphere is actually treated
// like a box itself, so this may return true if the sphere's bounding box touches
// a corner of the box but the sphere itself doesn't).
bool QuickBoxSphereTest( 
	const Vector& vOrigin,
	float flRadius,
	const Vector& bbMin,
	const Vector& bbMax );


// Return true of the boxes intersect (but not if they just touch).
bool QuickBoxIntersectTest( 
	const Vector& vBox1Min,
	const Vector& vBox1Max,
	const Vector& vBox2Min,
	const Vector& vBox2Max );


extern float GammaToLinearFullRange( float gamma );
extern float LinearToGammaFullRange( float linear );
extern float GammaToLinear( float gamma );
extern float LinearToGamma( float linear );

extern float SrgbGammaToLinear( float flSrgbGammaValue );
extern float SrgbLinearToGamma( float flLinearValue );

// linear (0..4) to screen corrected vertex space (0..1?)
float LinearToVertexLight( float f );


unsigned char LinearToLightmap( float f );

void ColorClamp( Vector& color );

void ColorClampTruncate( Vector& color );

// Interpolate a Catmull-Rom spline.
// t is a [0,1] value and interpolates a curve between p2 and p3.
void Catmull_Rom_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// Interpolate a Catmull-Rom spline.
// Returns the tangent of the point at t of the spline
void Catmull_Rom_Spline_Tangent( 
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// area under the curve [0..t]
void Catmull_Rom_Spline_Integral( 
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// area under the curve [0..1]
void Catmull_Rom_Spline_Integral( 
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	Vector& output );

// Interpolate a Catmull-Rom spline.
// Normalize p2->p1 and p3->p4 to be the same length as p2->p3
void Catmull_Rom_Spline_Normalize(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// area under the curve [0..t]
// Normalize p2->p1 and p3->p4 to be the same length as p2->p3
void Catmull_Rom_Spline_Integral_Normalize(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// Interpolate a Catmull-Rom spline.
// Normalize p2.x->p1.x and p3.x->p4.x to be the same length as p2.x->p3.x
void Catmull_Rom_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// area under the curve [0..t]
void Catmull_Rom_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// Interpolate a Hermite spline.
// t is a [0,1] value and interpolates a curve between p1 and p2 with the deltas d1 and d2.
void Hermite_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &d1,
	const Vector &d2,
	float t, 
	Vector& output );

float Hermite_Spline(
	float p1,
	float p2,
	float d1,
	float d2,
	float t );

// t is a [0,1] value and interpolates a curve between p1 and p2 with the slopes p0->p1 and p1->p2
void Hermite_Spline(
	const Vector &p0,
	const Vector &p1,
	const Vector &p2,
	float t, 
	Vector& output );

float Hermite_Spline(
	float p0,
	float p1,
	float p2,
	float t );


void Hermite_SplineBasis( float t, float basis[4] );

void Hermite_Spline( 
	const Quaternion &q0, 
	const Quaternion &q1, 
	const Quaternion &q2, 
	float t, 
	Quaternion &output );


// See http://en.wikipedia.org/wiki/Kochanek-Bartels_curves
// 
// Tension:  -1 = Round -> 1 = Tight
// Bias:     -1 = Pre-shoot (bias left) -> 1 = Post-shoot (bias right)
// Continuity: -1 = Box corners -> 1 = Inverted corners
//
// If T=B=C=0 it's the same matrix as Catmull-Rom.
// If T=1 & B=C=0 it's the same as Cubic.
// If T=B=0 & C=-1 it's just linear interpolation
// 
// See http://news.povray.org/povray.binaries.tutorials/attachment/%3CXns91B880592482seed7@povray.org%3E/Splines.bas.txt
// for example code and descriptions of various spline types...
// 
void Kochanek_Bartels_Spline(
	float tension, 
	float bias, 
	float continuity,
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void Kochanek_Bartels_Spline_NormalizeX(
	float tension, 
	float bias, 
	float continuity,
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// See link at Kochanek_Bartels_Spline for info on the basis matrix used
void Cubic_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void Cubic_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// See link at Kochanek_Bartels_Spline for info on the basis matrix used
void BSpline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void BSpline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// See link at Kochanek_Bartels_Spline for info on the basis matrix used
void Parabolic_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void Parabolic_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// quintic interpolating polynomial from Perlin.
// 0->0, 1->1, smooth-in between with smooth tangents
float QuinticInterpolatingPolynomial(float t);

// given a table of sorted tabulated positions, return the two indices and blendfactor to linear
// interpolate. Does a search. Can be used to find the blend value to interpolate between
// keyframes.
void GetInterpolationData( float const *pKnotPositions, 
						   float const *pKnotValues,
						   int nNumValuesinList,
						   int nInterpolationRange,
						   float flPositionToInterpolateAt,
						   bool bWrap,
						   float *pValueA, 
						   float *pValueB,
						   float *pInterpolationValue);

float RangeCompressor( float flValue, float flMin, float flMax, float flBase );

// Get the minimum distance from vOrigin to the bounding box defined by [mins,maxs]
// using voronoi regions.
// 0 is returned if the origin is inside the box.
float CalcSqrDistanceToAABB( const Vector &mins, const Vector &maxs, const Vector &point );
void CalcClosestPointOnAABB( const Vector &mins, const Vector &maxs, const Vector &point, Vector &closestOut );
void CalcSqrDistAndClosestPointOnAABB( const Vector &mins, const Vector &maxs, const Vector &point, Vector &closestOut, float &distSqrOut );

float CalcDistanceToAABB( const Vector &mins, const Vector &maxs, const Vector &point );

// Get the closest point from P to the (infinite) line through vLineA and vLineB and
// calculate the shortest distance from P to the line.
// If you pass in a value for t, it will tell you the t for (A + (B-A)t) to get the closest point.
// If the closest point lies on the segment between A and B, then 0 <= t <= 1.
void  CalcClosestPointOnLine( const Vector &P, const Vector &vLineA, const Vector &vLineB, Vector &vClosest, float *t=0 );
float CalcDistanceToLine( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );
float CalcDistanceSqrToLine( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );

// The same three functions as above, except now the line is closed between A and B.
void  CalcClosestPointOnLineSegment( const Vector &P, const Vector &vLineA, const Vector &vLineB, Vector &vClosest, float *t=0 );
float CalcDistanceToLineSegment( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );
float CalcDistanceSqrToLineSegment( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );

// A function to compute the closes line segment connnection two lines (or false if the lines are parallel, etc.)
bool CalcLineToLineIntersectionSegment(
   const Vector& p1,const Vector& p2,const Vector& p3,const Vector& p4,Vector *s1,Vector *s2,
   float *t1, float *t2 );

// The above functions in 2D
void  CalcClosestPointOnLine2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, Vector2D &vClosest, float *t=0 );
float CalcDistanceToLine2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );
float CalcDistanceSqrToLine2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );
void  CalcClosestPointOnLineSegment2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, Vector2D &vClosest, float *t=0 );
float CalcDistanceToLineSegment2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );
float CalcDistanceSqrToLineSegment2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );

// Init the mathlib
void MathLib_Init( float gamma = 2.2f, float texGamma = 2.2f, float brightness = 0.0f, int overbright = 2.0f, bool bAllow3DNow = true, bool bAllowSSE = true, bool bAllowSSE2 = true, bool bAllowMMX = true );
bool MathLib_3DNowEnabled( void );
bool MathLib_MMXEnabled( void );
bool MathLib_SSEEnabled( void );
bool MathLib_SSE2Enabled( void );

float Approach( float target, float value, float speed );
float ApproachAngle( float target, float value, float speed );
float AngleDiff( float destAngle, float srcAngle );
float AngleDistance( float next, float cur );
float AngleNormalize( float angle );

// ensure that 0 <= angle <= 360
float AngleNormalizePositive( float angle );

bool AnglesAreEqual( float a, float b, float tolerance = 0.0f );


void RotationDeltaAxisAngle( const QAngle &srcAngles, const QAngle &destAngles, Vector &deltaAxis, float &deltaAngle );
void RotationDelta( const QAngle &srcAngles, const QAngle &destAngles, QAngle *out );

void ComputeTrianglePlane( const Vector& v1, const Vector& v2, const Vector& v3, Vector& normal, float& intercept );
int PolyFromPlane( Vector *outVerts, const Vector& normal, float dist, float fHalfScale = 9000.0f );
int ClipPolyToPlane( Vector *inVerts, int vertCount, Vector *outVerts, const Vector& normal, float dist, float fOnPlaneEpsilon = 0.1f );
int ClipPolyToPlane_Precise( double *inVerts, int vertCount, double *outVerts, const double *normal, double dist, double fOnPlaneEpsilon = 0.1 );

//-----------------------------------------------------------------------------
// Computes a reasonable tangent space for a triangle
//-----------------------------------------------------------------------------
void CalcTriangleTangentSpace( const Vector &p0, const Vector &p1, const Vector &p2,
							  const Vector2D &t0, const Vector2D &t1, const Vector2D& t2,
							  Vector &sVect, Vector &tVect );

//-----------------------------------------------------------------------------
// Transforms a AABB into another space; which will inherently grow the box.
//-----------------------------------------------------------------------------
void TransformAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Uses the inverse transform of in1
//-----------------------------------------------------------------------------
void ITransformAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Rotates a AABB into another space; which will inherently grow the box. 
// (same as TransformAABB, but doesn't take the translation into account)
//-----------------------------------------------------------------------------
void RotateAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Uses the inverse transform of in1
//-----------------------------------------------------------------------------
void IRotateAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Transform a plane
//-----------------------------------------------------------------------------
void MatrixTransformPlane( const matrix3x4_t &src, const cplane_t &inPlane, cplane_t &outPlane );

void MatrixITransformPlane( const matrix3x4_t &src, const cplane_t &inPlane, cplane_t &outPlane );

int CeilPow2( int in );
int FloorPow2( int in );

float * UnpackNormal_HEND3N( const unsigned int *pPackedNormal, float *pNormal );

unsigned int * PackNormal_HEND3N( const float *pNormal, unsigned int *pPackedNormal );

unsigned int * PackNormal_HEND3N( float nx, float ny, float nz, unsigned int *pPackedNormal );

float * UnpackNormal_SHORT2( const unsigned int *pPackedNormal, float *pNormal, bool bIsTangent = FALSE );

unsigned int * PackNormal_SHORT2( float nx, float ny, float nz, unsigned int *pPackedNormal, float binormalSign = +1.0f );

unsigned int * PackNormal_SHORT2( const float *pNormal, unsigned int *pPackedNormal, float binormalSign = +1.0f );

// Unpacks a UBYTE4 normal (for a tangent, the result's fourth component receives the binormal 'sign')
float * UnpackNormal_UBYTE4( const unsigned int *pPackedNormal, float *pNormal, bool bIsTangent = FALSE );

//////////////////////////////////////////////////////////////////////////////
// See: http://www.oroboro.com/rafael/docserv.php/index/programming/article/unitv2
//
// UBYTE4 encoding, using per-octant projection onto x+y+z=1
// Assume input vector is already unit length
//
// binormalSign specifies 'sign' of binormal, stored in t sign bit of tangent
// (lets the shader know whether norm/tan/bin form a right-handed basis)
//
// bIsTangent is used to specify which WORD of the output to store the data
// The expected usage is to call once with the normal and once with
// the tangent and binormal sign flag, bitwise OR'ing the returned DWORDs
unsigned int * PackNormal_UBYTE4( float nx, float ny, float nz, unsigned int *pPackedNormal, bool bIsTangent = false, float binormalSign = +1.0f );

unsigned int * PackNormal_UBYTE4( const float *pNormal, unsigned int *pPackedNormal, bool bIsTangent = false, float binormalSign = +1.0f );

void RGB2YUV( int &nR, int &nG, int &nB, float &fY, float &fU, float &fV, bool bApplySaturationCurve );

//-----------------------------------------------------------------------------
// Convert RGB to HSV
//-----------------------------------------------------------------------------
void RGBtoHSV( const Vector &rgb, Vector &hsv );


//-----------------------------------------------------------------------------
// Convert HSV to RGB
//-----------------------------------------------------------------------------
void HSVtoRGB( const Vector &hsv, Vector &rgb );


//-----------------------------------------------------------------------------
// Fast version of pow and log
//-----------------------------------------------------------------------------

float FastLog2(float i);			// log2( i )
float FastPow2(float i);			// 2^i
float FastPow(float a, float b);	// a^b
float FastPow10( float i );			// 10^i

//-----------------------------------------------------------------------------
// For testing float equality
//-----------------------------------------------------------------------------

bool CloseEnough( float a, float b, float epsilon = EQUAL_EPSILON );

bool CloseEnough( const Vector &a, const Vector &b, float epsilon = EQUAL_EPSILON );

// Fast compare
// maxUlps is the maximum error in terms of Units in the Last Place. This 
// specifies how big an error we are willing to accept in terms of the value
// of the least significant digit of the floating point number�s 
// representation. maxUlps can also be interpreted in terms of how many 
// representable floats we are willing to accept between A and B. 
// This function will allow maxUlps-1 floats between A and B.
bool AlmostEqual(float a, float b, int maxUlps = 10);

bool AlmostEqual( const Vector &a, const Vector &b, int maxUlps = 10);

#endif	// MATH_LIB_H
