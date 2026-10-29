#include "public/mathlib/vector2d.h"
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


bool Vector2D::IsZero( float tolerance) const
{
	return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
}

Vector2D::Vector2D(void)
{ 
#ifdef _DEBUG
	// Initialize to NAN to catch errors
	x = y = VEC_T_NAN;
#endif
}

Vector2D::Vector2D(vec_t X, vec_t Y)
{ 
	x = X; y = Y;
	Assert( IsValid() );
}

Vector2D::Vector2D(const float *pFloat)
{
	Assert( pFloat );
	x = pFloat[0]; y = pFloat[1];	
	Assert( IsValid() );
}

void Vector2D::Init( vec_t ix, vec_t iy )
{ 
	x = ix; y = iy;
	Assert( IsValid() );
}

void Vector2D::Random( float minVal, float maxVal )
{
	x = minVal + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (maxVal - minVal);
	y = minVal + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (maxVal - minVal);
}

void Vector2DClear( Vector2D& a )
{
	a.x = a.y = 0.0f;
}

vec_t& Vector2D::operator[](int i)
{
	Assert( (i >= 0) && (i < 2) );
	return ((vec_t*)this)[i];
}

vec_t Vector2D::operator[](int i) const
{
	Assert( (i >= 0) && (i < 2) );
	return ((vec_t*)this)[i];
}

vec_t* Vector2D::Base()
{
	return (vec_t*)this;
}

vec_t const* Vector2D::Base() const
{
	return (vec_t const*)this;
}

bool Vector2D::IsValid() const
{
	return IsFinite(x) && IsFinite(y);
}

bool Vector2D::operator==( const Vector2D& src ) const
{
	Assert( src.IsValid() && IsValid() );
	return (src.x == x) && (src.y == y);
}

bool Vector2D::operator!=( const Vector2D& src ) const
{
	Assert( src.IsValid() && IsValid() );
	return (src.x != x) || (src.y != y);
}

void Vector2DCopy( const Vector2D& src, Vector2D& dst )
{
	Assert( src.IsValid() );
	dst.x = src.x;
	dst.y = src.y;
}

void	Vector2D::CopyToArray(float* rgfl) const
{ 
	Assert( IsValid() );
	Assert( rgfl );
	rgfl[0] = x; rgfl[1] = y; 
}

void Vector2D::Negate()
{ 
	Assert( IsValid() );
	x = -x; y = -y;
} 

Vector2D& Vector2D::operator+=(const Vector2D& v)
{ 
	Assert( IsValid() && v.IsValid() );
	x+=v.x; y+=v.y;	
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v)
{ 
	Assert( IsValid() && v.IsValid() );
	x-=v.x; y-=v.y;	
	return *this;
}

Vector2D& Vector2D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	Assert( IsValid() );
	return *this;
}

Vector2D& Vector2D::operator*=(const Vector2D& v)
{ 
	x *= v.x;
	y *= v.y;
	Assert( IsValid() );
	return *this;
}

Vector2D& Vector2D::operator/=(float fl)
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	Assert( IsValid() );
	return *this;
}

Vector2D& Vector2D::operator/=(const Vector2D& v)
{ 
	Assert( v.x != 0.0f && v.y != 0.0f );
	x /= v.x;
	y /= v.y;
	Assert( IsValid() );
	return *this;
}

void Vector2DAdd( const Vector2D& a, const Vector2D& b, Vector2D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x + b.x;
	c.y = a.y + b.y;
}

void Vector2DSubtract( const Vector2D& a, const Vector2D& b, Vector2D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}

void Vector2DMultiply( const Vector2D& a, vec_t b, Vector2D& c )
{
	Assert( a.IsValid() && IsFinite(b) );
	c.x = a.x * b;
	c.y = a.y * b;
}

void Vector2DMultiply( const Vector2D& a, const Vector2D& b, Vector2D& c )
{				  
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x * b.x;
	c.y = a.y * b.y;
}

void Vector2DDivide( const Vector2D& a, vec_t b, Vector2D& c )
{
	Assert( a.IsValid() );
	Assert( b != 0.0f );
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
}

void Vector2DDivide( const Vector2D& a, const Vector2D& b, Vector2D& c )
{
	Assert( a.IsValid() );
	Assert( (b.x != 0.0f) && (b.y != 0.0f) );
	c.x = a.x / b.x;
	c.y = a.y / b.y;
}

void Vector2DMA( const Vector2D& start, float s, const Vector2D& dir, Vector2D& result )
{
	Assert( start.IsValid() && IsFinite(s) && dir.IsValid() );
	result.x = start.x + s*dir.x;
	result.y = start.y + s*dir.y;
}

// FIXME: Remove
// For backwards compatability
void	Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
}

void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest )
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
vec_t DotProduct2D(const Vector2D& a, const Vector2D& b)
{ 
	Assert( a.IsValid() && b.IsValid() );
	return( a.x*b.x + a.y*b.y ); 
}

// for backwards compatability
vec_t Vector2D::Dot( const Vector2D& vOther ) const
{
	return DotProduct2D( *this, vOther );
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
vec_t Vector2DLength( const Vector2D& v )
{
	Assert( v.IsValid() );
	return (vec_t)FastSqrt(v.x*v.x + v.y*v.y);		
}

vec_t Vector2D::LengthSqr(void) const
{ 
	Assert( IsValid() );
	return (x*x + y*y);		
}

vec_t Vector2D::NormalizeInPlace()
{
	return Vector2DNormalize( *this );
}

bool Vector2D::IsLengthGreaterThan( float val ) const
{
	return LengthSqr() > val*val;
}

bool Vector2D::IsLengthLessThan( float val ) const
{
	return LengthSqr() < val*val;
}

vec_t Vector2D::Length(void) const
{
	return Vector2DLength( *this );
}

void Vector2DMin( const Vector2D &a, const Vector2D &b, Vector2D &result )
{
	result.x = (a.x < b.x) ? a.x : b.x;
	result.y = (a.y < b.y) ? a.y : b.y;
}

void Vector2DMax( const Vector2D &a, const Vector2D &b, Vector2D &result )
{
	result.x = (a.x > b.x) ? a.x : b.x;
	result.y = (a.y > b.y) ? a.y : b.y;
}

//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------
vec_t Vector2DNormalize( Vector2D& v )
{
	Assert( v.IsValid() );
	vec_t l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f; 
	}
	return l;
}

//-----------------------------------------------------------------------------
// Get the distance from this Vector2D to the other one 
//-----------------------------------------------------------------------------
vec_t Vector2D::DistTo(const Vector2D &vOther) const
{
	Vector2D delta;
	Vector2DSubtract( *this, vOther, delta );
	return delta.Length();
}

vec_t Vector2D::DistToSqr(const Vector2D &vOther) const
{
	Vector2D delta;
	Vector2DSubtract( *this, vOther, delta );
	return delta.LengthSqr();
}

//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint2D( const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D *pResult )
{
	Vector2D vecDelta;
	Vector2DSubtract( vecTarget, vecStart, vecDelta );
	float flDistSqr = vecDelta.LengthSqr();
	if ( flDistSqr <= flMaxDist * flMaxDist )
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt( flDistSqr );
		Vector2DMA( vecStart, flMaxDist, vecDelta, *pResult );
	}
}

#ifndef VECTOR_NO_SLOW_OPERATIONS
Vector2D Vector2D::Min(const Vector2D &vOther) const
{
	return Vector2D(x < vOther.x ? x : vOther.x, 
		y < vOther.y ? y : vOther.y);
}

Vector2D Vector2D::Max(const Vector2D &vOther) const
{
	return Vector2D(x > vOther.x ? x : vOther.x, 
		y > vOther.y ? y : vOther.y);
}

Vector2D Vector2D::operator-(void) const
{ 
	return Vector2D(-x,-y);				
}

Vector2D Vector2D::operator+(const Vector2D& v) const
{ 
	Vector2D res;
	Vector2DAdd( *this, v, res );
	return res;	
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{ 
	Vector2D res;
	Vector2DSubtract( *this, v, res );
	return res;	
}

Vector2D Vector2D::operator*(float fl) const
{ 
	Vector2D res;
	Vector2DMultiply( *this, fl, res );
	return res;	
}

Vector2D Vector2D::operator*(const Vector2D& v) const
{ 
	Vector2D res;
	Vector2DMultiply( *this, v, res );
	return res;	
}

Vector2D Vector2D::operator/(float fl) const
{ 
	Vector2D res;
	Vector2DDivide( *this, fl, res );
	return res;	
}

Vector2D Vector2D::operator/(const Vector2D& v) const
{ 
	Vector2D res;
	Vector2DDivide( *this, v, res );
	return res;	
}

Vector2D operator*(float fl, const Vector2D& v)
{ 
	return v * fl; 
}

#endif
