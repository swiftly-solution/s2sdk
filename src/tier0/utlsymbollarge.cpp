#include "public/tier0/utlsymbollarge.h"

uint32_t CUtlSymbolLarge_Hash( bool CASEINSENSITIVE, const char *pString, int len )
{
	return ( CASEINSENSITIVE ? MurmurHash2LowerCase( pString, len, UTL_SYMBOL_LARGE_MURMURHASH ) : MurmurHash2( pString, len, UTL_SYMBOL_LARGE_MURMURHASH ) ); 
}

CUtlSymbolLarge::CUtlSymbolLarge()
{
	m_pString = nullptr;
}

CUtlSymbolLarge::CUtlSymbolLarge( const char* pString )
{
	m_pString = pString;
}

bool CUtlSymbolLarge::operator==( CUtlSymbolLarge const& src ) const
{ 
	return m_pString == src.m_pString; 
}

bool CUtlSymbolLarge::operator!=( CUtlSymbolLarge const& src ) const
{ 
	return m_pString != src.m_pString; 
}

bool CUtlSymbolLarge::operator<( CUtlSymbolLarge const& src ) const
{
	return ( uintptr_t )m_pString < ( uintptr_t )src.m_pString;
}

const char* CUtlSymbolLarge::String() const
{ 
	if ( !m_pString )
		return "";
	return m_pString; 
}

bool CUtlSymbolLarge::IsValid() const
{
	return m_pString != nullptr;
}
