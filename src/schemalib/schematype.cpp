#include "public/schemalib/schematype.h"

uint32_t CSchemaType_Hash( const char *pString, int len )
{
	return MurmurHash2( pString, len, 0xBAADFEED );
}