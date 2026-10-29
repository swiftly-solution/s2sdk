#include "public/entity2/variant.h"

void * CVariantDefaultAllocator::Allocate( uint32_t nSize )
{
	return malloc( nSize );
}

void CVariantDefaultAllocator:: Free( void *pMemory )
{
	free( pMemory );
}
