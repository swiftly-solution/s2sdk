#ifndef RECTTYPES_H
#define RECTTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

//-----------------------------------------------------------------------------
// MaterialRect_t struct - used for DrawDebugText
//-----------------------------------------------------------------------------
struct Rect_t
{
	int x, y;
	int width, height;
};

struct Rect3D_t
{
	int x, y, z;
	int width, height, depth;

	Rect3D_t( int nX, int nY, int nZ, int nWidth, int nHeight, int nDepth );

	Rect3D_t( void );

};

#endif // RECTTYPES_H
