//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef UTLSTRINGTOKEN_H
#define UTLSTRINGTOKEN_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "generichash.h"

#define STRINGTOKEN_MURMURHASH_SEED 0x31415926

class CUtlStringToken
{
public:
    CUtlStringToken(uint32_t hashCode = 0): m_nHashCode(hashCode) {}

    template<size_t N>
    constexpr CUtlStringToken(const char (&str)[N]) : m_nHashCode(str[0] ? MurmurHash2LowerCase(str, N - 1, STRINGTOKEN_MURMURHASH_SEED) : 0) {}

    bool operator==( CUtlStringToken const &other ) const { return ( other.m_nHashCode == m_nHashCode ); }
	bool operator!=( CUtlStringToken const &other ) const { return !operator==( other ); }
	bool operator<( CUtlStringToken const &other ) const { return ( m_nHashCode < other.m_nHashCode ); }
	bool IsValid() const { return m_nHashCode != 0; }
	uint32_t GetHashCode() const { return m_nHashCode; }
	void SetHashCode( uint32_t hash ) { m_nHashCode = hash; }

	DLL_CLASS_IMPORT static CUtlStringToken Make( const char *str );

private:
    uint32_t m_nHashCode;
};

#endif