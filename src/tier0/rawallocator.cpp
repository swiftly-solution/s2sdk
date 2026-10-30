#include "public/tier0/rawallocator.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "public/tier0/memdbgon.h"

void CMemAllocAllocator:: Free( void* pMem )
{
	if(pMem)
		free( pMem );
}
