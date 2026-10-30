#include "public/tier0/platform.h"

uint32_t LoadLittleDWord( const uint32_t *base, unsigned int dwordIndex )
{
    return LittleDWord( base[dwordIndex] );
}

void StoreLittleDWord( uint32_t *base, unsigned int dwordIndex, uint32_t dword )
{
    base[dwordIndex] = LittleDWord(dword);
}

uint64 CastPtrToUint64( const void* p )
{
	return (uint64) ( (uintptr_t) p );
}

int64 CastPtrToInt64( const void* p )
{
	return (int64) ( (uintptr_t) p );
}