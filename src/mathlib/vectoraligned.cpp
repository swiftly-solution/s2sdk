#include "public/mathlib/vectoraligned.h"

VectorAligned::VectorAligned(void)
{};

VectorAligned::VectorAligned(vec_t X, vec_t Y, vec_t Z)
{
	Init(X,Y,Z);
}

#ifdef VECTOR_NO_SLOW_OPERATIONS
#else
VectorAligned::VectorAligned(const Vector &vOther)
{
	Init(vOther.x, vOther.y, vOther.z);
}

VectorAligned& VectorAligned::operator=(const Vector &vOther)
{
	Init(vOther.x, vOther.y, vOther.z);
	return *this;
}

VectorAligned& VectorAligned::operator=(const VectorAligned &vOther)
{
	// we know we're aligned, so use simd
	// we can't use the convenient abstract interface coz it gets declared later
#if _WIN32
	_mm_store_ps(Base(), _mm_load_ps( vOther.Base() ));
#else
	Init(vOther.x, vOther.y, vOther.z);
#endif
	return *this;
}

#endif
#if !defined(NO_MALLOC_OVERRIDE)
void* VectorAligned::operator new[] ( size_t nSize)
{
	return MemAlloc_AllocAligned(nSize, 16);
}

void VectorAligned::operator delete[] ( void* p)
{
	MemAlloc_FreeAligned(p);
}

void* VectorAligned::operator new   ( size_t nSize )
{
	return MemAlloc_AllocAligned(nSize, 16);
}

void VectorAligned::operator delete ( void* p)
{
	MemAlloc_FreeAligned(p);
}

#endif
