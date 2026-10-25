#ifndef UTLSYMBOLLARGE_H
#define UTLSYMBOLLARGE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "generichash.h"

typedef unsigned int UtlSymLargeId_t;

#define UTL_SYMBOL_LARGE_MURMURHASH     0x31415926
#define UTL_INVAL_SYMBOL_LARGE          ((UtlSymLargeId_t)~0)

class CUtlSymbolLarge
{
public:
	CUtlSymbolLarge() 
	{
		m_pString = nullptr;
	}

	CUtlSymbolLarge( const char* pString )
	{
		m_pString = pString;
	}

	bool operator==( CUtlSymbolLarge const& src ) const 
	{ 
		return m_pString == src.m_pString; 
	}

	bool operator!=( CUtlSymbolLarge const& src ) const 
	{ 
		return m_pString != src.m_pString; 
	}
	
	bool operator<( CUtlSymbolLarge const& src ) const
	{
		return ( uintptr_t )m_pString < ( uintptr_t )src.m_pString;
	}

	const char* String() const 
	{ 
		if ( !m_pString )
			return "";
		return m_pString; 
	}

	bool IsValid() const
	{
		return m_pString != nullptr;
	}

private:
	bool operator==( const char* pString ) const;

	const char* m_pString;
};

uint32_t CUtlSymbolLarge_Hash( bool CASEINSENSITIVE, const char *pString, int len );

#endif