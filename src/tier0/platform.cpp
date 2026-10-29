#include "public/tier0/platform.h"

uint32_t LoadLittleDWord( const uint32_t *base, unsigned int dwordIndex )
{
    return LittleDWord( base[dwordIndex] );
}

void StoreLittleDWord( uint32_t *base, unsigned int dwordIndex, uint32_t dword )
{
    base[dwordIndex] = LittleDWord(dword);
}

uint64_t CastPtrToUint64( const void* p )
{
	return (uint64_t) ( (uintptr_t) p );
}

int64_t CastPtrToInt64( const void* p )
{
	return (int64_t) ( (uintptr_t) p );
}