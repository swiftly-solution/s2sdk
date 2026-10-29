#include "public/tier0/basetypes.h"

float  fpmin( float a, float b )
{
	return ( a < b ) ? a : b;
}

float  fpmax( float a, float b )
{
	return ( a > b ) ? a : b;
}

unsigned * color32_s::asInt(void)
{ return reinterpret_cast<unsigned*>(this); }

const unsigned * color32_s::asInt(void) const
{ return reinterpret_cast<const unsigned*>(this); }

void color32_s:: Copy(const color32_s &rhs)
{
	*asInt() = *rhs.asInt();
}

bool color32::operator!=( const color32 &other ) const
{
	return r != other.r || g != other.g || b != other.b || a != other.a;
}
