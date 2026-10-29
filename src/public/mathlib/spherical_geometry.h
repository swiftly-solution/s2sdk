//====== Copyright © 2007-2007, Valve Corporation, All rights reserved. =======//
//
// Purpose: Functions for spherical geometry.
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef SPHERICAL_GEOMETRY_H
#define SPHERICAL_GEOMETRY_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>
#include "public/mathlib/vector.h"

// see http://mathworld.wolfram.com/SphericalTrigonometry.html

// return the spherical distance, in radians, between 2 points on the unit sphere.
float UnitSphereLineSegmentLength( Vector const &a, Vector const &b );


// given 3 points on the unit sphere, return the spherical area (in radians) of the triangle they form.
// valid for "small" triangles.
float UnitSphereTriangleArea( Vector const &a, Vector const &b , Vector const &c );

// spherical harmonics-related functions. Best explanation at http://www.research.scea.com/gdc2003/spherical-harmonic-lighting.pdf

// Evaluate associated legendre polynomial P( l, m ) at flX, using recurrence relation
float AssociatedLegendrePolynomial( int nL, int nM, float flX );

// Evaluate order N spherical harmonic with spherical coordinates
// nL = band, 0..N
// nM = -nL .. nL
// theta = 0..M_PI
// phi = 0.. 2 * M_PHI
float SphericalHarmonic( int nL, int nM, float flTheta, float flPhi );

// evaluate spherical harmonic with normalized vector direction
float SphericalHarmonic( int nL, int nM, Vector const &vecDirection );


#endif // SPHERICAL_GEOMETRY_H
