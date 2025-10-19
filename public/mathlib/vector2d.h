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

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?
#include "tier0/basetypes.h"

// For rand(). We really need a library!
#include <stdlib.h>

#include "tier0/dbg.h"

//=========================================================
// 2D Vector2D
//=========================================================

class Vector2D					
{
public:
	// Members
	float x, y;

	// Construction/destruction
	Vector2D(void) {
		x = 0.0f;
		y = 0.0f;
	}
	Vector2D(float X, float Y) {
		x = X;
		y = Y;
	}
	Vector2D(const float *pFloat) {
		x = pFloat[0];
		y = pFloat[1];
	}

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f) {
		x = ix;
		y = iy;
	}


	// array access...
	float operator[](int i) const {
		return ((float*)this)[i];
	}
	float& operator[](int i) {
		return ((float*)this)[i];
	}

	// Base address...
	float* Base() {
		return (float*)this;
	}
	float const* Base() const {
		return (float const*)this;
	}

	bool operator==(const Vector2D& v) const {
		return x == v.x && y == v.y;
	}
	bool operator!=(const Vector2D& v) const {
		return x != v.x || y != v.y;
	}

	// arithmetic operations
	Vector2D&	operator+=(const Vector2D &v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2D&	operator-=(const Vector2D &v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector2D&	operator*=(const Vector2D &v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector2D&	operator*=(float s) {
		x *= s;
		y *= s;
		return *this;
	}
	Vector2D&	operator/=(const Vector2D &v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}
	Vector2D&	operator/=(float s) {
		x /= s;
		y /= s;
		return *this;
	}


	Vector2D& operator=(const Vector2D &vOther) {
		x = vOther.x;
		y = vOther.y;
		return *this;
	}
};

const Vector2D vec2_origin(0,0);
const Vector2D vec2_invalid( FLT_MAX, FLT_MAX );


#endif // VECTOR2D_H

