#ifndef QUATERNIONALIGNED_H
#define QUATERNIONALIGNED_H
#ifdef _WIN32
#pragma once
#endif


#include "public/mathlib/vector.h"
#include "public/tier0/memalloc.h"

//-----------------------------------------------------------------------------
// Here's where we add all those lovely SSE optimized routines
//-----------------------------------------------------------------------------
class ALIGN16 QuaternionAligned : public Quaternion
{
public:
	QuaternionAligned(void);
	QuaternionAligned(vec_t X, vec_t Y, vec_t Z, vec_t W);

	operator Quaternion * ();
	operator const Quaternion * ();

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	QuaternionAligned(const QuaternionAligned& vOther);
	QuaternionAligned(const Quaternion &vOther);

#else
public:
	explicit QuaternionAligned(const Quaternion &vOther);

	QuaternionAligned& operator=(const Quaternion &vOther);

	QuaternionAligned& operator=(const QuaternionAligned &vOther);

#endif

#if !defined(NO_MALLOC_OVERRIDE)
	void* operator new[] ( size_t nSize);

	void operator delete[] ( void* p);

	// please don't allocate a single quaternion...
	void* operator new   ( size_t nSize );
	void operator delete ( void* p);
#endif
} ALIGN16_POST;

#endif // QUATERNIONALIGNED_H
