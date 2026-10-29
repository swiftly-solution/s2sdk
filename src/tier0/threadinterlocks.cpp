#include "public/tier0/threadinterlocks.h"

void  ThreadInterlockedExchangePointerToConst( void const * volatile *p, void const *value )
{
	ThreadInterlockedExchangePointer( (void * volatile *)p, (void *)value );
}

bool  ThreadInterlockedAssignPointerToConstIf( void const * volatile *p, void const *value, void const *comparand )
{
	return ThreadInterlockedCompareExchangePointer( (void * volatile *)p, (void *)value, (void *)comparand ) == comparand;
}
