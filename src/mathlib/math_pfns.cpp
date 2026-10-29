#include "public/mathlib/math_pfns.h"

#include <xmmintrin.h>

float FastSqrt( float x )
{
	__m128 root = _mm_sqrt_ss( _mm_load_ss( &x ) );
	return *( reinterpret_cast<float *>( &root ) );
}

float FastRSqrtFast( float x )
{
	// use intrinsics
	__m128 rroot = _mm_rsqrt_ss( _mm_load_ss( &x ) );
	return *( reinterpret_cast<float *>( &rroot ) );
}

float FastRSqrt( float x )
{
	float rroot = FastRSqrtFast( x );
	return (0.5f * rroot) * (3.f - (x * rroot) * rroot);
}

float FastRecip(float x) {return 1.0f / x;}
float FastSqrtEst(float x) { return FastRSqrtFast(x) * x; }

float FastClampInfinity( float x )
{
	return ( x > FLT_MAX ? FLT_MAX : x );
}
