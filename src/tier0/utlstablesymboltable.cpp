#include "public/tier0/utlstablesymboltable.h"

CStableSymbolTable::CStableSymbolTable()
: m_stringStorage( 8 )
{}

const char * CStableSymbolTable::GetSymbol( int nId ) const
{ return m_IdToSymbol.IsValidIndex( nId ) ? m_IdToSymbol[ nId ] : nullptr; }

int CStableSymbolTable::Count() const
{ return m_IdToSymbol.Count(); }

uint32 CStableSymbolTable::Hash_t::operator()( const char *pString ) const
{ return MurmurHash2LowerCase( pString, STRINGTOKEN_MURMURHASH_SEED ); }

bool CStableSymbolTable::Eq_t::operator()( const char *pString1, const char *pString2 ) const
{ return V_stricmp_fast( pString1, pString2 ) == 0; }
