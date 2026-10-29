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
    CUtlStringToken(uint32_t hashCode = 0);

    template<size_t N>
    constexpr CUtlStringToken(const char (&str)[N]) : m_nHashCode(str[0] ? MurmurHash2LowerCase(str, N - 1, STRINGTOKEN_MURMURHASH_SEED) : 0) {}

    CUtlStringToken(const char* str);

    bool operator==( CUtlStringToken const &other ) const;
	bool operator!=( CUtlStringToken const &other ) const;
	bool operator<( CUtlStringToken const &other ) const;
	bool IsValid() const;
	uint32_t GetHashCode() const;
	void SetHashCode( uint32_t hash );

	DLL_CLASS_IMPORT static CUtlStringToken Make( const char *str );

private:
    uint32_t m_nHashCode;
};

#endif