//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef _3D_UNITVEC_H
#define _3D_UNITVEC_H
#ifdef _WIN32
#pragma once
#endif

#include "public/mathlib/vector.h"


#define UNITVEC_DECLARE_STATICS \
   float cUnitVector::mUVAdjustment[0x2000]; \
   Vector cUnitVector::mTmpVec;

// upper 3 bits
#define SIGN_MASK  0xe000
#define XSIGN_MASK 0x8000
#define YSIGN_MASK 0x4000
#define ZSIGN_MASK 0x2000

// middle 6 bits - xbits
#define TOP_MASK  0x1f80

// lower 7 bits - ybits
#define BOTTOM_MASK  0x007f

// unitcomp.cpp : A Unit Vector to 16-bit word conversion
// algorithm based on work of Rafael Baptista (rafael@oroboro.com)
// Accuracy improved by O.D. (punkfloyd@rocketmail.com)
// Used with Permission.

// a compressed unit vector. reasonable fidelty for unit
// vectors in a 16 bit package. Good enough for surface normals
// we hope.
class cUnitVector // : public c3dMathObject
{
public:
   cUnitVector();
   cUnitVector( const Vector& vec );
   cUnitVector( unsigned short val );

   cUnitVector& operator=( const Vector& vec );

   operator Vector();

   void packVector( const Vector& vec );

   void unpackVector( Vector& vec );

   static void initializeStatics();

#if 0
   void test();

   friend ostream& operator<< ( ostream& os, const cUnitVector& vec );
#endif

//protected: // !!!!

   unsigned short mVec;
   static float mUVAdjustment[0x2000];
   static Vector mTmpVec;
};

#endif // _3D_VECTOR_H
