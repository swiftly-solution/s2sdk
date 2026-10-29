#include "public/tier0/utlhash.h"

int CUtlHashFastNoHash:: Hash( int key, int bucketMask )
{
	return ( key & bucketMask );
}

int CUtlHashFastGenericHash:: Hash( int key, int bucketMask )
{
	return ( HashIntConventional( key ) & bucketMask );
}

uint32_t CPassthroughHash32:: HashKey32( uint32_t nKey )
{ return nKey; }
