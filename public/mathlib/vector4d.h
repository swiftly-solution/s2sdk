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
#include <stdlib.h>		// for rand(). we really need a library!
#include <float.h>
#if !defined( _X360 )
#include <xmmintrin.h>	// for sse
#endif
#include "tier0/basetypes.h"	// For vec_t, put this somewhere else?
#include "tier0/dbg.h"
#include "mathlib/vector.h"

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
	float x, y, z, w;

	// Construction/destruction
	Vector4D(void) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
	Vector4D(float X, float Y, float Z, float W) {
		x = X;
		y = Y;
		z = Z;
		w = W;
	}
	Vector4D(const float *pFloat) {
		x = pFloat[0];
		y = pFloat[1];
		z = pFloat[2];
		w = pFloat[3];
	}



	// array access...
	vec_t operator[](int i) const {
		return ((float*)this)[i];
	}
	vec_t& operator[](int i) {
		return ((float*)this)[i];
	}

	// Base address...
	inline float* Base() { return &x; }
	inline float const* Base() const { return &x; }



	bool operator==(const Vector4D& v) const {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}
	bool operator!=(const Vector4D& v) const {
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	Vector4D&	operator+=(const Vector4D &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}			
	Vector4D&	operator-=(const Vector4D &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	Vector4D&	operator*=(const Vector4D &v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	Vector4D&	operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	Vector4D&	operator/=(const Vector4D &v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}
	Vector4D&	operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}

	Vector4D	operator-( void ) const {
		return Vector4D(-x, -y, -z, -w);
	}
	Vector4D	operator*( float fl ) const {
		return Vector4D(x * fl, y * fl, z * fl, w * fl);
	}
	Vector4D	operator/( float fl ) const {
		return Vector4D(x / fl, y / fl, z / fl, w / fl);
	}
	Vector4D	operator*( const Vector4D& v ) const {
		return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	Vector4D	operator+( const Vector4D& v ) const {
		return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	Vector4D	operator-( const Vector4D& v ) const {
		return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4D& operator=( Vector4D const& src ) {
		x = src.x;
		y = src.y;
		z = src.z;
		w = src.w;
		return *this;
	}
};

const Vector4D vec4_origin( 0.0f, 0.0f, 0.0f, 0.0f );
const Vector4D vec4_invalid( FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX );



#endif // VECTOR4D_H

