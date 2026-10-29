#include "public/mathlib/quaternionaligned.h"

QuaternionAligned::QuaternionAligned(void)
{};

QuaternionAligned::QuaternionAligned(vec_t X, vec_t Y, vec_t Z, vec_t W)
{
	Init(X,Y,Z,W);
}

QuaternionAligned::operator Quaternion * ()
{ return this; } 

QuaternionAligned::operator const Quaternion * ()
{ return this; } 

#ifdef VECTOR_NO_SLOW_OPERATIONS
#else
QuaternionAligned::QuaternionAligned(const Quaternion &vOther)
{
	Init(vOther.x, vOther.y, vOther.z, vOther.w);
}

QuaternionAligned& QuaternionAligned::operator=(const Quaternion &vOther)
{
	Init(vOther.x, vOther.y, vOther.z, vOther.w);
	return *this;
}

QuaternionAligned& QuaternionAligned::operator=(const QuaternionAligned &vOther)
{
	// we know we're aligned, so use simd
	// we can't use the convenient abstract interface coz it gets declared later
#if _WIN32
	_mm_store_ps(Base(), _mm_load_ps( vOther.Base() ));
#else
	Init(vOther.x, vOther.y, vOther.z, vOther.w);
#endif
	return *this;
}

#endif
#if !defined(NO_MALLOC_OVERRIDE)
void* QuaternionAligned::operator new[] ( size_t nSize)
{
	return MemAlloc_AllocAligned(nSize, 16);
}

void QuaternionAligned::operator delete[] ( void* p)
{
	MemAlloc_FreeAligned(p);
}

void* QuaternionAligned::operator new   ( size_t nSize )
{
	return MemAlloc_AllocAligned(nSize, 16);
}

void QuaternionAligned::operator delete ( void* p)
{
	MemAlloc_FreeAligned(p);
}

#endif
