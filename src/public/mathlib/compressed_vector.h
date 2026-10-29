//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef COMPRESSED_VECTOR_H
#define COMPRESSED_VECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?
#include "public/tier0/platform.h"

// For rand(). We really need a library!
#include "public/tier0/stdlib.h"

#include "public/mathlib/vector.h"

#include "public/mathlib/mathlib.h"

//=========================================================
// fit a 3D vector into 32 bits
//=========================================================

class Vector32
{
public:
	// Construction/destruction:
	Vector32(void);
	Vector32(vec_t X, vec_t Y, vec_t Z);

	// assignment
	Vector32& operator=(const Vector &vOther);
	operator Vector ();

private:
	unsigned short x:10;
	unsigned short y:10;
	unsigned short z:10;
	unsigned short exp:2;
};

//=========================================================
// Fit a unit vector into 32 bits
//=========================================================

class Normal32
{
public:
	// Construction/destruction:
	Normal32(void);
	Normal32(vec_t X, vec_t Y, vec_t Z);

	// assignment
	Normal32& operator=(const Vector &vOther);
	operator Vector ();

private:
	unsigned short x:15;
	unsigned short y:15;
	unsigned short zneg:1;
};

//=========================================================
// 64 bit Quaternion
//=========================================================

class Quaternion64
{
public:
	// Construction/destruction:
	Quaternion64(void);
	Quaternion64(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion64 &vOther);
	Quaternion64& operator=(const Quaternion &vOther);
	operator Quaternion ();
private:
	uint64 x:21;
	uint64 y:21;
	uint64 z:21;
	uint64 wneg:1;
};

//=========================================================
// 48 bit Quaternion
//=========================================================

class Quaternion48
{
public:
	// Construction/destruction:
	Quaternion48(void);
	Quaternion48(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion48& operator=(const Quaternion &vOther);
	operator Quaternion ();
//private:
	unsigned short x:16;
	unsigned short y:16;
	unsigned short z:15;
	unsigned short wneg:1;
};

//=========================================================
// 32 bit Quaternion
//=========================================================

class Quaternion32
{
public:
	// Construction/destruction:
	Quaternion32(void);
	Quaternion32(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion32& operator=(const Quaternion &vOther);
	operator Quaternion ();
private:
	unsigned int x:11;
	unsigned int y:10;
	unsigned int z:10;
	unsigned int wneg:1;
};

//=========================================================
// 16 bit float
//=========================================================


const int float32bias = 127;
const int float16bias = 15;

const float maxfloat16bits = 65504.0f;

class float16
{
public:
	//float16() {}
	//float16( float f ) { m_storage.rawWord = ConvertFloatTo16bits(f); }

	void Init();
//	float16& operator=(const float16 &other) { m_storage.rawWord = other.m_storage.rawWord; return *this; }
//	float16& operator=(const float &other) { m_storage.rawWord = ConvertFloatTo16bits(other); return *this; }
//	operator unsigned short () { return m_storage.rawWord; }
//	operator float () { return Convert16bitFloatTo32bits( m_storage.rawWord ); }
	unsigned short GetBits() const;
	float GetFloat() const;
	void SetFloat( float in );

	bool IsInfinity() const;
	bool IsNaN() const;

	bool operator==(const float16 other) const;
	bool operator!=(const float16 other) const;

//	bool operator< (const float other) const	   { return GetFloat() < other; }
//	bool operator> (const float other) const	   { return GetFloat() > other; }

protected:
	union float32bits
	{
		float rawFloat;
		struct
		{
			unsigned int mantissa : 23;
			unsigned int biased_exponent : 8;
			unsigned int sign : 1;
		} bits;
	};

	union float16bits
	{
		unsigned short rawWord;
		struct
		{
			unsigned short mantissa : 10;
			unsigned short biased_exponent : 5;
			unsigned short sign : 1;
		} bits;
	};

	static bool IsNaN( float16bits in );
	static bool IsInfinity( float16bits in );

	// 0x0001 - 0x03ff
	static unsigned short ConvertFloatTo16bits( float input );

	static float Convert16bitFloatTo32bits( unsigned short input );


	float16bits m_storage;
};

class float16_with_assign : public float16
{
public:
	float16_with_assign();
	float16_with_assign( float f );

	float16& operator=(const float16 &other);
	float16& operator=(const float &other);
//	operator unsigned short () const { return m_storage.rawWord; }
	operator float () const;
};

//=========================================================
// Fit a 3D vector in 48 bits
//=========================================================

class Vector48
{
public:
	// Construction/destruction:
	Vector48(void);
	Vector48(vec_t X, vec_t Y, vec_t Z);

	// assignment
	Vector48& operator=(const Vector &vOther);
	operator Vector ();

	const float operator[]( int i ) const;

	float16 x;
	float16 y;
	float16 z;
};

//=========================================================
// Fit a 2D vector in 32 bits
//=========================================================

class Vector2d32
{
public:
	// Construction/destruction:
	Vector2d32(void);
	Vector2d32(vec_t X, vec_t Y);

	// assignment
	Vector2d32& operator=(const Vector &vOther);
	Vector2d32& operator=(const Vector2D &vOther);

	operator Vector2D ();

	void Init( vec_t ix = 0.f, vec_t iy = 0.f);

	float16_with_assign x;
	float16_with_assign y;
};

#endif
