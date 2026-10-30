#include "public/common.h"
#include "public/tier0/basetypes.h"

float fpmin( float a, float b )
{
	return a < b ? a : b;
}

float fpmax( float a, float b )
{
	return a > b ? a : b;
}
