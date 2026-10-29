#include "public/tier0/splitstring.h"

CSplitString::CSplitString( const char *pString, const char *pSeparator, bool include_empty )
: m_szBuffer( nullptr ), m_nBufferSize( 0 )
{
	Split( pString, -1, &pSeparator, 1, include_empty );
}

CSplitString::CSplitString( const char *pString, const char **pSeparators, int nSeparators, bool include_empty )
: m_szBuffer( nullptr ), m_nBufferSize( 0 )
{
	Split( pString, -1, pSeparators, nSeparators, include_empty );
}

CSplitString::~CSplitString()
{
	if(m_szBuffer)
		delete[] m_szBuffer;
}

void CSplitString::PurgeAndDeleteElements()
{
	Purge();
}
