#include "public/mathlib/mathlib.h"

uint32_t SmallestPowerOfTwoGreaterOrEqual( uint32_t x )
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}