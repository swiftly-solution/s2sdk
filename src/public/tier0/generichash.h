#ifndef GENERICHASH_H
#define GENERICHASH_H

#ifdef _WIN32
#pragma once
#endif

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "platform_base.h"

namespace generichash_detail
{
	template <typename T>
	concept Byte = std::is_integral_v<T> && sizeof(T) == 1;

	template <bool Lower, Byte T>
	constexpr uint8_t Get(const T* data, size_t i)
	{
		uint8_t c = static_cast<uint8_t>(data[i]);
		if constexpr (Lower)
			if (c >= 'A' && c <= 'Z')
				c = static_cast<uint8_t>(c + ('a' - 'A'));
		return c;
	}

	template <bool Lower, Byte T>
	constexpr uint32_t Read32(const T* data, size_t i)
	{
		return uint32_t(Get<Lower>(data, i)) | uint32_t(Get<Lower>(data, i + 1)) << 8 |
			   uint32_t(Get<Lower>(data, i + 2)) << 16 | uint32_t(Get<Lower>(data, i + 3)) << 24;
	}

	constexpr uint32_t Rotl32(uint32_t x, int r) { return (x << r) | (x >> (32 - r)); }

	template <bool Lower, Byte T>
	constexpr uint32_t Hash2(const T* data, size_t len, uint32_t seed)
	{
		constexpr uint32_t m = 0x5bd1e995;
		constexpr int r = 24;

		uint32_t h = seed ^ static_cast<uint32_t>(len);
		size_t i = 0;

		for (; len - i >= 4; i += 4)
		{
			uint32_t k = Read32<Lower>(data, i);
			k *= m;
			k ^= k >> r;
			k *= m;
			h *= m;
			h ^= k;
		}

		switch (len - i)
		{
		case 3: h ^= uint32_t(Get<Lower>(data, i + 2)) << 16; [[fallthrough]];
		case 2: h ^= uint32_t(Get<Lower>(data, i + 1)) << 8; [[fallthrough]];
		case 1: h ^= uint32_t(Get<Lower>(data, i)); h *= m;
		}

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;
		return h;
	}

	template <bool Lower, Byte T>
	constexpr uint32_t Hash3(const T* data, size_t len, uint32_t seed)
	{
		constexpr uint32_t c1 = 0xcc9e2d51;
		constexpr uint32_t c2 = 0x1b873593;

		uint32_t h = seed;
		size_t i = 0;

		for (; len - i >= 4; i += 4)
		{
			uint32_t k = Read32<Lower>(data, i);
			k *= c1;
			k = Rotl32(k, 15);
			k *= c2;
			h ^= k;
			h = Rotl32(h, 13);
			h = h * 5 + 0xe6546b64;
		}

		uint32_t k = 0;
		switch (len - i)
		{
		case 3: k ^= uint32_t(Get<Lower>(data, i + 2)) << 16; [[fallthrough]];
		case 2: k ^= uint32_t(Get<Lower>(data, i + 1)) << 8; [[fallthrough]];
		case 1:
			k ^= uint32_t(Get<Lower>(data, i));
			k *= c1;
			k = Rotl32(k, 15);
			k *= c2;
			h ^= k;
		}

		h ^= static_cast<uint32_t>(len);
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;
		return h;
	}

	template <bool Lower, Byte T>
	constexpr uint64_t Hash64(const T* data, size_t len, uint32_t seed)
	{
		constexpr uint32_t m = 0x5bd1e995;
		constexpr int r = 24;

		uint32_t h1 = seed ^ static_cast<uint32_t>(len);
		uint32_t h2 = 0;
		size_t i = 0;

		for (; len - i >= 8; i += 8)
		{
			uint32_t k1 = Read32<Lower>(data, i);
			k1 *= m; k1 ^= k1 >> r; k1 *= m;
			h1 *= m; h1 ^= k1;

			uint32_t k2 = Read32<Lower>(data, i + 4);
			k2 *= m; k2 ^= k2 >> r; k2 *= m;
			h2 *= m; h2 ^= k2;
		}

		if (len - i >= 4)
		{
			uint32_t k1 = Read32<Lower>(data, i);
			k1 *= m; k1 ^= k1 >> r; k1 *= m;
			h1 *= m; h1 ^= k1;
			i += 4;
		}

		switch (len - i)
		{
		case 3: h2 ^= uint32_t(Get<Lower>(data, i + 2)) << 16; [[fallthrough]];
		case 2: h2 ^= uint32_t(Get<Lower>(data, i + 1)) << 8; [[fallthrough]];
		case 1: h2 ^= uint32_t(Get<Lower>(data, i)); h2 *= m;
		}

		h1 ^= h2 >> 18; h1 *= m;
		h2 ^= h1 >> 22; h2 *= m;
		h1 ^= h2 >> 17; h1 *= m;
		h2 ^= h1 >> 19; h2 *= m;

		return (uint64_t(h1) << 32) | h2;
	}

}

template <generichash_detail::Byte T>
constexpr uint32_t MurmurHash2(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash2<false>(data, len, seed);
}

template <generichash_detail::Byte T>
constexpr uint32_t MurmurHash2CaseInsensitive(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash2<true>(data, len, seed);
}

uint32_t MurmurHash2(const void* data, size_t len, uint32_t seed = 0);
uint32_t MurmurHash2CaseInsensitive(const void* data, size_t len, uint32_t seed = 0);

template <generichash_detail::Byte T>
constexpr uint32_t MurmurHash2LowerCase(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash2<true>(data, len, seed);
}

uint32_t MurmurHash2LowerCase(const void* data, size_t len, uint32_t seed = 0);

template <generichash_detail::Byte T>
constexpr uint32_t MurmurHash3(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash3<false>(data, len, seed);
}

template <generichash_detail::Byte T>
constexpr uint32_t MurmurHash3CaseInsensitive(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash3<true>(data, len, seed);
}

uint32_t MurmurHash3(const void* data, size_t len, uint32_t seed = 0);
uint32_t MurmurHash3CaseInsensitive(const void* data, size_t len, uint32_t seed = 0);

template <generichash_detail::Byte T>
constexpr uint64_t MurmurHash64(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash64<false>(data, len, seed);
}

template <generichash_detail::Byte T>
constexpr uint64_t MurmurHash64CaseInsensitive(const T* data, size_t len, uint32_t seed = 0)
{
	return generichash_detail::Hash64<true>(data, len, seed);
}

uint64_t MurmurHash64(const void* data, size_t len, uint32_t seed = 0);
uint64_t MurmurHash64CaseInsensitive(const void* data, size_t len, uint32_t seed = 0);

constexpr uint32_t HashIntAlternate( uint32_t n)
{
	n = ( n + 0x7ed55d16 ) + ( n << 12 );
	n = ( n ^ 0xc761c23c ) ^ ( n >> 19 );
	n = ( n + 0x165667b1 ) + ( n << 5 );
	n = ( n + 0xd3a2646c ) ^ ( n << 9 );
	n = ( n + 0xfd7046c5 ) + ( n << 3 );
	n = ( n ^ 0xb55a4f09 ) ^ ( n >> 16 );
	return n;
}

constexpr uint32_t HashIntConventional( const int n ) // faster but less effective
{
	// first byte
	uint32_t hash = 0xAAAAAAAA + (n & 0xFF);
	// second byte
	hash = ( hash << 5 ) + hash + ( (n >> 8) & 0xFF );
	// third byte
	hash = ( hash << 5 ) + hash + ( (n >> 16) & 0xFF );
	// fourth byte
	hash = ( hash << 5 ) + hash + ( (n >> 24) & 0xFF );

	return hash;

	/* this is the old version, which would cause a load-hit-store on every
	   line on a PowerPC, and therefore took hundreds of clocks to execute!
	  
	byte *p = (byte *)&n;
	uint32 hash = 0xAAAAAAAA + *p++;
	hash = ( ( hash << 5 ) + hash ) + *p++;
	hash = ( ( hash << 5 ) + hash ) + *p++;
	return ( ( hash << 5 ) + hash ) + *p;
	*/
}

uint32_t HashString( const char *pszKey );
uint32_t HashStringCaseless( const char *pszKey );

#endif
