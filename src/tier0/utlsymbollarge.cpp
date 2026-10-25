#include "public/tier0/utlsymbollarge.h"

uint32_t CUtlSymbolLarge_Hash( bool CASEINSENSITIVE, const char *pString, int len )
{
	return ( CASEINSENSITIVE ? MurmurHash2LowerCase( pString, len, UTL_SYMBOL_LARGE_MURMURHASH ) : MurmurHash2( pString, len, UTL_SYMBOL_LARGE_MURMURHASH ) ); 
}