//====== Copyright © 1996-2006, Valve Corporation, All rights reserved. =======//
//
// Purpose: Provide a class (SSE/SIMD only) holding a 2d matrix of class FourVectors,
// for high speed processing in tools.
//
// $NoKeywords: $
//
//=============================================================================//



#include "public/mathlib/simdvectormatrix.h"
#include "public/mathlib/mathlib.h"
#include "public/mathlib/ssemath.h"
#include "public/tier0/platform.h"

void CSIMDVectorMatrix::Init( void )
{
	m_pData = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nPaddedWidth = 0;
}

int CSIMDVectorMatrix::NVectors( void ) const
{
	return m_nHeight * m_nPaddedWidth;
}

CSIMDVectorMatrix::CSIMDVectorMatrix( void )
{
	Init();
}

CSIMDVectorMatrix::~CSIMDVectorMatrix( void )
{
	if ( m_pData )
		delete[] m_pData;
}

void CSIMDVectorMatrix::SetSize( int width, int height )
{
	if ( ( ! m_pData ) || ( width != m_nWidth ) || ( height != m_nHeight ) )
	{
		if ( m_pData )
			delete[] m_pData;

		m_nWidth = width;
		m_nHeight = height;

		m_nPaddedWidth = ( m_nWidth + 3) >> 2;
		m_pData = NULL;
		if ( width && height )
			m_pData = new FourVectors[ m_nPaddedWidth * m_nHeight ];
	}
}

CSIMDVectorMatrix::CSIMDVectorMatrix( int width, int height )
{
	Init();
	SetSize( width, height );
}

CSIMDVectorMatrix &CSIMDVectorMatrix::operator=( CSIMDVectorMatrix const &src )
{
	SetSize( src.m_nWidth, src.m_nHeight );
	if ( m_pData )
		memcpy( (void *)m_pData, src.m_pData, m_nHeight*m_nPaddedWidth*sizeof(m_pData[0]) );
	return *this;
}

Vector CSIMDVectorMatrix::Element(int x, int y) const
{
	Assert( m_pData );
	Assert( x < m_nWidth );
	Assert( y < m_nHeight );
	Vector ret;
	FourVectors const *pData=m_pData+y*m_nPaddedWidth+(x >> 2);

	int xo=(x & 3);
	ret.x=pData->X( xo );
	ret.y=pData->Y( xo );
	ret.z=pData->Z( xo );
	return ret;
}

FourVectors &CSIMDVectorMatrix::CompoundElement(int x, int y)
{
	Assert( m_pData );
	Assert( y < m_nHeight );
	Assert( x < m_nPaddedWidth );
	return m_pData[x + m_nPaddedWidth*y ];
}

void CSIMDVectorMatrix::Clear( void )
{
	Assert( m_pData );
	memset( (void *)m_pData, 0, m_nHeight*m_nPaddedWidth*sizeof(m_pData[0]) );
}

void CSIMDVectorMatrix::CreateFromRGBA_FloatImageData(int srcwidth, int srcheight,
													  float const *srcdata )
{
	Assert( srcwidth && srcheight && srcdata );
	SetSize( srcwidth, srcheight );

	FourVectors *p_write_ptr=m_pData;
	int n_vectors_per_source_line=(srcwidth >> 2);
	int ntrailing_pixels_per_source_line=(srcwidth & 3);
	for(int y=0;y<srcheight;y++)
	{
		float const *data_in=srcdata;
		float *data_out=reinterpret_cast<float *>( p_write_ptr );
		// copy full input blocks
		for(int x=0;x<n_vectors_per_source_line;x++)
		{
			for(int c=0;c<3;c++)
			{
				data_out[0]=data_in[c];					// x0
				data_out[1]=data_in[4+c];				// x1
				data_out[2]=data_in[8+c];				// x2
				data_out[3]=data_in[12+c];				// x3
				data_out+=4;
			}
			data_in += 16;
		}
		// now, copy trailing data and pad with copies
		if (ntrailing_pixels_per_source_line )
		{
			for(int c=0;c<3;c++)
			{
				for(int cp=0;cp<4; cp++)
				{
					int real_cp=MIN( cp, ntrailing_pixels_per_source_line-1 );
					data_out[4*c+cp]= data_in[c+4*real_cp];
				}
			}
		}
		// advance ptrs to next line
		p_write_ptr += m_nPaddedWidth;
		srcdata += 4 * srcwidth;
	}
}

void CSIMDVectorMatrix::RaiseToPower( float power )
{
	int nv=NVectors();
	if ( nv )
	{
		int fixed_point_exp=(int) ( 4.0*power );
		FourVectors *src=m_pData;
		do
		{
			src->x=Pow_FixedPoint_Exponent_SIMD( src->x, fixed_point_exp );
			src->y=Pow_FixedPoint_Exponent_SIMD( src->y, fixed_point_exp );
			src->z=Pow_FixedPoint_Exponent_SIMD( src->z, fixed_point_exp );
			src++;
		} while (--nv);
	}
}

CSIMDVectorMatrix & CSIMDVectorMatrix::operator+=( CSIMDVectorMatrix const &src )
{
	Assert( m_nWidth == src.m_nWidth );
	Assert( m_nHeight == src.m_nHeight );
	int nv=NVectors();
	if ( nv )
	{
		FourVectors *srcv=src.m_pData;
		FourVectors *destv=m_pData;
		do													// !! speed !! inline more iters
		{
			*( destv++ ) += *( srcv++ );
		} while ( --nv );
	}
	return *this;
}

CSIMDVectorMatrix & CSIMDVectorMatrix::operator*=( Vector const &src )
{
	int nv=NVectors();
	if ( nv )
	{
		FourVectors scalevalue;
		scalevalue.DuplicateVector( src );
		FourVectors *destv=m_pData;
		do													// !! speed !! inline more iters
		{
			destv->VProduct( scalevalue );
			destv++;
		} while ( --nv );
	}
	return *this;
}

