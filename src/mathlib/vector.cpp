#include "public/mathlib/vector.h"
#include "public/mathlib/mathlib.h"

#include <xmmintrin.h>

#ifndef ThreadPause
#define ThreadPause() _mm_pause()
#endif

#ifndef COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT( pred ) static_assert( pred, "Compile time assert constraint is not true: " #pred )
#endif

#ifndef VEC_T_NAN
#define VEC_T_NAN FLOAT32_NAN
#endif


vec_t Vector::LengthSqr(void) const
{ 
	CHECK_VALID(*this);
	return (x*x + y*y + z*z);		
}

vec_t Vector::LengthRecipFast(void) const
{ 
	return FastRSqrtFast( LengthSqr() );
}

bool Vector::IsZero( float tolerance) const
{
	return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
}

bool Vector::IsZeroFast( ) const RESTRICT
{
	COMPILE_TIME_ASSERT( sizeof(vec_t) == sizeof(int) );
	return ( FloatBits( x ) == 0 && 
			 FloatBits( y ) == 0 && 
			 FloatBits( z ) == 0 );
}

vec_t Vector::DistToSqr(const Vector &vOther) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}

Vector::operator VectorByValue &()
{ return *((VectorByValue *)(this)); }

Vector::operator const VectorByValue &() const
{ return *((const VectorByValue *)(this)); }

VectorByValue::VectorByValue(void)
: Vector() {} 

VectorByValue::VectorByValue(vec_t X, vec_t Y, vec_t Z)
: Vector( X, Y, Z ) {}

VectorByValue::VectorByValue(const VectorByValue& vOther)
{ *this = vOther; }

TableVector::operator Vector &()
{ return *((Vector *)(this)); }

TableVector::operator const Vector &() const
{ return *((const Vector *)(this)); }

vec_t& TableVector::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

vec_t TableVector::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

Vector ReplicateToVector( float x )
{
	return Vector( x, x, x );
}

// check if a point is in the field of a view of an object.
bool PointWithinViewAngle( Vector const &vecSrcPosition, 
									   Vector const &vecTargetPosition, 
									   Vector const &vecLookDirection, float flCosHalfFOV )
{
	Vector vecDelta = vecTargetPosition - vecSrcPosition;
	float cosDiff = DotProduct( vecLookDirection, vecDelta );
	
	if ( flCosHalfFOV <= 0 ) // >180
	{
		// signs are different, answer is implicit
		if ( cosDiff > 0 )
			return true;

		// a/sqrt(b) > c  == a^2 < b * c ^2
		// IFF left and right sides are <= 0
		float flLen2 = vecDelta.LengthSqr();
		return ( cosDiff * cosDiff <= flLen2 * flCosHalfFOV * flCosHalfFOV );
	}
	else // flCosHalfFOV > 0
	{
		// signs are different, answer is implicit
		if ( cosDiff < 0 )
			return false;

		// a/sqrt(b) > c  == a^2 > b * c ^2
		// IFF left and right sides are >= 0
		float flLen2 = vecDelta.LengthSqr();
		return ( cosDiff * cosDiff >= flLen2 * flCosHalfFOV * flCosHalfFOV );
	}
}

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
Vector::Vector(void)
{ 
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

Vector::Vector(vec_t X, vec_t Y, vec_t Z)
{ 
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}

#if 0
Vector::Vector(const Vector &vOther)
{ 
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
}

#endif
void Vector::Init( vec_t ix, vec_t iy, vec_t iz )
{ 
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}

void Vector::Random( vec_t minVal, vec_t maxVal )
{
	x = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	y = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	z = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	CHECK_VALID(*this);
}

void Vector::Zero()
{
	x = y = z = 0.0f;
}

void VectorClear( Vector& a )
{
	a.x = a.y = a.z = 0.0f;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& Vector::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

vec_t Vector::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
vec_t* Vector::Base()
{
	return (vec_t*)this;
}

vec_t const* Vector::Base() const
{
	return (vec_t const*)this;
}

Vector2D& Vector::AsVector2D()
{
	return *(Vector2D*)this;
}

const Vector2D& Vector::AsVector2D() const
{
	return *(const Vector2D*)this;
}

bool Vector::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z);
}

void Vector::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
//#endif
//#endif
}

bool Vector::operator==( const Vector& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

bool Vector::operator!=( const Vector& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}

void VectorCopy( const Vector& src, Vector& dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void	Vector::CopyToArray(float* rgfl) const
{ 
	Assert( rgfl );
	CHECK_VALID(*this);
	rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; 
}

void Vector::Negate()
{ 
	CHECK_VALID(*this);
	x = -x; y = -y; z = -z; 
} 

Vector& Vector::operator+=(const Vector& v)
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;	
	return *this;
}

Vector& Vector::operator-=(const Vector& v)
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;	
	return *this;
}

Vector& Vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

Vector& Vector::operator*=(const Vector& v)
{ 
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}

// this ought to be an opcode.
Vector&	Vector::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}

Vector&	Vector::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}

Vector& Vector::operator/=(float fl)
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}

Vector& Vector::operator/=(const Vector& v)
{ 
	CHECK_VALID(v);
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}

// get the component of this vector parallel to some other given vector
Vector Vector::ProjectOnto( const Vector& onto )
{
	return onto * ( this->Dot(onto) / ( onto.LengthSqr() ) );
}

void ShortVector::Init( short ix, short iy, short iz, short iw )
{ 
	x = ix; y = iy; z = iz; w = iw;
}

void ShortVector::Set( const ShortVector& vOther )
{
   x = vOther.x;
   y = vOther.y;
   z = vOther.z;
   w = vOther.w;
}

void ShortVector::Set( const short ix, const short iy, const short iz, const short iw )
{
   x = ix;
   y = iy;
   z = iz;
   w = iw;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
short ShortVector::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((short*)this)[i];
}

short& ShortVector::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((short*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
short* ShortVector::Base()
{
	return (short*)this;
}

short const* ShortVector::Base() const
{
	return (short const*)this;
}

bool ShortVector::operator==( const ShortVector& src ) const
{
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

bool ShortVector::operator!=( const ShortVector& src ) const
{
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}

ShortVector& ShortVector::operator+=(const ShortVector& v)
{ 
	x+=v.x; y+=v.y; z += v.z; w += v.w;
	return *this;
}

ShortVector& ShortVector::operator-=(const ShortVector& v)
{ 
	x-=v.x; y-=v.y; z -= v.z; w -= v.w;
	return *this;
}

ShortVector& ShortVector::operator*=(float fl)
{
	x = (short)(x * fl);
	y = (short)(y * fl);
	z = (short)(z * fl);
	w = (short)(w * fl);
	return *this;
}

ShortVector& ShortVector::operator*=(const ShortVector& v)
{ 
	x = (short)(x * v.x);
	y = (short)(y * v.y);
	z = (short)(z * v.z);
	w = (short)(w * v.w);
	return *this;
}

ShortVector& ShortVector::operator/=(float fl)
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x = (short)(x * oofl);
	y = (short)(y * oofl);
	z = (short)(z * oofl);
	w = (short)(w * oofl);
	return *this;
}

ShortVector& ShortVector::operator/=(const ShortVector& v)
{ 
	Assert( v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0 );
	x = (short)(x / v.x);
	y = (short)(y / v.y);
	z = (short)(z / v.z);
	w = (short)(w / v.w);
	return *this;
}

void ShortVectorMultiply( const ShortVector& src, float fl, ShortVector& res )
{
	Assert( IsFinite(fl) );
	res.x = (short)(src.x * fl);
	res.y = (short)(src.y * fl);
	res.z = (short)(src.z * fl);
	res.w = (short)(src.w * fl);
}

ShortVector ShortVector::operator*(float fl) const
{ 
	ShortVector res;
	ShortVectorMultiply( *this, fl, res );
	return res;	
}

void IntVector4D::Init( int ix, int iy, int iz, int iw )
{ 
	x = ix; y = iy; z = iz; w = iw;
}

void IntVector4D::Set( const IntVector4D& vOther )
{
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	w = vOther.w;
}

void IntVector4D::Set( const int ix, const int iy, const int iz, const int iw )
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
int IntVector4D::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((int*)this)[i];
}

int& IntVector4D::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((int*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
int* IntVector4D::Base()
{
	return (int*)this;
}

int const* IntVector4D::Base() const
{
	return (int const*)this;
}

bool IntVector4D::operator==( const IntVector4D& src ) const
{
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

bool IntVector4D::operator!=( const IntVector4D& src ) const
{
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}

IntVector4D& IntVector4D::operator+=(const IntVector4D& v)
{ 
	x+=v.x; y+=v.y; z += v.z; w += v.w;
	return *this;
}

IntVector4D& IntVector4D::operator-=(const IntVector4D& v)
{ 
	x-=v.x; y-=v.y; z -= v.z; w -= v.w;
	return *this;
}

IntVector4D& IntVector4D::operator*=(float fl)
{
	x = (int)(x * fl);
	y = (int)(y * fl);
	z = (int)(z * fl);
	w = (int)(w * fl);
	return *this;
}

IntVector4D& IntVector4D::operator*=(const IntVector4D& v)
{ 
	x = (int)(x * v.x);
	y = (int)(y * v.y);
	z = (int)(z * v.z);
	w = (int)(w * v.w);
	return *this;
}

IntVector4D& IntVector4D::operator/=(float fl)
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x = (int)(x * oofl);
	y = (int)(y * oofl);
	z = (int)(z * oofl);
	w = (int)(w * oofl);
	return *this;
}

IntVector4D& IntVector4D::operator/=(const IntVector4D& v)
{ 
	Assert( v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0 );
	x = (int)(x / v.x);
	y = (int)(y / v.y);
	z = (int)(z / v.z);
	w = (int)(w / v.w);
	return *this;
}

void IntVector4DMultiply( const IntVector4D& src, float fl, IntVector4D& res )
{
	Assert( IsFinite(fl) );
	res.x = (int)(src.x * fl);
	res.y = (int)(src.y * fl);
	res.z = (int)(src.z * fl);
	res.w = (int)(src.w * fl);
}

IntVector4D IntVector4D::operator*(float fl) const
{ 
	IntVector4D res;
	IntVector4DMultiply( *this, fl, res );
	return res;	
}

void VectorAdd( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

void VectorSubtract( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

void VectorMultiply( const Vector& a, vec_t b, Vector& c )
{
	CHECK_VALID(a);
	Assert( IsFinite(b) );
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

void VectorMultiply( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
}

// for backwards compatability
void VectorScale ( const Vector& in, vec_t scale, Vector& result )
{
	VectorMultiply( in, scale, result );
}

void VectorDivide( const Vector& a, vec_t b, Vector& c )
{
	CHECK_VALID(a);
	Assert( b != 0.0f );
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
	c.z = a.z * oob;
}

void VectorDivide( const Vector& a, const Vector& b, Vector& c )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	Assert( (b.x != 0.0f) && (b.y != 0.0f) && (b.z != 0.0f) );
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
}

// FIXME: Remove
// For backwards compatability
void	Vector::MulAdd(const Vector& a, const Vector& b, float scalar)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
	z = a.z + b.z * scalar;
}

void VectorLerp(const Vector& src1, const Vector& src2, vec_t t, Vector& dest )
{
	CHECK_VALID(src1);
	CHECK_VALID(src2);
	dest.x = src1.x + (src2.x - src1.x) * t;
	dest.y = src1.y + (src2.y - src1.y) * t;
	dest.z = src1.z + (src2.z - src1.z) * t;
}

Vector VectorLerp(const Vector& src1, const Vector& src2, vec_t t )
{
	Vector result;
	VectorLerp( src1, src2, t, result );
	return result;
}

//-----------------------------------------------------------------------------
// Temporary storage for vector results so const Vector& results can be returned
//-----------------------------------------------------------------------------
Vector &AllocTempVector()
{
	static Vector s_vecTemp[128];
	static CInterlockedInt s_nIndex;

	int nIndex;
	for (;;)
	{
		int nOldIndex = s_nIndex;
		nIndex = ( (nOldIndex + 0x10001) & 0x7F );

		if ( s_nIndex.AssignIf( nOldIndex, nIndex ) )
		{
			break;
		}
		ThreadPause();
	} 
	return s_vecTemp[nIndex & 0xffff];
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
vec_t DotProduct(const Vector& a, const Vector& b)
{ 
	CHECK_VALID(a);
	CHECK_VALID(b);
	return( a.x*b.x + a.y*b.y + a.z*b.z ); 
}

// for backwards compatability
vec_t Vector::Dot( const Vector& vOther ) const
{
	CHECK_VALID(vOther);
	return DotProduct( *this, vOther );
}

int Vector::LargestComponent() const
{
	float flAbsx = fabs(x);
	float flAbsy = fabs(y);
	float flAbsz = fabs(z);
	if ( flAbsx > flAbsy )
	{
		if ( flAbsx > flAbsz )
			return X_INDEX;
		return Z_INDEX;
	}
	if ( flAbsy > flAbsz )
		return Y_INDEX;
	return Z_INDEX;
}

void CrossProduct(const Vector& a, const Vector& b, Vector& result )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	Assert( &a != &result );
	Assert( &b != &result );
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
}

vec_t DotProductAbs( const Vector &v0, const Vector &v1 )
{
	CHECK_VALID(v0);
	CHECK_VALID(v1);
	return FloatMakePositive(v0.x*v1.x) + FloatMakePositive(v0.y*v1.y) + FloatMakePositive(v0.z*v1.z);
}

vec_t DotProductAbs( const Vector &v0, const float *v1 )
{
	return FloatMakePositive(v0.x * v1[0]) + FloatMakePositive(v0.y * v1[1]) + FloatMakePositive(v0.z * v1[2]);
}

vec_t VectorLength( const Vector& v )
{
	CHECK_VALID(v);
	return (vec_t)FastSqrt(v.x*v.x + v.y*v.y + v.z*v.z);		
}

vec_t Vector::Length(void) const
{
	CHECK_VALID(*this);
	return VectorLength( *this );
}

// check a point against a box
bool Vector::WithinAABox( Vector const &boxmin, Vector const &boxmax)
{
	return ( 
		( x >= boxmin.x ) && ( x <= boxmax.x) &&
		( y >= boxmin.y ) && ( y <= boxmax.y) &&
		( z >= boxmin.z ) && ( z <= boxmax.z)
		);
}

//-----------------------------------------------------------------------------
// Get the distance from this vector to the other one 
//-----------------------------------------------------------------------------
vec_t Vector::DistTo(const Vector &vOther) const
{
	Vector delta;
	VectorSubtract( *this, vOther, delta );
	return delta.Length();
}

//-----------------------------------------------------------------------------
// Vector equality with tolerance
//-----------------------------------------------------------------------------
bool VectorsAreEqual( const Vector& src1, const Vector& src2, float tolerance )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	return (FloatMakePositive(src1.z - src2.z) <= tolerance);
}

//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint( const Vector& vecStart, float flMaxDist, const Vector& vecTarget, Vector *pResult )
{
	Vector vecDelta;
	VectorSubtract( vecTarget, vecStart, vecDelta );
	float flDistSqr = vecDelta.LengthSqr();
	if ( flDistSqr <= flMaxDist * flMaxDist )
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt( flDistSqr );
		VectorMA( vecStart, flMaxDist, vecDelta, *pResult );
	}
}

//-----------------------------------------------------------------------------
// Takes the absolute value of a vector
//-----------------------------------------------------------------------------
void VectorAbs( const Vector& src, Vector& dst )
{
	dst.x = FloatMakePositive(src.x);
	dst.y = FloatMakePositive(src.y);
	dst.z = FloatMakePositive(src.z);
}

#ifndef VECTOR_NO_SLOW_OPERATIONS
//-----------------------------------------------------------------------------
// Returns a vector with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------
Vector Vector::Min(const Vector &vOther) const
{
	return Vector(x < vOther.x ? x : vOther.x, 
		y < vOther.y ? y : vOther.y, 
		z < vOther.z ? z : vOther.z);
}

Vector Vector::Max(const Vector &vOther) const
{
	return Vector(x > vOther.x ? x : vOther.x, 
		y > vOther.y ? y : vOther.y, 
		z > vOther.z ? z : vOther.z);
}

Vector Vector::operator-(void) const
{ 
	return Vector(-x,-y,-z);				
}

Vector Vector::operator+(const Vector& v) const
{ 
	Vector res;
	VectorAdd( *this, v, res );
	return res;	
}

Vector Vector::operator-(const Vector& v) const
{ 
	Vector res;
	VectorSubtract( *this, v, res );
	return res;	
}

Vector Vector::operator*(float fl) const
{ 
	Vector res;
	VectorMultiply( *this, fl, res );
	return res;	
}

Vector Vector::operator*(const Vector& v) const
{ 
	Vector res;
	VectorMultiply( *this, v, res );
	return res;	
}

Vector Vector::operator/(float fl) const
{ 
	Vector res;
	VectorDivide( *this, fl, res );
	return res;	
}

Vector Vector::operator/(const Vector& v) const
{ 
	Vector res;
	VectorDivide( *this, v, res );
	return res;	
}

Vector operator*(float fl, const Vector& v)
{ 
	return v * fl; 
}

Vector Vector::Cross(const Vector& vOther) const
{ 
	Vector res;
	CrossProduct( *this, vOther, res );
	return res;
}

vec_t Vector::Length2D(void) const
{ 
	return (vec_t)FastSqrt(x*x + y*y); 
}

vec_t Vector::Length2DSqr(void) const
{ 
	return (x*x + y*y); 
}

Vector CrossProduct(const Vector& a, const Vector& b)
{ 
	return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); 
}

void VectorMin( const Vector &a, const Vector &b, Vector &result )
{
	result.x = fpmin(a.x, b.x);
	result.y = fpmin(a.y, b.y);
	result.z = fpmin(a.z, b.z);
}

void VectorMax( const Vector &a, const Vector &b, Vector &result )
{
	result.x = fpmax(a.x, b.x);
	result.y = fpmax(a.y, b.y);
	result.z = fpmax(a.z, b.z);
}

// and when you want to return the vector rather than cause a LHS with it...
Vector VectorMin( const Vector &a, const Vector &b )
{
	return Vector( fpmin(a.x, b.x), fpmin(a.y, b.y), fpmin(a.z, b.z) );
}

Vector VectorMax( const Vector &a, const Vector &b )
{
	return Vector( fpmax(a.x, b.x), fpmax(a.y, b.y), fpmax(a.z, b.z) );
}

float ComputeVolume( const Vector &vecMins, const Vector &vecMaxs )
{
	Vector vecDelta;
	VectorSubtract( vecMaxs, vecMins, vecDelta );
	return DotProduct( vecDelta, vecDelta );
}

// Get a random vector.
Vector RandomVector( float minVal, float maxVal )
{
	Vector random;
	random.Random( minVal, maxVal );
	return random;
}

#endif
bool operator==( float const* f, const Vector& v )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

bool operator==( const Vector& v, float const* f )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

bool operator!=( float const* f, const Vector& v )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

bool operator!=( const Vector& v, float const* f )
{
	// AIIIEEEE!!!!
	Assert(0);
	return false;
}

#ifndef VECTOR_NO_SLOW_OPERATIONS
AngularImpulse RandomAngularImpulse( float minVal, float maxVal )
{
	AngularImpulse	angImp;
	angImp.Random( minVal, maxVal );
	return angImp;
}

#endif
Quaternion::Quaternion(void)
{ 

// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	x = y = z = w = VEC_T_NAN;
#endif
#endif
}

Quaternion::Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw)
: x(ix), y(iy), z(iz), w(iw) { }

void Quaternion::Init(vec_t ix, vec_t iy, vec_t iz, vec_t iw)
{ x = ix; y = iy; z = iz; w = iw; }

vec_t* Quaternion::Base()
{ return (vec_t*)this; }

const vec_t* Quaternion::Base() const
{ return (vec_t*)this; }

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& Quaternion::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

vec_t Quaternion::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Equality test
//-----------------------------------------------------------------------------
bool Quaternion::operator==( const Quaternion &src ) const
{
	return ( x == src.x ) && ( y == src.y ) && ( z == src.z ) && ( w == src.w );
}

bool Quaternion::operator!=( const Quaternion &src ) const
{
	return !operator==( src );
}

//-----------------------------------------------------------------------------
// Quaternion equality with tolerance
//-----------------------------------------------------------------------------
bool QuaternionsAreEqual( const Quaternion& src1, const Quaternion& src2, float tolerance )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	if (FloatMakePositive(src1.z - src2.z) > tolerance)
		return false;
	return (FloatMakePositive(src1.w - src2.w) <= tolerance);
}

RadianEuler::RadianEuler(void)
{ }

RadianEuler::RadianEuler(vec_t X, vec_t Y, vec_t Z)
{ x = X; y = Y; z = Z; }

void RadianEuler::Init(vec_t ix, vec_t iy, vec_t iz)
{ x = ix; y = iy; z = iz; }

vec_t *RadianEuler::Base()
{ return &x; }

const vec_t *RadianEuler::Base() const
{ return &x; } 

Quaternion::Quaternion(RadianEuler const &angle)
{
	AngleQuaternion( angle, *this );
}

bool Quaternion::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w);
}

void Quaternion::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = w = VEC_T_NAN;
//#endif
//#endif
}

RadianEuler::RadianEuler(Quaternion const &q)
{
	QuaternionAngles( q, *this );
}

void VectorCopy( RadianEuler const& src, RadianEuler &dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

void VectorScale( RadianEuler const& src, float b, RadianEuler &dst )
{
	CHECK_VALID(src);
	Assert( IsFinite(b) );
	dst.x = src.x * b;
	dst.y = src.y * b;
	dst.z = src.z * b;
}

bool RadianEuler::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z);
}

void RadianEuler::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
//#endif
//#endif
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& RadianEuler::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

vec_t RadianEuler::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

QAngle::operator QAngleByValue &()
{ return *((QAngleByValue *)(this)); }

QAngle::operator const QAngleByValue &() const
{ return *((const QAngleByValue *)(this)); }

QAngleByValue::QAngleByValue(void)
: QAngle() {} 

QAngleByValue::QAngleByValue(vec_t X, vec_t Y, vec_t Z)
: QAngle( X, Y, Z ) {}

QAngleByValue::QAngleByValue(const QAngleByValue& vOther)
{ *this = vOther; }

void VectorAdd( const QAngle& a, const QAngle& b, QAngle& result )
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
}

void VectorMA( const QAngle &start, float scale, const QAngle &direction, QAngle &dest )
{
	CHECK_VALID(start);
	CHECK_VALID(direction);
	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
QAngle::QAngle(void)
{ 
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

QAngle::QAngle(vec_t X, vec_t Y, vec_t Z)
{ 
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
void QAngle::Init( vec_t ix, vec_t iy, vec_t iz )
{ 
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}

void QAngle::Random( vec_t minVal, vec_t maxVal )
{
	x = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	y = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	z = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	CHECK_VALID(*this);
}

#ifndef VECTOR_NO_SLOW_OPERATIONS
QAngle RandomAngle( float minVal, float maxVal )
{
	Vector random;
	random.Random( minVal, maxVal );
	QAngle ret( random.x, random.y, random.z );
	return ret;
}

#endif
RadianEuler::RadianEuler(QAngle const &angles)
{
	Init(
		angles.z * 3.14159265358979323846f / 180.f,
		angles.x * 3.14159265358979323846f / 180.f, 
		angles.y * 3.14159265358979323846f / 180.f );
}

QAngle RadianEuler::ToQAngle( void) const
{
	return QAngle(
		y * 180.f / 3.14159265358979323846f,
		z * 180.f / 3.14159265358979323846f,
		x * 180.f / 3.14159265358979323846f );
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& QAngle::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

vec_t QAngle::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
vec_t* QAngle::Base()
{
	return (vec_t*)this;
}

vec_t const* QAngle::Base() const
{
	return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------
bool QAngle::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z);
}

void QAngle::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
//#endif
//#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
bool QAngle::operator==( const QAngle& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

bool QAngle::operator!=( const QAngle& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}

//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
void VectorCopy( const QAngle& src, QAngle& dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
QAngle& QAngle::operator+=(const QAngle& v)
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;	
	return *this;
}

QAngle& QAngle::operator-=(const QAngle& v)
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;	
	return *this;
}

QAngle& QAngle::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

QAngle& QAngle::operator/=(float fl)
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
vec_t QAngle::Length( ) const
{
	CHECK_VALID(*this);
	return (vec_t)FastSqrt( LengthSqr( ) );		
}

vec_t QAngle::LengthSqr( ) const
{
	CHECK_VALID(*this);
	return x * x + y * y + z * z;
}

//-----------------------------------------------------------------------------
// Vector equality with tolerance
//-----------------------------------------------------------------------------
bool QAnglesAreEqual( const QAngle& src1, const QAngle& src2, float tolerance)
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	return (FloatMakePositive(src1.z - src2.z) <= tolerance);
}

#ifndef VECTOR_NO_SLOW_OPERATIONS
QAngle QAngle::operator-(void) const
{ 
	QAngle ret(-x,-y,-z);
	return ret;
}

QAngle QAngle::operator+(const QAngle& v) const
{ 
	QAngle res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;	
}

QAngle QAngle::operator-(const QAngle& v) const
{ 
	QAngle res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;	
}

QAngle QAngle::operator*(float fl) const
{ 
	QAngle res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;	
}

QAngle QAngle::operator/(float fl) const
{ 
	QAngle res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;	
}

QAngle operator*(float fl, const QAngle& v)
{ 
        QAngle ret( v * fl );
	return ret;
}

#endif
//-----------------------------------------------------------------------------
// NOTE: These are not completely correct.  The representations are not equivalent
// unless the QAngle represents a rotational impulse along a coordinate axis (x,y,z)
void QAngleToAngularImpulse( const QAngle &angles, AngularImpulse &impulse )
{
	impulse.x = angles.z;
	impulse.y = angles.x;
	impulse.z = angles.y;
}

void AngularImpulseToQAngle( const AngularImpulse &impulse, QAngle &angles )
{
	angles.x = impulse.y;
	angles.y = impulse.z;
	angles.z = impulse.x;
}

vec_t InvRSquared( const float* v )
{
	return 1.0f / MAX( 1.0f, v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

vec_t InvRSquared( const Vector &v )
{
	return InvRSquared( v.Base() );
}

// FIXME: Obsolete version of VectorNormalize, once we remove all the friggin float*s
float VectorNormalize( float * v )
{
	return VectorNormalize(*(reinterpret_cast<Vector *>(v)));
}

void VectorNormalizeFast (Vector& vec)
{
	float ool = FastRSqrt( FLT_EPSILON + vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );

	vec.x *= ool;
	vec.y *= ool;
	vec.z *= ool;
}

vec_t Vector::NormalizeInPlace()
{
	return VectorNormalize( *this );
}

Vector Vector::Normalized() const
{
	Vector norm = *this;
	VectorNormalize( norm );
	return norm;
}

bool Vector::IsLengthGreaterThan( float val ) const
{
	return LengthSqr() > val*val;
}

bool Vector::IsLengthLessThan( float val ) const
{
	return LengthSqr() < val*val;
}
