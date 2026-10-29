#include "public/entity2/datamap.h"

CDatadescGeneratedNameHolder::CDatadescGeneratedNameHolder( const char *pszBase )
: m_pszBase(pszBase)
{
	m_nLenBase = strlen( m_pszBase );
}

CDatadescGeneratedNameHolder::~CDatadescGeneratedNameHolder()
{
	for ( int i = 0; i < m_Names.Count(); i++ )
	{
		delete m_Names[i];
	}
}

const char * CDatadescGeneratedNameHolder::GenerateName( const char *pszIdentifier )
{
	char *pBuf = new char[m_nLenBase + strlen(pszIdentifier) + 1];
	strcpy( pBuf, m_pszBase );
	strcat( pBuf, pszIdentifier );
	m_Names.AddToTail( pBuf );
	return pBuf;
}
