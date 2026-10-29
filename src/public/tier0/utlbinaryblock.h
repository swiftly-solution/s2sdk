//====== Copyright © 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef UTLBINARYBLOCK_H
#define UTLBINARYBLOCK_H
#ifdef _WIN32
#pragma once
#endif

#include "utlleanvector.h"
#include "strtools.h"
#include "limits.h"

//-----------------------------------------------------------------------------
// Base class, containing simple memory management
//-----------------------------------------------------------------------------
class CUtlBinaryBlock
{
public:
	CUtlBinaryBlock( int growSize = 0, int initSize = 0 );

	// NOTE: nInitialLength indicates how much of the buffer starts full
	CUtlBinaryBlock( void* pMemory, int nSizeInBytes, int nInitialLength );
	CUtlBinaryBlock( const CUtlBinaryBlock& src );

	void		Get( void *pValue, int nMaxLen ) const;
	void		Set( const void *pValue, int nLen );
	const void	*Get( ) const;
	void		*Get( );

	unsigned char& operator[]( int i );
	const unsigned char& operator[]( int i ) const;

	int			Length() const;
	void		SetLength( int nLength );	// Undefined memory will result
	bool		IsEmpty() const;
	void		Clear();
	void		Purge();

	CUtlBinaryBlock &operator=( const CUtlBinaryBlock &src );
	CUtlBinaryBlock &operator=( CUtlBinaryBlock &&src );

	// Test for equality
	bool operator==( const CUtlBinaryBlock &src ) const;

private:
	CUtlLeanVector<uint8, int> m_Memory;
};


//-----------------------------------------------------------------------------
// class inlines
//-----------------------------------------------------------------------------








#endif // UTLBINARYBLOCK_H

