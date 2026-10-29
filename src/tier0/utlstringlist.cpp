#include "public/tier0/utlstringlist.h"

CUtlStringList::CUtlStringList()
{}

CUtlStringList::CUtlStringList( const char *pString, const char *pSeparator )
{
	SplitString( pString, &pSeparator, 1 );
}

CUtlStringList::CUtlStringList( const char *pString, const char * const *pSeparators, int nSeparators )
{
	SplitString( pString, pSeparators, nSeparators );
}

CUtlStringList::~CUtlStringList()
{
	PurgeAndDeleteElements();
}

void CUtlStringList::CopyAndAddToTail( char const *pString )
{
	char *pNewStr = new char[1 + strlen( pString )];
	V_strcpy( pNewStr, pString );
	AddToTail( pNewStr );
}

int __cdecl CUtlStringList::SortFunc( char *const *sz1, char *const *sz2 )
{
	return strcmp( *sz1, *sz2 );
}

void CUtlStringList::PurgeAndDeleteElements()
{
	for(int i = 0; i < m_Size; i++)
	{
		delete[] Element( i );
	}
	Purge();
}
