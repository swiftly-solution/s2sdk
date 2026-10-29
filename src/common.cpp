#include "public/common.h"

uint32_t& FloatBits( float& f )
{
	return *reinterpret_cast<uint32_t*>((char*)(&f));
}

uint32_t const FloatBits( const float &f )
{
	union Convertor_t
	{
		float f;
		uint32_t ul;
	}tmp;
	tmp.f = f;
	return tmp.ul;
}

float BitsToFloat( uint32_t i )
{
	union Convertor_t
	{
		float f;
		uint32_t ul;
	}tmp;
	tmp.ul = i;
	return tmp.f;
}

bool IsFinite( const float &f )
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

uint32_t FloatAbsBits( float f )
{
	return FloatBits(f) & 0x7FFFFFFF;
}

float FloatMakeNegative( float f )
{
	return BitsToFloat( FloatBits(f) | 0x80000000 );
}

float FloatMakePositive( float f )
{
	return BitsToFloat( FloatBits(f) & 0x7FFFFFFF );
}

float fpmin( float a, float b )
{
	return a < b ? a : b;
}

float fpmax( float a, float b )
{
	return a > b ? a : b;
}
