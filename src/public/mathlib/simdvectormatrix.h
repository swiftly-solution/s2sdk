//====== Copyright © 1996-2006, Valve Corporation, All rights reserved. =======//
//
// Purpose: Provide a class (SSE/SIMD only) holding a 2d matrix of class FourVectors,
// for high speed processing in tools.
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef SIMDVECTORMATRIX_H
#define SIMDVECTORMATRIX_H

#ifdef _WIN32
#pragma once
#endif


#include <string.h>
#include "public/tier0/platform.h"
#include "public/mathlib/ssemath.h"

// utlsoacontainer.h is not in this repo
class CSOAContainer;

class CSIMDVectorMatrix
{
public:
	int m_nWidth;											// in actual vectors
	int m_nHeight;

	int m_nPaddedWidth;										// # of 4x wide elements

	FourVectors *m_pData;

protected:
	void Init( void );

	int NVectors( void ) const;

public:
	// constructors and destructors
	CSIMDVectorMatrix( void );

	~CSIMDVectorMatrix( void );

	// set up storage and fields for m x n matrix. destroys old data
	void SetSize( int width, int height );

	CSIMDVectorMatrix( int width, int height );

	CSIMDVectorMatrix &operator=( CSIMDVectorMatrix const &src );

	CSIMDVectorMatrix &operator+=( CSIMDVectorMatrix const &src );

	CSIMDVectorMatrix &operator*=( Vector const &src );

	// create from an RGBA float bitmap. alpha ignored.
	void CreateFromRGBA_FloatImageData(int srcwidth, int srcheight, float const *srcdata );

	// create from 3 fields in a csoa
	void CreateFromCSOAAttributes( CSOAContainer const *pSrc,
								   int nAttrIdx0, int nAttrIdx1, int nAttrIdx2 );

	// Element access. If you are calling this a lot, you don't want to use this class, because
	// you're not getting the sse advantage
	Vector Element(int x, int y) const;

	//addressing the individual fourvectors elements
	FourVectors &CompoundElement(int x, int y);

	// math operations on the whole image
	void Clear( void );

	void RaiseToPower( float power );
};



#endif
