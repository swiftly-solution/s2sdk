#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include <ctime>
#include "messages.h"

#include <utility>

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

uint32_t LoadLittleDWord( const uint32_t *base, unsigned int dwordIndex );
void StoreLittleDWord( uint32_t *base, unsigned int dwordIndex, uint32_t dword );
uint64 CastPtrToUint64( const void* p );
int64 CastPtrToInt64( const void* p );

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		SafeSwapFloat( pOut, pIn )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )
#define SwapFloat( pOut, pIn )		BigFloat( pOut, pIn )

PLATFORM_INTERFACE bool				Plat_IsInTestMode();

PLATFORM_INTERFACE double			Plat_FloatTime();		// Returns time in seconds since the module was loaded.
PLATFORM_INTERFACE uint32_t			Plat_MSTime();			// Time in milliseconds.
PLATFORM_INTERFACE uint64			Plat_MSTime64();		// Time in milliseconds.
PLATFORM_INTERFACE uint64			Plat_USTime();			// Time in microseconds.
PLATFORM_INTERFACE char *			Plat_ctime64( const time_t *timep, char *buf, size_t bufsize );
PLATFORM_INTERFACE uint64			Plat_GetClockStart();	// Snapshot of the clock when app started.
PLATFORM_INTERFACE time_t			Plat_GetTime();			// Same as time().

template <class T>
inline T* Construct( T* pMemory )
{
	return reinterpret_cast<T*>(::new( pMemory ) T);
}

template <class T, typename... ARGS>
inline T* Construct( T* pMemory, ARGS&&... args )
{
	return reinterpret_cast<T*>(::new( pMemory ) T( std::forward<ARGS>( args )... ));
}

template <class T, class P>
inline void ConstructOneArg( T* pMemory, P const& arg)
{
	::new( pMemory ) T(arg);
}

template <class T, class P1, class P2 >
inline void ConstructTwoArg( T* pMemory, P1 const& arg1, P2 const& arg2)
{
	::new( pMemory ) T(arg1, arg2);
}

template <class T, class P1, class P2, class P3 >
inline void ConstructThreeArg( T* pMemory, P1 const& arg1, P2 const& arg2, P3 const& arg3)
{
	::new( pMemory ) T(arg1, arg2, arg3);
}

template <class T>
inline T* CopyConstruct( T* pMemory, T const& src )
{
	return reinterpret_cast<T*>(::new( pMemory ) T(src));
}

template <class T>
inline T* MoveConstruct( T* pMemory, T&& src )
{
	return reinterpret_cast<T*>(::new( pMemory ) T(std::move(src)));
}

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

#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) )

PLATFORM_INTERFACE void			V_tier0_memset( void *dest, int fill, size_t count );
PLATFORM_INTERFACE void			V_tier0_memcpy( void *dest, const void *src, size_t count );
PLATFORM_INTERFACE void			V_tier0_memmove( void *dest, const void *src, size_t count );
PLATFORM_INTERFACE int			V_tier0_memcmp( const void *m1, const void *m2, size_t count );

#define V_memset(dest, fill, count)		V_tier0_memset		((dest), (fill), (count))
#define V_memcpy(dest, src, count)		V_tier0_memcpy		((dest), (src), (count))
#define V_memmove(dest, src, count)		V_tier0_memmove		((dest), (src), (count))
#define V_memcmp(m1, m2, count)			V_tier0_memcmp		((m1), (m2), (count))
#define Q_memcmp				V_memcmp

PLATFORM_INTERFACE uint64 Plat_CPUTickFrequency();
PLATFORM_INTERFACE void Plat_Printf( const tchar *pFmt, ... ) FMTFUNCTION( 1, 2 );


#define Plat_MessageBox( ... ) ((void)0)

//-----------------------------------------------------------------------------
// What OS version are we?
//-----------------------------------------------------------------------------
enum EOSType
{
	k_eIOSUnknown = -600,

	k_eAndroidUnknown = -500,

	k_eAndroid50,
	k_eAndroid51,
	k_eAndroid60,
	k_eAndroid70,
	k_eAndroid711,
	k_eAndroid8,
	k_eAndroid8MR1,
	k_eAndroid9,
	k_eAndroid10,

	k_eAndroidMax,

	k_eOSUMQ = -400,

	k_eOSPS3 = -300,

	k_eLinuxUnknown = -203,

	k_eLinux22,
	k_eLinux24,
	k_eLinux26,
	k_eLinux32,
	k_eLinux35,
	k_eLinux36,
	k_eLinux310,
	k_eLinux40,
	k_eLinux45,
	k_eLinux418,
	k_eLinux50,

	k_eLinuxMax,

	k_eMacOSUnknown = -102,

	k_eMacOS104,
	k_eMacOS105,
	k_eMacOS1058,
	k_eMacOS106 = -95,
	k_eMacOS1063,
	k_eMacOS1064_slgu,
	k_eMacOS1067,
	k_eMacOS107 = -90,
	k_eMacOS108,
	k_eMacOS109,
	k_eMacOS1010,
	k_eMacOS1011,
	k_eMacOS1012,
	k_eMacOS1013,
	k_eMacOS1014,
	k_eMacOS1015,
	k_eMacOS1016,
	k_eMacOS11,
	k_eMacOS111,
	k_eMacOS1017,
	k_eMacOS12,
	k_eMacOS1018,
	k_eMacOS13,
	k_eMacOS14,
	k_eMacOS15,

	k_eMacOSMax,

	k_eWinUnknown = 0,

	k_eWin311,
	k_eWin95,
	k_eWin98,
	k_eWinME,
	k_eWinNT,
	k_eWin2000,
	k_eWinXP,
	k_eWin2003,
	k_eWinVista,
	k_eWin2008,
	k_eWindows7,
	k_eWin2008R2,
	k_eWin2012,
	k_eWindows8,
	k_eWindows81,
	k_eWin2012R2,
	k_eWindows10,
	k_eWin2016,
	k_eWin2019,
	k_eWin2022,
	k_eWindows11,

	k_eWinMAX,

	k_eOSUnknown = -1
};

PLATFORM_INTERFACE EOSType Plat_GetOSType();
PLATFORM_INTERFACE bool Plat_GetOSDescriptionString(char *buf, size_t size);
PLATFORM_INTERFACE const char *GetNameFromOSType( EOSType type );

#endif // PLATFORM_H
