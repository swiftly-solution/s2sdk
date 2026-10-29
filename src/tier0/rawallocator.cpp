#include "public/tier0/rawallocator.h"

void CMemAllocAllocator:: Free( void* pMem )
{
	if(pMem)
		free( pMem );
}
