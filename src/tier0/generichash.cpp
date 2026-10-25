#include "public/tier0/generichash.h"

using namespace generichash_detail;

static const unsigned char* Bytes(const void* p) { return static_cast<const unsigned char*>(p); }

uint32_t MurmurHash2(const void* data, size_t len, uint32_t seed)
{
	return Hash2<false>(Bytes(data), len, seed);
}

uint32_t MurmurHash2CaseInsensitive(const void* data, size_t len, uint32_t seed)
{
	return Hash2<true>(Bytes(data), len, seed);
}

uint32_t MurmurHash2LowerCase(const void* data, size_t len, uint32_t seed)
{
	return Hash2<true>(Bytes(data), len, seed);
}

uint32_t MurmurHash3(const void* data, size_t len, uint32_t seed)
{
	return Hash3<false>(Bytes(data), len, seed);
}

uint32_t MurmurHash3CaseInsensitive(const void* data, size_t len, uint32_t seed)
{
	return Hash3<true>(Bytes(data), len, seed);
}

uint64_t MurmurHash64(const void* data, size_t len, uint32_t seed)
{
	return Hash64<false>(Bytes(data), len, seed);
}

uint64_t MurmurHash64CaseInsensitive(const void* data, size_t len, uint32_t seed)
{
	return Hash64<true>(Bytes(data), len, seed);
}
