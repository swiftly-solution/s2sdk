#ifndef VECTORALIGNED_H
#define VECTORALIGNED_H
#ifdef _WIN32
#pragma once
#endif


#include "public/mathlib/vector.h"
#include "public/tier0/memalloc.h"

//-----------------------------------------------------------------------------
// Here's where we add all those lovely SSE optimized routines
//-----------------------------------------------------------------------------

class ALIGN16 VectorAligned : public Vector
{
public:
	VectorAligned(void);
	VectorAligned(vec_t X, vec_t Y, vec_t Z);

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	VectorAligned(const VectorAligned& vOther);
	VectorAligned(const Vector &vOther);

#else
public:
	explicit VectorAligned(const Vector &vOther);
	
	VectorAligned& operator=(const Vector &vOther);

	VectorAligned& operator=(const VectorAligned &vOther);

	
#endif
	float w;	// this space is used anyway

#if !defined(NO_MALLOC_OVERRIDE)
	void* operator new[] ( size_t nSize);

	void operator delete[] ( void* p);

	// please don't allocate a single quaternion...
	void* operator new   ( size_t nSize );
	void operator delete ( void* p);
#endif
} ALIGN16_POST;

#endif // VECTORALIGNED_H
