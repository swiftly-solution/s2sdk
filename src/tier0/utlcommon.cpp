#include "public/tier0/utlcommon.h"

bool StringLessFunctor::operator()( const char *a, const char *b ) const
{ return Q_strcmp( a, b ) < 0; }

bool StringEqualFunctor::operator()( const char *a, const char *b ) const
{ return Q_strcmp( a, b ) == 0; }

bool CaselessStringLessFunctor::operator()( const char *a, const char *b ) const
{ return Q_strcasecmp( a, b ) < 0; }

bool CaselessStringEqualFunctor::operator()( const char *a, const char *b ) const
{ return Q_strcasecmp( a, b ) == 0; }

unsigned int MurmurHash3Mix32HashFunctor::operator()( uint32_t n ) const
{ return MurmurHash3Int( n ); }

unsigned int MurmurHash3Mix64HashFunctor::operator()( uint64 n ) const
{ return MurmurHash3Int64( n ); }

bool PointerLessFunctor::operator()( const void *a, const void *b ) const
{ return a < b; }

bool PointerEqualFunctor::operator()( const void *a, const void *b ) const
{ return a == b; }

unsigned int PointerHashFunctor::operator()( const void* s ) const
{ return MurmurHash3Mix64HashFunctor()( (uint64)(uintptr_t)s ); }

unsigned int StringHashFunctor::operator()( const char* s ) const
{
	uint32_t h = 2166136261u;
	for ( ; *s; ++s )
	{
		uint32_t c = (unsigned char) *s;
		h = (h ^ c) * 16777619;
	}
	return (h ^ (h << 17)) + (h >> 21);
}

unsigned int CaselessStringHashFunctor::operator()( const char* s ) const
{
	uint32_t h = 2166136261u;
	for ( ; *s; ++s )
	{
		uint32_t c = (unsigned char) *s;
		// Brutally fast branchless ASCII tolower():
		// if ((c >= 'A') && (c <= 'Z')) c += ('a' - 'A');
		c += (((('A'-1) - c) & (c - ('Z'+1))) >> 26) & 32;
		h = (h ^ c) * 16777619;
	}
	return (h ^ (h << 17)) + (h >> 21);
}
