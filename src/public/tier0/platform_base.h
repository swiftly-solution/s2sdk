#ifndef PLATFORM_COMPILE_BASE_H
#define PLATFORM_COMPILE_BASE_H

#ifdef _WIN32
#pragma once
#endif

#include <cstddef>
#include <cstdint>
#include <assert.h>
#include <limits>

#define VALIGNOF_PORTABLE( type ) ( sizeof( AlignOf_t<type> ) - sizeof( type ) )
#define VALIGNOF( type ) __alignof( type )
#define VALIGNOF_TEMPLATE_SAFE( type ) VALIGNOF_PORTABLE( type )

#ifdef _WIN32
	#define DLL_EXPORT				extern "C" __declspec( dllexport )
	#define DLL_IMPORT				extern "C" __declspec( dllimport )

	#define DLL_CLASS_EXPORT		__declspec( dllexport )
	#define DLL_CLASS_IMPORT		__declspec( dllimport )

	#define DLL_GLOBAL_EXPORT		extern __declspec( dllexport )
	#define DLL_GLOBAL_IMPORT		extern __declspec( dllimport )

    #define DebuggerBreak()         __debugbreak()
    #define FMTFUNCTION( a, b )
    #define DECL_ALIGN(x)			__declspec( align( x ) )
#else
	#define DLL_EXPORT				extern "C" __attribute__ ((visibility("default")))
	#define DLL_IMPORT				extern "C"

	#define DLL_CLASS_EXPORT		__attribute__ ((visibility("default")))
	#define DLL_CLASS_IMPORT

	#define DLL_GLOBAL_EXPORT		__attribute__((visibility("default")))
	#define DLL_GLOBAL_IMPORT		extern

    #define DebuggerBreak()  raise(SIGTRAP)
    #define FMTFUNCTION( fmtargnumber, firstvarargnumber ) __attribute__ (( format( printf, fmtargnumber, firstvarargnumber )))
    #define DECL_ALIGN(x)			__attribute__( ( aligned( x ) ) )
#endif

#define PLATFORM_INTERFACE	DLL_IMPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_IMPORT
#define PLATFORM_CLASS		DLL_CLASS_IMPORT
#define Assert              assert

#if !defined( offsetof )
	#ifdef __GNUC__
		#define offsetof( type, var ) __builtin_offsetof( type, var )
	#else
		#define offsetof(s,m)	(size_t)&(((s *)0)->m)
	#endif
#endif

#if defined( GNUC )
// gnuc has the align decoration at the end
#define ALIGN4
#define ALIGN8
#define ALIGN16
#define ALIGN32
#define ALIGN128
#define ALIGN_N( _align_ )

#define ALIGN4_POST DECL_ALIGN(4)
#define ALIGN8_POST DECL_ALIGN(8)
#define ALIGN16_POST DECL_ALIGN(16)
#define ALIGN32_POST DECL_ALIGN(32)
#define ALIGN128_POST DECL_ALIGN(128)
#define ALIGN_N_POST( _align_ ) DECL_ALIGN( _align_ )
#else
// MSVC has the align at the start of the struct
#define ALIGN4 DECL_ALIGN(4)
#define ALIGN8 DECL_ALIGN(8)
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN32 DECL_ALIGN(32)
#define ALIGN128 DECL_ALIGN(128)
#define ALIGN_N( _align_ ) DECL_ALIGN( _align_ )

#define ALIGN4_POST
#define ALIGN8_POST
#define ALIGN16_POST
#define ALIGN32_POST
#define ALIGN128_POST
#define ALIGN_N_POST( _align_ )
#endif

#define INT_TO_POINTER( i ) (void *)( ( i ) + (char *)NULL )
#define POINTER_TO_INT( p ) ( (int)(uintp)( p ) )

struct empty_t {};

PLATFORM_INTERFACE void				Plat_NonFatalErrorFunc( const char *pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void				Plat_FatalError( const char *pMsg, ... ) FMTFUNCTION( 1, 2 );

PLATFORM_INTERFACE bool Plat_IsInDebugSession();
PLATFORM_INTERFACE void Plat_DebugString( const char * );

#define ALIGN_ASSERT( pred ) { static_assert( pred ); }
template< class T, int ALIGN >
inline void ValidateAlignmentExplicit(void)
{
	// Alignment must be a power of two
	ALIGN_ASSERT((ALIGN & (ALIGN - 1)) == 0);
	// Alignment must not imply gaps in the array (which the CUtlMemory pattern does not allow for)
	ALIGN_ASSERT(ALIGN <= sizeof(T));
	// Alignment must be a multiple of the size of the object type, or elements will *NOT* be aligned!
	ALIGN_ASSERT((sizeof(T) % ALIGN) == 0);
	// Alignment should be a multiple of the base alignment of T
	ALIGN_ASSERT((ALIGN % VALIGNOF(T)) == 0);
	// Alignment must not be bigger than the maximum declared alignment used by DECLARE_ALIGNED_BYTE_ARRAY
	// (if you hit this, just add more powers of 2 below and increase this limit)
	ALIGN_ASSERT( ALIGN <= 128 );
}
template< class T > inline void ValidateAlignment(void) { ValidateAlignmentExplicit<T, VALIGNOF(T)>(); }

// Portable alternative to __alignof
template<class T> struct AlignOf_t { AlignOf_t(){} AlignOf_t & operator=(const AlignOf_t &) { return *this; } uint8_t b; T t; };

template < size_t NUM, class T, int ALIGN > struct AlignedByteArrayExplicit_t{};
template < size_t NUM, class T > struct AlignedByteArray_t : public AlignedByteArrayExplicit_t< NUM, T, VALIGNOF_TEMPLATE_SAFE(T) > {};

#define DECLARE_ALIGNED_BYTE_ARRAY( ALIGN ) \
	template < size_t NUM, class T > \
	struct ALIGN_N( ALIGN ) AlignedByteArrayExplicit_t< NUM, T, ALIGN > \
	{ \
		/* NOTE: verify alignment in the constructor (which may be wrong if this is heap-allocated, for ALIGN > MEMALLOC_MAX_AUTO_ALIGN) */ \
		AlignedByteArrayExplicit_t()	{ if ( (ALIGN-1) & (size_t)this ) { Plat_NonFatalErrorFunc( "AlignedByteArray not properly aligned\n" ); } } \
		T *			Base( void )		{ ValidateAlignmentExplicit<T,ALIGN>(); return (T *)&m_Data; } \
		const T *	Base( void ) const	{ ValidateAlignmentExplicit<T,ALIGN>(); return (const T *)&m_Data; } \
		size_t		Count() const		{ return NUM; } \
		T &operator[]( int i )				{ return Base()[i]; } \
		const T &operator[]( int i ) const	{ return Base()[i]; } \
	private: \
		uint8_t m_Data[ NUM*sizeof( T ) ]; \
	} ALIGN_N_POST( ALIGN );

DECLARE_ALIGNED_BYTE_ARRAY(1);
DECLARE_ALIGNED_BYTE_ARRAY(2);
DECLARE_ALIGNED_BYTE_ARRAY(4);
DECLARE_ALIGNED_BYTE_ARRAY(8);
DECLARE_ALIGNED_BYTE_ARRAY(16);
DECLARE_ALIGNED_BYTE_ARRAY(32);
DECLARE_ALIGNED_BYTE_ARRAY(64);
DECLARE_ALIGNED_BYTE_ARRAY(128);

#endif