#ifndef PLATFORM_COMPILE_BASE_H
#define PLATFORM_COMPILE_BASE_H

#if defined( _MSC_VER ) && !defined( MSVC )
	#define MSVC 1
#endif

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

#if defined( _WIN32 ) && !defined( PLATFORM_WINDOWS )
	#define PLATFORM_WINDOWS 1
	#define PLATFORM_WINDOWS_PC 1
#endif

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

    #define SINGLE_INHERITANCE		__single_inheritance
	#define MULTIPLE_INHERITANCE	__multiple_inheritance
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

    #define SINGLE_INHERITANCE		
	#define MULTIPLE_INHERITANCE	
#endif

#define PLATFORM_INTERFACE	DLL_IMPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_IMPORT
#define PLATFORM_CLASS		DLL_CLASS_IMPORT
#define Assert              assert

#if defined( _MSC_VER ) && defined( _WIN64 ) && !defined( COMPILER_MSVC64 )
#define COMPILER_MSVC64 1
#endif
#ifndef AssertMsg
#define AssertMsg( expr, msg ) assert( ( expr ) && ( msg ) )
#endif

typedef unsigned char byte;
typedef unsigned int uint;
typedef int qboolean;
typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;
typedef float float32;
typedef double float64;

#ifndef MAX
	#define MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )
#endif
#ifndef MIN
	#define MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )
#endif
#ifndef stackalloc
	#define stackalloc( _size ) alloca( _size )
#endif

#if !defined( offsetof )
	#ifdef __GNUC__
		#define offsetof( type, var ) __builtin_offsetof( type, var )
	#else
		#define offsetof(s,m)	(size_t)&(((s *)0)->m)
	#endif
#endif

#define GET_OUTER( OuterType, OuterMember ) \
	( ( OuterType * ) ( (uint8_t *)this - offsetof( OuterType, OuterMember ) ) )

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

#ifdef _WIN32
#define CORRECT_PATH_SEPARATOR '\\'
#define CORRECT_PATH_SEPARATOR_S "\\"
#define INCORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR_S "/"
#else
#define CORRECT_PATH_SEPARATOR '/'
#define CORRECT_PATH_SEPARATOR_S "/"
#define INCORRECT_PATH_SEPARATOR '\\'
#define INCORRECT_PATH_SEPARATOR_S "\\"
#endif

#define INT_TO_POINTER( i ) (void *)( ( i ) + (char *)NULL )
#define POINTER_TO_INT( p ) ( (int)(uintptr_t)( p ) )
#define MAX_PATH  260

struct empty_t {};

#ifdef _WIN32
	#define STDCALL __stdcall
	typedef void *HANDLE;
#else
	#define STDCALL
#endif

#if defined( _WIN64 ) || defined( __LP64__ ) || defined( __x86_64__ ) || defined( __aarch64__ )
	#define PLATFORM_64BITS 1
#endif

typedef uintptr_t uintp;

#define FORWARD_DECLARE_HANDLE( name ) typedef struct name##__ *name

#define COMPILE_TIME_ASSERT( pred ) static_assert( pred, "Compile time assert constraint is not true: " #pred )



#ifndef FORCEINLINE
	#ifdef _MSC_VER
		#define FORCEINLINE __forceinline
	#else
		#define FORCEINLINE inline __attribute__((always_inline))
	#endif
#endif

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

typedef float vec_t;
typedef intptr_t intp;
typedef char tchar;

#ifdef _WIN32
	typedef void *OSFILEHANDLE;
#else
	typedef int OSFILEHANDLE;
	typedef void *HMODULE;
#endif

#ifdef _MSC_VER
	#define NO_VTABLE __declspec( novtable )
	#define RESTRICT __restrict
	#define RESTRICT_FUNC __declspec( restrict )
#else
	#define NO_VTABLE
	#define RESTRICT __restrict
	#define RESTRICT_FUNC
#endif

#define abstract_class class

#if defined( __GNUC__ )
	#define mallocsize( _p ) ( malloc_usable_size( _p ) )
#elif defined( _MSC_VER )
	#include <malloc.h>
	#define mallocsize( _p ) ( _msize( _p ) )
#endif

typedef void* (*CreateInterfaceFn)( const char *pName, int *pReturnCode );
typedef void* (*InstantiateInterfaceFn)();

#endif