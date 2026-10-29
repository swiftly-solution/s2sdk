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
	CUtlSymbolLarge();

	CUtlSymbolLarge( const char* pString );

	bool operator==( CUtlSymbolLarge const& src ) const;

	bool operator!=( CUtlSymbolLarge const& src ) const;
	
	bool operator<( CUtlSymbolLarge const& src ) const;

	const char* String() const;

	bool IsValid() const;

private:
	bool operator==( const char* pString ) const;

	const char* m_pString;
};

uint32_t CUtlSymbolLarge_Hash( bool CASEINSENSITIVE, const char *pString, int len );

#endif