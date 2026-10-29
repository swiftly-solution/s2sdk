#ifndef AABB_H
#define AABB_H

#ifdef _WIN32
#pragma once
#endif

#include "public/mathlib/vector.h"
#include "public/mathlib/vector4d.h"
#include "public/mathlib/mathlib.h"

//--------------------------------------------------------------------------------------
// AABB
//--------------------------------------------------------------------------------------
struct AABB_t
{
	Vector				m_vMinBounds;
	Vector				m_vMaxBounds;

	Vector GetCenter() const;

	float GetMinDistToPoint( const Vector &vPoint ) const;

	void CreatePlanesFrom( Vector4D *pPlanes ) const;

	// set the aabb to be invalid (max < min )
	void MakeInvalid( void );

};

#endif // AABB_H
