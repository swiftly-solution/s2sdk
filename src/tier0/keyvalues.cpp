#include "public/tier0/keyvalues.h"

bool KeyValuesDumpAsDevMsg( KeyValues *pKeyValues, int nIndentLevel, int nDeveloperLevel )
{
    CKeyValuesDumpContextAsDevMsg ctx( nDeveloperLevel );
	return pKeyValues->Dump( &ctx, nIndentLevel );
}

CTemporaryKeyValues::CTemporaryKeyValues()
: m_pKeyValues( nullptr ), m_hScope()
{}

CTemporaryKeyValues::~CTemporaryKeyValues()
{
	KeyValuesSystem()->FreeTemporaryKeyValueAllocationScope( m_hScope );
}

KeyValues::AutoDelete::AutoDelete( KeyValues *pKeyValues )
: m_pKeyValues( pKeyValues )
{}

KeyValues::AutoDelete::AutoDelete( const char *pchKVName )
: m_pKeyValues( new KeyValues( pchKVName ) )
{}

KeyValues::AutoDelete::~AutoDelete( void )
{ if(m_pKeyValues) delete m_pKeyValues; }

void KeyValues::AutoDelete::Assign( KeyValues *pKeyValues )
{ m_pKeyValues = pKeyValues; }

KeyValues * KeyValues::AutoDelete::operator->()
{ return m_pKeyValues; }

KeyValues::AutoDelete::operator KeyValues *()
{ return m_pKeyValues; }

KeyValues::AutoDeleteInline::AutoDeleteInline( KeyValues *pKeyValues )
: AutoDelete( pKeyValues )
{}

KeyValues::AutoDeleteInline::operator KeyValues *() const
{ return m_pKeyValues; }

KeyValues * KeyValues::AutoDeleteInline::Get() const
{ return m_pKeyValues; }

bool KeyValues::GetBool( const char *keyName, bool defaultValue ) const
{ return GetInt( keyName, defaultValue ? 1 : 0 ) ? true : false; }

bool KeyValues::GetBool( HKeySymbol keySymbol, bool defaultValue )
{ return GetInt( keySymbol, defaultValue ? 1 : 0 ) ? true : false; }

void KeyValues::SetBool( const char *keyName, bool value )
{ SetInt( keyName, value ? 1 : 0 ); }

CKeyValuesDumpContextAsDevMsg::CKeyValuesDumpContextAsDevMsg( int nDeveloperLevel )
: m_nDeveloperLevel( nDeveloperLevel )
{}
