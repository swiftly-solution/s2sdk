//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef MATH_PFNS_H
#define MATH_PFNS_H

#ifdef _WIN32
#pragma once
#endif

#include <limits>
#include <float.h>
#include "public/tier0/platform.h"

// These globals are initialized by mathlib and redirected based on available fpu features

// The following are not declared as macros because they are often used in limiting situations,
// and sometimes the compiler simply refuses to inline them for some reason
float FastSqrt( float x );

float FastRSqrtFast( float x );
// Single iteration NewtonRaphson reciprocal square root:
// 0.5 * rsqrtps * (3 - x * rsqrtps(x) * rsqrtps(x)) 	
// Very low error, and fine to use in place of 1.f / sqrtf(x).	
float FastRSqrt( float x );

void FastSinCos( float x, float* s, float* c );  // any x
float FastCos( float x );

float FastRecip(float x);
// Simple SSE rsqrt.  Usually accurate to around 6 (relative) decimal places 
// or so, so ok for closed transforms.  (ie, computing lighting normals)
float FastSqrtEst(float x);

// if x is infinite, return FLT_MAX
float FastClampInfinity( float x );

#endif // MATH_PFNS_H
