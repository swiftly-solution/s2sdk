#include "public/tier0/keyvalues.h"

bool KeyValuesDumpAsDevMsg( KeyValues *pKeyValues, int nIndentLevel = 0, int nDeveloperLevel = 1 )
{
    CKeyValuesDumpContextAsDevMsg ctx( nDeveloperLevel );
	return pKeyValues->Dump( &ctx, nIndentLevel );
}