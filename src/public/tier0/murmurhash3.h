#ifndef MURMURHASH3_H
#define MURMURHASH3_H

#if defined(_WIN32)
#pragma once
#endif

#include <string.h>
#include "platform.h"

uint32_t MurmurHash3_32( const void *key, size_t len, uint32_t seed, bool bCaselessStringVariant = false );
uint32_t MurmurHash3String( const char *pszKey, size_t len );
uint32_t MurmurHash3StringCaseless( const char *pszKey, size_t len );
uint32_t MurmurHash3String( const char *pszKey );
uint32_t MurmurHash3StringCaseless( const char *pszKey );

template <typename T>
inline uint32_t MurmurHash3Item( const T &item )
{
	return MurmurHash3_32( &item, sizeof(item), 1047 );
}

uint32_t MurmurHash3Int( uint32_t h );
uint32_t MurmurHash3Int64( uint64_t h );
uint32_t MurmurHash3Item( const uint32_t &item );
uint32_t MurmurHash3Item( const int32_t &item );

template<typename T>
struct MurmurHash3Functor
{
	typedef uint32_t TargetType;
	TargetType operator()(const T &key) const
	{
		return MurmurHash3Item( key );
	}
};

template<>
struct MurmurHash3Functor<char *>
{
	typedef uint32_t TargetType;
	TargetType operator()(const char *key) const
	{
		return MurmurHash3String( key );
	}
};

template<>
struct MurmurHash3Functor<const char *>
{
	typedef uint32_t TargetType;
	TargetType operator()(const char *key) const
	{
		return MurmurHash3String( key );
	}
};

#endif