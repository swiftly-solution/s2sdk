#include "public/mathlib/vector4d.h"
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


bool Vector4D::IsZero( float tolerance) const
{
	return (x > -tolerance && x < tolerance &&
		y > -tolerance && y < tolerance &&
		z > -tolerance && z < tolerance &&
		w > -tolerance && w < tolerance);
}

Vector4DAligned::Vector4DAligned(void)
{}

__m128 &Vector4DAligned::AsM128()
{ return *(__m128*)&x; }

const __m128 &Vector4DAligned::AsM128() const
{ return *(const __m128*)&x; } 

Vector4D::Vector4D(void)
{ 
#ifdef _DEBUG
	// Initialize to NAN to catch errors
	x = y = z = w = VEC_T_NAN;
#endif
}

Vector4D::Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W )
{ 
	x = X; y = Y; z = Z; w = W;
	Assert( IsValid() );
}

Vector4D::Vector4D(const float *pFloat)
{
	Assert( pFloat );
	x = pFloat[0]; y = pFloat[1]; z = pFloat[2]; w = pFloat[3];	
	Assert( IsValid() );
}

Vector4D::Vector4D(const Vector4D &vOther)
{ 
	Assert( vOther.IsValid() );
	x = vOther.x; y = vOther.y; z = vOther.z; w = vOther.w;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
void Vector4D::Init( vec_t ix, vec_t iy, vec_t iz, vec_t iw )
{ 
	x = ix; y = iy; z = iz;	w = iw;
	Assert( IsValid() );
}

void Vector4D::Init( const Vector& src, vec_t iw )
{
	x = src.x; y = src.y; z = src.z; w = iw;
	Assert( IsValid() );
}

void Vector4D::Random( vec_t minVal, vec_t maxVal )
{
	x = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	y = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	z = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
	w = minVal + (static_cast<vec_t>(rand()) / static_cast<vec_t>(RAND_MAX)) * (maxVal - minVal);
}

void Vector4DClear( Vector4D& a )
{
	a.x = a.y = a.z = a.w = 0.0f;
}

Vector4D& Vector4D::operator=(const Vector4D &vOther)
{
	Assert( vOther.IsValid() );
	x=vOther.x; y=vOther.y; z=vOther.z; w=vOther.w;
	return *this; 
}

vec_t& Vector4D::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

vec_t Vector4D::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

Vector& Vector4D::AsVector3D()
{
	return *(Vector*)this;
}

Vector const& Vector4D::AsVector3D() const
{
	return *(Vector const*)this;
}

Vector2D& Vector4D::AsVector2D()
{
	return *(Vector2D*)this;
}

Vector2D const& Vector4D::AsVector2D() const
{
	return *(Vector2D const*)this;
}

vec_t* Vector4D::Base()
{
	return (vec_t*)this;
}

vec_t const* Vector4D::Base() const
{
	return (vec_t const*)this;
}

bool Vector4D::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w);
}

bool Vector4D::operator==( Vector4D const& src ) const
{
	Assert( src.IsValid() && IsValid() );
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

bool Vector4D::operator!=( Vector4D const& src ) const
{
	Assert( src.IsValid() && IsValid() );
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}

void Vector4DCopy( Vector4D const& src, Vector4D& dst )
{
	Assert( src.IsValid() );
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
	dst.w = src.w;
}

void	Vector4D::CopyToArray(float* rgfl) const
{ 
	Assert( IsValid() );
	Assert( rgfl );
	rgfl[0] = x; rgfl[1] = y; rgfl[2] = z; rgfl[3] = w;
}

void Vector4D::Negate()
{ 
	Assert( IsValid() );
	x = -x; y = -y; z = -z; w = -w;
} 

Vector4D& Vector4D::operator+=(const Vector4D& v)
{ 
	Assert( IsValid() && v.IsValid() );
	x+=v.x; y+=v.y; z += v.z; w += v.w;	
	return *this;
}

Vector4D& Vector4D::operator-=(const Vector4D& v)
{ 
	Assert( IsValid() && v.IsValid() );
	x-=v.x; y-=v.y; z -= v.z; w -= v.w;
	return *this;
}

Vector4D& Vector4D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	w *= fl;
	Assert( IsValid() );
	return *this;
}

Vector4D& Vector4D::operator*=(Vector4D const& v)
{ 
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	Assert( IsValid() );
	return *this;
}

Vector4D Vector4D::operator-(void) const
{ 
	return Vector4D(-x,-y,-z,-w);				
}

Vector4D Vector4D::operator+(const Vector4D& v) const
{ 
	Vector4D res;
	Vector4DAdd( *this, v, res );
	return res;	
}

Vector4D Vector4D::operator-(const Vector4D& v) const
{ 
	Vector4D res;
	Vector4DSubtract( *this, v, res );
	return res;	
}

Vector4D Vector4D::operator*(float fl) const
{ 
	Vector4D res;
	Vector4DMultiply( *this, fl, res );
	return res;	
}

Vector4D Vector4D::operator*(const Vector4D& v) const
{ 
	Vector4D res;
	Vector4DMultiply( *this, v, res );
	return res;	
}

Vector4D Vector4D::operator/(float fl) const
{ 
	Vector4D res;
	Vector4DDivide( *this, fl, res );
	return res;	
}

Vector4D operator*( float fl, const Vector4D& v )
{ 
	return v * fl; 
}

Vector4D& Vector4D::operator/=(float fl)
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	w *= oofl;
	Assert( IsValid() );
	return *this;
}

Vector4D& Vector4D::operator/=(Vector4D const& v)
{ 
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
	Assert( IsValid() );
	return *this;
}

void Vector4DAdd( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	c.w = a.w + b.w;
}

void Vector4DSubtract( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	c.w = a.w - b.w;
}

void Vector4DMultiply( Vector4D const& a, vec_t b, Vector4D& c )
{
	Assert( a.IsValid() && IsFinite(b) );
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
	c.w = a.w * b;
}

void Vector4DMultiply( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
	c.w = a.w * b.w;
}

void Vector4DDivide( Vector4D const& a, vec_t b, Vector4D& c )
{
	Assert( a.IsValid() );
	Assert( b != 0.0f );
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
	c.z = a.z * oob;
	c.w = a.w * oob;
}

void Vector4DDivide( Vector4D const& a, Vector4D const& b, Vector4D& c )
{
	Assert( a.IsValid() );
	Assert( (b.x != 0.0f) && (b.y != 0.0f) && (b.z != 0.0f) && (b.w != 0.0f) );
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
	c.w = a.w / b.w;
}

void Vector4DMA( Vector4D const& start, float s, Vector4D const& dir, Vector4D& result )
{
	Assert( start.IsValid() && IsFinite(s) && dir.IsValid() );
	result.x = start.x + s*dir.x;
	result.y = start.y + s*dir.y;
	result.z = start.z + s*dir.z;
	result.w = start.w + s*dir.w;
}

// FIXME: Remove
// For backwards compatability
void	Vector4D::MulAdd(Vector4D const& a, Vector4D const& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
	z = a.z + b.z * scalar;
	w = a.w + b.w * scalar;
}

void Vector4DLerp(const Vector4D& src1, const Vector4D& src2, vec_t t, Vector4D& dest )
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
	dest[2] = src1[2] + (src2[2] - src1[2]) * t;
	dest[3] = src1[3] + (src2[3] - src1[3]) * t;
}

vec_t DotProduct4D(const Vector4D& a, const Vector4D& b)
{ 
	Assert( a.IsValid() && b.IsValid() );
	return( a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w ); 
}

// for backwards compatability
vec_t Vector4D::Dot( Vector4D const& vOther ) const
{
	return DotProduct4D( *this, vOther );
}

vec_t Vector4DLength( Vector4D const& v )
{				   
	Assert( v.IsValid() );
	return (vec_t)FastSqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);		
}

vec_t Vector4D::LengthSqr(void) const
{ 
	Assert( IsValid() );
	return (x*x + y*y + z*z + w*w);		
}

vec_t Vector4D::Length(void) const
{
	return Vector4DLength( *this );
}

// FIXME: Can't use until we're un-macroed in mathlib.h
vec_t Vector4DNormalize( Vector4D& v )
{
	Assert( v.IsValid() );
	vec_t l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = v.z = v.w = 0.0f;
	}
	return l;
}

vec_t Vector4D::DistTo(const Vector4D &vOther) const
{
	Vector4D delta;
	Vector4DSubtract( *this, vOther, delta );
	return delta.Length();
}

vec_t Vector4D::DistToSqr(const Vector4D &vOther) const
{
	Vector4D delta;
	Vector4DSubtract( *this, vOther, delta );
	return delta.LengthSqr();
}

Vector4DAligned::Vector4DAligned( vec_t X, vec_t Y, vec_t Z, vec_t W )
{ 
	x = X; y = Y; z = Z; w = W;
	Assert( IsValid() );
}

void Vector4DAligned::Set( vec_t X, vec_t Y, vec_t Z, vec_t W )
{ 
	x = X; y = Y; z = Z; w = W;
	Assert( IsValid() );
}

void Vector4DAligned::InitZero( void )
{ 
	this->AsM128() = _mm_set1_ps( 0.0f );
	Assert( IsValid() );
}

void Vector4DMultiplyAligned( Vector4DAligned const& a, Vector4DAligned const& b, Vector4DAligned& c )
{
	Assert( a.IsValid() && b.IsValid() );
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
	c.w = a.w * b.w;
}

void Vector4DWeightMAD( vec_t w, Vector4DAligned const& vInA, Vector4DAligned& vOutA, Vector4DAligned const& vInB, Vector4DAligned& vOutB )
{
	Assert( vInA.IsValid() && vInB.IsValid() && IsFinite(w) );

	vOutA.x += vInA.x * w;
	vOutA.y += vInA.y * w;
	vOutA.z += vInA.z * w;
	vOutA.w += vInA.w * w;

	vOutB.x += vInB.x * w;
	vOutB.y += vInB.y * w;
	vOutB.z += vInB.z * w;
	vOutB.w += vInB.w * w;
}

void Vector4DWeightMADSSE( vec_t w, Vector4DAligned const& vInA, Vector4DAligned& vOutA, Vector4DAligned const& vInB, Vector4DAligned& vOutB )
{
	Assert( vInA.IsValid() && vInB.IsValid() && IsFinite(w) );

	// Replicate scalar float out to 4 components
	__m128 packed = _mm_set1_ps( w );

	// 4D SSE Vector MAD
	vOutA.AsM128() = _mm_add_ps( vOutA.AsM128(), _mm_mul_ps( vInA.AsM128(), packed ) );
	vOutB.AsM128() = _mm_add_ps( vOutB.AsM128(), _mm_mul_ps( vInB.AsM128(), packed ) );
}
