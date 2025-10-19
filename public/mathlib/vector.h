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

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?
#include "tier0/basetypes.h"

// For rand(). We really need a library!
#include <stdlib.h>

#ifndef _X360
// For MMX intrinsics
#include <xmmintrin.h>
#endif

#ifndef ALIGN16_POST
#define ALIGN16_POST
#endif

#include "tier0/dbg.h"
#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "mathlib/vector2d.h"
#include "tier0/memalloc.h"

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


class Vector					
{
public:
	// Members
	float x, y, z;

	Vector(void) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	Vector(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}

	// Initialization
	void Init(float ix=0.0f, float iy=0.0f, float iz=0.0f) {
		x = ix;
		y = iy;
		z = iz;
	}
	 // TODO (Ilya): Should there be an init that takes a single float for consistency?


	// Base address...
	float* Base() {
		return &x;
	}
	float const* Base() const {
		return &x;
	}

	float& operator[](int i) {
		return ((float*)this)[i];
	}
	float operator[](int i) const {
		return ((float*)this)[i];
	}


	inline void Zero() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	// equality
	bool operator==(const Vector& v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	bool operator!=(const Vector& v) const {
		return x != v.x || y != v.y || z != v.z;
	}

	// arithmetic operations
	Vector&	operator+=(const Vector &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector&	operator-=(const Vector &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector&	operator*=(const Vector &v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector&	operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector&	operator/=(const Vector &v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	Vector&	operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
	Vector&	operator+=(float fl) {
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}
	Vector&	operator-=(float fl) {
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}
	Vector operator+(const Vector& v) const {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) const {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	Vector operator*(const Vector& v) const {
		return Vector(x * v.x, y * v.y, z * v.z);
	}
	Vector operator/(const Vector& v) const {
		return Vector(x / v.x, y / v.y, z / v.z);
	}
	Vector operator*(float s) const {
		return Vector(x * s, y * s, z * s);
	}
	Vector operator/(float s) const {
		return Vector(x / s, y / s, z / s);
	}
	Vector operator+(float s) const {
		return Vector(x + s, y + s, z + s);
	}


} ALIGN16_POST;


class ALIGN16 VectorAligned : public Vector
{
public:
	inline VectorAligned(void) {};
	inline VectorAligned(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	VectorAligned(const VectorAligned& vOther);
	VectorAligned(const Vector& vOther);

#else
public:
	explicit VectorAligned(const Vector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned& operator=(const Vector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

#endif
	float w;	// this space is used anyway
} ALIGN16_POST;


class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	inline Quaternion(void)	{
		// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
		x = y = z = w = VEC_T_NAN;
#endif
#endif
	}
	inline Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw) : x(ix), y(iy), z(iz), w(iw) { }

	inline void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f)	{ x = ix; y = iy; z = iz; w = iw; }

	bool IsValid() const;
	void Invalidate();

	bool operator==( const Quaternion &src ) const;
	bool operator!=( const Quaternion &src ) const;

	vec_t* Base() { return (vec_t*)this; }
	const vec_t* Base() const { return (vec_t*)this; }

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z, w;
} ALIGN16_POST;


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& Quaternion::operator[](int i)
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}

inline vec_t Quaternion::operator[](int i) const
{
	Assert( (i >= 0) && (i < 4) );
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Equality test
//-----------------------------------------------------------------------------
inline bool Quaternion::operator==( const Quaternion &src ) const
{
	return ( x == src.x ) && ( y == src.y ) && ( z == src.z ) && ( w == src.w );
}

inline bool Quaternion::operator!=( const Quaternion &src ) const
{
	return !operator==( src );
}


//-----------------------------------------------------------------------------
// Quaternion equality with tolerance
//-----------------------------------------------------------------------------
inline bool QuaternionsAreEqual( const Quaternion& src1, const Quaternion& src2, float tolerance )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	if (FloatMakePositive(src1.z - src2.z) > tolerance)
		return false;
	return (FloatMakePositive(src1.w - src2.w) <= tolerance);
}


//-----------------------------------------------------------------------------
// Here's where we add all those lovely SSE optimized routines
//-----------------------------------------------------------------------------
class ALIGN16 QuaternionAligned : public Quaternion
{
public:
	inline QuaternionAligned(void) {};
	inline QuaternionAligned(vec_t X, vec_t Y, vec_t Z, vec_t W) 
	{
		Init(X,Y,Z,W);
	}

	operator Quaternion * () { return this; } 
	operator const Quaternion * () { return this; } 

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	QuaternionAligned(const QuaternionAligned& vOther);
	QuaternionAligned(const Quaternion &vOther);

#else
public:
	explicit QuaternionAligned(const Quaternion &vOther) 
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
	}

	QuaternionAligned& operator=(const Quaternion &vOther)	
	{
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
		return *this;
	}

	QuaternionAligned& operator=(const QuaternionAligned &vOther)
	{
		// we know we're aligned, so use simd
		// we can't use the convenient abstract interface coz it gets declared later
#ifdef _X360
		XMStoreVector4A(Base(), XMLoadVector4A(vOther.Base()));
#elif _WIN32
		_mm_store_ps(Base(), _mm_load_ps( vOther.Base() ));
#else
		Init(vOther.x, vOther.y, vOther.z, vOther.w);
#endif
		return *this;
	}

#endif

#if !defined(NO_MALLOC_OVERRIDE)
	void* operator new[] ( size_t nSize)
	{
		return MemAlloc_AllocAligned(nSize, 16);
	}

	void* operator new[] ( size_t nSize, const char *pFileName, int nLine)
	{
		return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}

	void* operator new[] ( size_t nSize, int /*nBlockUse*/, const char *pFileName, int nLine)
	{
		return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}

	void operator delete[] ( void* p) 
	{
		MemAlloc_FreeAligned(p);
	}

	void operator delete[] ( void* p, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	void operator delete[] ( void* p, int /*nBlockUse*/, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	// please don't allocate a single quaternion...
	void* operator new   ( size_t nSize )
	{
		return MemAlloc_AllocAligned(nSize, 16);
	}
	void* operator new   ( size_t nSize, const char *pFileName, int nLine )
	{
		return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}
	void* operator new   ( size_t nSize, int /*nBlockUse*/, const char *pFileName, int nLine )
	{
		return MemAlloc_AllocAlignedFileLine(nSize, 16, pFileName, nLine);
	}
	void operator delete ( void* p) 
	{
		MemAlloc_FreeAligned(p);
	}

	void operator delete ( void* p, const char *pFileName, int nLine)  
	{
		MemAlloc_FreeAligned(p, pFileName, nLine);
	}

	void operator delete (void *p, int /*nBlockUse*/, const char *pFileName, int nLine)
	{
		MemAlloc_FreeAligned(p, pFileName, nLine);
	}
#endif
} ALIGN16_POST;


//-----------------------------------------------------------------------------
// Radian Euler angle aligned to axis (NOT ROLL/PITCH/YAW)
//-----------------------------------------------------------------------------
class QAngle;


inline bool Quaternion::IsValid() const
{
	return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w);
}

inline void Quaternion::Invalidate()
{
//#ifdef _DEBUG
//#ifdef VECTOR_PARANOIA
	x = y = z = w = VEC_T_NAN;
//#endif
//#endif
}




class QAngle					
{
public:
	// Members
	float x, y, z;

	// Construction/destruction
	QAngle(void) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	QAngle(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}



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
	
	// equality
	bool operator==(const QAngle& v) const {
		return x == v.x && y == v.y && z == v.z;	
	}
	bool operator!=(const QAngle& v) const {
		return x != v.x || y != v.y || z != v.z;
	}

	// arithmetic operations
	QAngle&	operator+=(const QAngle &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	QAngle&	operator-=(const QAngle &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	QAngle&	operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	QAngle&	operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}


	// No assignment operators either...
	QAngle& operator=( const QAngle& src ) {
		x = src.x;
		y = src.y;
		z = src.z;
		return *this;
	}

};










#endif

