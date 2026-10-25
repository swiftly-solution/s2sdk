#include "public/tier0/murmurhash3.h"

uint32_t MurmurHash3_32( const void *key, size_t len, uint32_t seed, bool bCaselessStringVariant = false )
{
	const uint8_t *data = (const uint8_t *)key;
	const size_t nblocks = len / 4;
	uint32_t uSourceBitwiseAndMask = 0xDFDFDFDF | ((uint32_t)bCaselessStringVariant - 1);

	uint32_t h1 = seed;

	//----------
	// body

	for ( size_t i = 0; i < nblocks; i++ )
	{
		uint32_t k1;
		memcpy( &k1, data + i * 4, sizeof( k1 ) );
		k1 = LittleDWord( k1 );
		k1 &= uSourceBitwiseAndMask;

		k1 *= 0xcc9e2d51;
		k1 = (k1 << 15) | (k1 >> 17);
		k1 *= 0x1b873593;

		h1 ^= k1;
		h1 = (h1 << 13) | (h1 >> 19);
		h1 = h1*5+0xe6546b64;
	}

	//----------
	// tail

	const uint8_t *tail = data + nblocks*4;

	uint32_t k1 = 0;

	switch ( len & 3 )
	{
	case 3: k1 ^= tail[2] << 16; [[fallthrough]];
	case 2: k1 ^= tail[1] << 8; [[fallthrough]];
	case 1: k1 ^= tail[0];
		k1 &= uSourceBitwiseAndMask;
		k1 *= 0xcc9e2d51;
		k1 = (k1 << 15) | (k1 >> 17);
		k1 *= 0x1b873593;
		h1 ^= k1;
	}

	//----------
	// finalization

	h1 ^= (uint32_t)len;

	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;

	return h1;
}

uint32_t MurmurHash3String( const char *pszKey, size_t len )
{
	return MurmurHash3_32( pszKey, len, 1047 /*anything will do for a seed*/, false );
}

uint32_t MurmurHash3StringCaseless( const char *pszKey, size_t len )
{
	return MurmurHash3_32( pszKey, len, 1047 /*anything will do for a seed*/, true );
}

uint32_t MurmurHash3String( const char *pszKey )
{
	return MurmurHash3String( pszKey, strlen( pszKey ) );
}

uint32_t MurmurHash3StringCaseless( const char *pszKey )
{
	return MurmurHash3StringCaseless( pszKey, strlen( pszKey ) );
}

uint32_t MurmurHash3Int( uint32_t h )
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

uint32_t MurmurHash3Int64( uint64_t h )
{
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccdull;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53ull;
	h ^= h >> 33;
	return (uint32_t)h;
}

uint32_t MurmurHash3Item( const uint32_t &item )
{
	return MurmurHash3Int( item );
}

uint32_t MurmurHash3Item( const int32_t &item )
{
	return MurmurHash3Int( item );
}