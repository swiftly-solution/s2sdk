#include "public/tier0/utlsymboltable.h"

int CUtlSymbolTable::GetNumStrings( void ) const
{
	return m_MemBlocks.Count();
}

CUtlSymbolTableMT::CUtlSymbolTableMT( int growSize, int initSize, bool caseInsensitive )
: CUtlSymbolTable( growSize, initSize, caseInsensitive )
{
}

CUtlSymbol CUtlSymbolTableMT::AddString( const char* pString, bool* created )
{
	m_lock.LockForWrite( __FILE__, __LINE__ );
	CUtlSymbol result = CUtlSymbolTable::AddString( pString, created );
	m_lock.UnlockWrite( __FILE__, __LINE__ );
	return result;
}

CUtlSymbol CUtlSymbolTableMT::AddString( const char* pString, int nLength, bool* created )
{
	m_lock.LockForWrite( __FILE__, __LINE__ );
	CUtlSymbol result = CUtlSymbolTable::AddString( pString, nLength, created );
	m_lock.UnlockWrite( __FILE__, __LINE__ );
	return result;
}

CUtlSymbol CUtlSymbolTableMT::Find( const char* pString ) const
{
	m_lock.LockForWrite( __FILE__, __LINE__ );
	CUtlSymbol result = CUtlSymbolTable::Find( pString );
	m_lock.UnlockWrite( __FILE__, __LINE__ );
	return result;
}

CUtlSymbol CUtlSymbolTableMT::Find( const char* pString, int nLength ) const
{
	m_lock.LockForWrite( __FILE__, __LINE__ );
	CUtlSymbol result = CUtlSymbolTable::Find( pString, nLength );
	m_lock.UnlockWrite( __FILE__, __LINE__ );
	return result;
}

const char* CUtlSymbolTableMT::String( CUtlSymbol id ) const
{
	m_lock.LockForRead( __FILE__, __LINE__ );
	const char *pszResult = CUtlSymbolTable::String( id );
	m_lock.UnlockRead( __FILE__, __LINE__ );
	return pszResult;
}
