#ifndef TIER0_MEMORY_H
#define TIER0_MEMORY_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

// 3d memcpy. Copy (up-to) 3 dimensional data with arbitrary source and destination
// strides. Optimizes to just a single memcpy when possible. For 2d data, set numslices to 1.
PLATFORM_INTERFACE void V_CopyMemory3D(
	void *pDestAdr, void const *pSrcAdr,
	int nNumCols, int nNumRows, int nNumSlices, // dimensions of copy
	int nSrcBytesPerRow, int nSrcBytesPerSlice, // strides for source.
	int nDestBytesPerRow, int nDestBytesPerSlice // strides for dest
);

#ifdef PLATFORM_WINDOWS
// Returns true if p points into the calling thread's stack
PLATFORM_INTERFACE bool V_IsPointerOnStack( const void *p );
#endif

#endif // TIER0_MEMORY_H
