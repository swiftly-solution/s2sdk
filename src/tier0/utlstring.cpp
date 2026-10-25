#include "public/tier0/utlstring.h"

CUtlString &CUtlString::operator=( const CUtlString &src )
{
	SetDirect( src.Get(), src.Length() );
	return *this;
}

CUtlString &CUtlString::operator=( const char *src )
{
	Set( src );
	return *this;
}

bool operator==( const char *pString, const CUtlString &utlString )
{
	return utlString.IsEqual_CaseSensitive( pString );
}

bool operator!=( const char *pString, const CUtlString &utlString )
{
	return !utlString.IsEqual_CaseSensitive( pString );
}

bool operator==( const CUtlString &utlString, const char *pString )
{
	return utlString.IsEqual_CaseSensitive( pString );
}

bool operator!=( const CUtlString &utlString, const char *pString )
{
	return !utlString.IsEqual_CaseSensitive( pString );
}

CUtlString::CUtlString()
	: m_pString( NULL )
{
}

CUtlString::CUtlString( const char *pString )
	: m_pString( NULL )
{
	Set( pString );
}

CUtlString::CUtlString( const char *pString, int length )
	: m_pString( NULL )
{
	SetDirect( pString, length );
}

CUtlString::CUtlString( const CUtlString &string )
	: m_pString( NULL )
{
	Set( string.Get() );
}

CUtlString::~CUtlString()
{
	Purge();
}

const char *CUtlString::Get() const
{
	if(!m_pString)
	{
		return "";
	}
	return m_pString;
}

int CUtlString::Length() const
{
	if(m_pString)
	{
		return V_strlen( m_pString );
	}
	return 0;
}

bool CUtlString::IsEmpty() const
{
	return !m_pString || m_pString[0] == 0;
}

int __cdecl CUtlString::SortCaseInsensitive( const CUtlString *pString1, const CUtlString *pString2 )
{
	return V_stricmp( pString1->String(), pString2->String() );
}

int __cdecl CUtlString::SortCaseSensitive( const CUtlString *pString1, const CUtlString *pString2 )
{
	return V_strcmp( pString1->String(), pString2->String() );
}