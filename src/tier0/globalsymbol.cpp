#include "public/tier0/globalsymbol.h"

CGlobalSymbol::CGlobalSymbol()
: m_pString( NULL )
{}

CGlobalSymbol::CGlobalSymbol( const CGlobalSymbol &other )
: m_pString( other.m_pString )
{}

bool CGlobalSymbol::operator==( const CGlobalSymbol &other ) const
{ return m_pString == other.m_pString; }

bool CGlobalSymbol::operator!=( const CGlobalSymbol &other ) const
{ return m_pString != other.m_pString; }

bool CGlobalSymbol::IsValid() const
{ return m_pString != NULL; }

const char * CGlobalSymbol::String() const
{ return m_pString ? m_pString : ""; }

CGlobalSymbol::CGlobalSymbol( const char *pString )
: m_pString( pString )
{}

CGlobalSymbol FindGlobalSymbolByHash( uint32 nHash )
{ return CGlobalSymbol( _FindGlobalSymbolByHash( nHash ) ); }

CGlobalSymbol FindGlobalSymbol( const char *pString )
{ return CGlobalSymbol( _FindGlobalSymbol( pString ) ); }

CGlobalSymbol MakeGlobalSymbol( const char *pString )
{ return CGlobalSymbol( _MakeGlobalSymbol( pString ) ); }

CGlobalSymbolCaseSensitive::CGlobalSymbolCaseSensitive()
: m_pString( NULL )
{}

CGlobalSymbolCaseSensitive::CGlobalSymbolCaseSensitive( const CGlobalSymbolCaseSensitive &other )
: m_pString( other.m_pString )
{}

bool CGlobalSymbolCaseSensitive::operator==( const CGlobalSymbolCaseSensitive &other ) const
{ return m_pString == other.m_pString; }

bool CGlobalSymbolCaseSensitive::operator!=( const CGlobalSymbolCaseSensitive &other ) const
{ return m_pString != other.m_pString; }

bool CGlobalSymbolCaseSensitive::IsValid() const
{ return m_pString != NULL; }

const char * CGlobalSymbolCaseSensitive::String() const
{ return m_pString ? m_pString : ""; }

CGlobalSymbolCaseSensitive::CGlobalSymbolCaseSensitive( const char *pString )
: m_pString( pString )
{}

CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *pString )
{ return CGlobalSymbolCaseSensitive( _MakeGlobalSymbolCaseSensitive( pString ) ); }
