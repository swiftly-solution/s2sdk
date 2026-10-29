#include "public/tier0/utlvectorultraconservative.h"

void * CUtlVectorUltraConservativeAllocator::Alloc( size_t nSize )
{
	return malloc( nSize );
}

void * CUtlVectorUltraConservativeAllocator::Realloc( void *pMem, size_t nSize )
{
	return realloc( pMem, nSize );
}

void CUtlVectorUltraConservativeAllocator:: Free( void *pMem )
{
	free( pMem );
}

size_t CUtlVectorUltraConservativeAllocator:: GetSize( void *pMem )
{
	return mallocsize( pMem );
}
