#include "public/tier0/recttypes.h"

Rect3D_t:: Rect3D_t( int nX, int nY, int nZ, int nWidth, int nHeight, int nDepth )
{
	x = nX;
	y = nY;
	z = nZ;
	width = nWidth;
	height = nHeight;
	depth = nDepth;
}

Rect3D_t:: Rect3D_t( void )
{
}
