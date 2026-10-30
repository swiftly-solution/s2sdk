#include "public/entity2/variant.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "public/tier0/memdbgon.h"

void * CVariantDefaultAllocator::Allocate( uint32_t nSize )
{
	return malloc( nSize );
}

void CVariantDefaultAllocator:: Free( void *pMemory )
{
	free( pMemory );
}
