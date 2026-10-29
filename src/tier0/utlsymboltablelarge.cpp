#include "public/tier0/utlsymboltablelarge.h"

bool CUtlSymbolTableLargeBaseTreeEntry_t:: IsEmpty() const
{
	return ( ( m_Hash == 0 ) && ( 0 == m_String[0] ) );
}

char const * CUtlSymbolTableLargeBaseTreeEntry_t::String() const
{
	return (const char *)&m_String[ 0 ];
}

CUtlSymbolLarge CUtlSymbolTableLargeBaseTreeEntry_t:: ToSymbol() const
{
	return CUtlSymbolLarge( String() );
}

LargeSymbolTableHashDecoration_t CUtlSymbolTableLargeBaseTreeEntry_t:: HashValue() const
{
	return m_Hash;
}
