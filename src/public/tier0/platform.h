#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "messages.h"

PLATFORM_INTERFACE const char *		Plat_GetGameDirectory( int unknown=0 );

#if defined _MSC_VER		// MSVC (What about MinGW and Clang for Windows)

#define WordSwap(d) _byteswap_ushort(d)
#define DWordSwap(d) ((uint32_t)(_byteswap_ulong( (unsigned long) d)))
#define QWordSwap(d) _byteswap_uint64(d)

#elif defined __GNUC__		// GCC or Clang

#define WordSwap(d) __builtin_bswap16(d)
#define DWordSwap(d) __builtin_bswap32(d)
#define QWordSwap(d) __builtin_bswap64(d)

#else						// N/A, native code

#pragma message( "TODO: Using non-intrinsic byteswap functions..." )

#define WordSwap WordSwapC
#define DWordSwap DWordSwapC
#define QWordSwap QWordSwapC

#endif

#define SafeSwapFloat( pOut, pIn )	(*((uint32_t*)pOut) = DWordSwap( *((uint32_t*)pIn) ))

#define BigShort( val )				WordSwap( val )
#define BigWord( val )				WordSwap( val )
#define BigLong( val )				DWordSwap( val )
#define BigDWord( val )				DWordSwap( val )
#define LittleShort( val )			( val )
#define LittleWord( val )			( val )
#define LittleLong( val )			( val )
#define LittleDWord( val )			( val )
#define SwapShort( val )			BigShort( val )
#define SwapWord( val )				BigWord( val )
#define SwapLong( val )				BigLong( val )
#define SwapDWord( val )			BigDWord( val )

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		SafeSwapFloat( pOut, pIn )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )
#define SwapFloat( pOut, pIn )		BigFloat( pOut, pIn )

PLATFORM_INTERFACE bool				Plat_IsInTestMode();

PLATFORM_INTERFACE double			Plat_FloatTime();		// Returns time in seconds since the module was loaded.
PLATFORM_INTERFACE uint32_t			Plat_MSTime();			// Time in milliseconds.
PLATFORM_INTERFACE uint64_t			Plat_MSTime64();		// Time in milliseconds.
PLATFORM_INTERFACE uint64_t			Plat_USTime();			// Time in microseconds.
PLATFORM_INTERFACE char *			Plat_ctime64( const time_t *timep, char *buf, size_t bufsize );
PLATFORM_INTERFACE uint64_t			Plat_GetClockStart();	// Snapshot of the clock when app started.
PLATFORM_INTERFACE time_t			Plat_GetTime();			// Same as time().

template <class T>
inline void Destruct( T* pMemory, size_t nAllocatedSize )
{
	pMemory->~T();
}

template <class T>
inline void Destruct( T* pMemory )
{
	Destruct( pMemory, sizeof(T) );
}

#endif