#ifndef _TIER0_THREADINTERLOCKS_H
#define _TIER0_THREADINTERLOCKS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"

#ifdef _WIN32
#include <intrin.h>
#endif

#if defined(_MSC_VER)
	// Prevent compiler reordering across this barrier. This is
	// sufficient for most purposes on x86/x64.

	#if _MSC_VER < 1500
		// !KLUDGE! For VC 2005
		// http://connect.microsoft.com/VisualStudio/feedback/details/100051
		#pragma intrinsic(_ReadWriteBarrier)
	#endif
	#define ThreadMemoryBarrier() _ReadWriteBarrier()
#elif defined(GNUC)
	// Prevent compiler reordering across this barrier. This is
	// sufficient for most purposes on x86/x64.
	// http://preshing.com/20120625/memory-ordering-at-compile-time
	#define ThreadMemoryBarrier() asm volatile("" ::: "memory")
#else
	#error Every platform needs to define ThreadMemoryBarrier to at least prevent compiler reordering
#endif

#if defined( POSIX )
// linux implementation
inline int32_t ThreadInterlockedIncrement( int32_t volatile *p )										{ Assert( (size_t)p % 4 == 0 ); return __sync_add_and_fetch( p, 1 ); }
inline int32_t ThreadInterlockedDecrement( int32_t volatile *p )										{ Assert( (size_t)p % 4 == 0 ); return __sync_sub_and_fetch( p, 1 ); }
inline int32_t ThreadInterlockedExchange( int32_t volatile *p, int32_t value )							{ Assert( (size_t)p % 4 == 0 ); return __sync_lock_test_and_set( p, value ); }
inline int32_t ThreadInterlockedExchangeAdd( int32_t volatile *p, int32_t value )							{ Assert( (size_t)p % 4 == 0 ); return __sync_fetch_and_add( p, value ); }
inline int32_t ThreadInterlockedCompareExchange( int32_t volatile *p, int32_t value, int32_t comperand )	{ Assert( (size_t)p % 4 == 0 ); return __sync_val_compare_and_swap( p, comperand, value ); }
inline bool ThreadInterlockedAssignIf( int32_t volatile *p, int32_t value, int32_t comperand )			{ Assert( (size_t)p % 4 == 0 ); return __sync_bool_compare_and_swap( p, comperand, value ); }

inline int64_t ThreadInterlockedIncrement64( int64_t volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return __sync_add_and_fetch( p, 1 ); }
inline int64_t ThreadInterlockedDecrement64( int64_t volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return __sync_sub_and_fetch( p, 1 ); }
inline int64_t ThreadInterlockedExchange64( int64_t volatile *p, int64_t value )							{ Assert( (size_t)p % 8 == 0 ); return __sync_lock_test_and_set( p, value ); }
inline int64_t ThreadInterlockedExchangeAdd64( int64_t volatile *p, int64_t value )						{ Assert( (size_t)p % 8 == 0 ); return __sync_fetch_and_add( p, value ); }
inline int64_t ThreadInterlockedCompareExchange64( int64_t volatile *p, int64_t value, int64_t comperand )	{ Assert( (size_t)p % 8 == 0 ); return __sync_val_compare_and_swap( p, comperand, value ); }
inline bool ThreadInterlockedAssignIf64( int64_t volatile *p, int64_t value, int64_t comperand )			{ Assert( (size_t)p % 8 == 0 ); return __sync_bool_compare_and_swap( p, comperand, value ); }

inline void *ThreadInterlockedExchangePointer( void * volatile *p, void *value )							{ Assert( (size_t)p % sizeof(void*) == 0 ); return __sync_lock_test_and_set( p, value ); }
inline void *ThreadInterlockedCompareExchangePointer( void * volatile *p, void *value, void *comperand )	{ Assert( (size_t)p % sizeof(void*) == 0 ); return __sync_val_compare_and_swap( p, comperand, value ); }
inline bool ThreadInterlockedAssignPointerIf( void * volatile *p, void *value, void *comperand )			{ Assert( (size_t)p % sizeof(void*) == 0 ); return __sync_bool_compare_and_swap( p, comperand, value ); }
#elif ( defined( _WIN32 ) && ( _MSC_VER >= 1310 ) )
// windows implemnetation using compiler intrinsics
#pragma intrinsic( _InterlockedIncrement )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedExchange )
#pragma intrinsic( _InterlockedExchangeAdd ) 
#pragma intrinsic( _InterlockedCompareExchange )

inline int32_t ThreadInterlockedIncrement( int32_t volatile *p )										{ Assert( (size_t)p % 4 == 0 ); return _InterlockedIncrement( (volatile long*)p ); }
inline int32_t ThreadInterlockedDecrement( int32_t volatile *p )										{ Assert( (size_t)p % 4 == 0 ); return _InterlockedDecrement( (volatile long*)p ); }
inline int32_t ThreadInterlockedExchange( int32_t volatile *p, int32_t value )							{ Assert( (size_t)p % 4 == 0 ); return _InterlockedExchange( (volatile long*)p, value ); }
inline int32_t ThreadInterlockedExchangeAdd( int32_t volatile *p, int32_t value )							{ Assert( (size_t)p % 4 == 0 ); return _InterlockedExchangeAdd( (volatile long*)p, value ); }
inline int32_t ThreadInterlockedCompareExchange( int32_t volatile *p, int32_t value, int32_t comperand )	{ Assert( (size_t)p % 4 == 0 ); return _InterlockedCompareExchange( (volatile long*)p, value, comperand ); }
inline bool ThreadInterlockedAssignIf( int32_t volatile *p, int32_t value, int32_t comperand )			{ Assert( (size_t)p % 4 == 0 ); return ( _InterlockedCompareExchange( (volatile long*)p, value, comperand ) == comperand ); }

#pragma intrinsic( _InterlockedIncrement64 )
#pragma intrinsic( _InterlockedDecrement64 )
#pragma intrinsic( _InterlockedExchange64 )
#pragma intrinsic( _InterlockedExchangeAdd64 ) 
#pragma intrinsic( _InterlockedCompareExchange64 )

inline int64_t ThreadInterlockedIncrement64( int64_t volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return _InterlockedIncrement64( (volatile __int64*)p ); }
inline int64_t ThreadInterlockedDecrement64( int64_t volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return _InterlockedDecrement64( (volatile __int64*)p ); }
inline int64_t ThreadInterlockedExchange64( int64_t volatile *p, int64_t value )							{ Assert( (size_t)p % 8 == 0 ); return _InterlockedExchange64( (volatile __int64*)p, value ); }
inline int64_t ThreadInterlockedExchangeAdd64( int64_t volatile *p, int64_t value )						{ Assert( (size_t)p % 8 == 0 ); return _InterlockedExchangeAdd64( (volatile __int64*)p, value ); }
inline int64_t ThreadInterlockedCompareExchange64( int64_t volatile *p, int64_t value, int64_t comperand )	{ Assert( (size_t)p % 8 == 0 ); return _InterlockedCompareExchange64( (volatile __int64*)p, value, comperand ); }
inline bool ThreadInterlockedAssignIf64( int64_t volatile *p, int64_t value, int64_t comperand )			{ Assert( (size_t)p % 8 == 0 ); return ( _InterlockedCompareExchange64( (volatile __int64*)p, value, comperand ) == comperand ); }

#pragma intrinsic( _InterlockedExchangePointer )
#pragma intrinsic( _InterlockedCompareExchangePointer )

inline void *ThreadInterlockedExchangePointer( void * volatile *p, void *value )							{ Assert( (size_t)p % sizeof(void*) == 0 ); return _InterlockedExchangePointer( p, value );  }
inline void *ThreadInterlockedCompareExchangePointer( void * volatile *p, void *value, void *comperand )	{ Assert( (size_t)p % sizeof(void*) == 0 ); return _InterlockedCompareExchangePointer( p, value, comperand ); }
inline bool ThreadInterlockedAssignPointerIf( void * volatile *p, void *value, void *comperand )			{ Assert( (size_t)p % sizeof(void*) == 0 ); return ( _InterlockedCompareExchangePointer( p, value, comperand ) == comperand ); }
#else
PLATFORM_INTERFACE int32 ThreadInterlockedIncrement( int32 volatile * ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedDecrement( int32 volatile * ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedExchange( int32 volatile *, int32 value ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedExchangeAdd( int32 volatile *, int32 value ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedCompareExchange( int32 volatile *, int32 value, int32 comperand ) NOINLINE;
PLATFORM_INTERFACE bool ThreadInterlockedAssignIf( int32 volatile *, int32 value, int32 comperand ) NOINLINE;

PLATFORM_INTERFACE int64 ThreadInterlockedIncrement64( int64 volatile * ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedDecrement64( int64 volatile * ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedExchange64( int64 volatile *, int64 value ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedExchangeAdd64( int64 volatile *, int64 value ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedCompareExchange64( int64 volatile *, int64 value, int64 comperand ) NOINLINE;
PLATFORM_INTERFACE bool ThreadInterlockedAssignIf64(volatile int64 *pDest, int64 value, int64 comperand ) NOINLINE;

PLATFORM_INTERFACE void *ThreadInterlockedExchangePointer( void * volatile *, void *value ) NOINLINE;
PLATFORM_INTERFACE void *ThreadInterlockedCompareExchangePointer( void * volatile *, void *value, void *comperand ) NOINLINE;
PLATFORM_INTERFACE bool ThreadInterlockedAssignPointerIf( void * volatile *, void *value, void *comperand ) NOINLINE;
#endif

#if defined (_WIN32) 
typedef __m128i int128;
inline int128 int128_zero()	{ return _mm_setzero_si128(); }

#pragma intrinsic( _InterlockedCompareExchange128 )

inline bool ThreadInterlockedAssignIf128( volatile int128 *pDest, const int128 &value, const int128 &comperand )
{
	Assert( (size_t)pDest % 16 == 0 );

	volatile int64_t *pDest64 = ( volatile int64_t * )pDest;
	int64_t *pValue64 = ( int64_t * )&value;
	int64_t *pComperand64 = ( int64_t * )&comperand;
	
	int64_t local_comperand[2] = { pComperand64[0], pComperand64[1] };

	return _InterlockedCompareExchange128( pDest64, pValue64[1], pValue64[0], local_comperand ) == 1;
}
#else
typedef __int128_t int128;
#define int128_zero() 0

inline bool ThreadInterlockedAssignIf128( volatile int128 *pDest, const int128 &value, const int128 &comperand )
{
	Assert( (size_t)pDest % 16 == 0 );
	
	int128 local_comperand = comperand;
	
	return __sync_bool_compare_and_swap( pDest, local_comperand, value );
}
#endif

template <typename T>
class CInterlockedIntT
{
public:
	CInterlockedIntT() : m_value( 0 ) 				{ static_assert( sizeof(T) == sizeof(int32_t) ); }
	CInterlockedIntT( T value ) : m_value( value ) 	{}

	T operator()( void ) const      { return m_value; }
	operator T() const				{ return m_value; }

	bool operator!() const			{ return ( m_value == 0 ); }
	bool operator==( T rhs ) const	{ return ( m_value == rhs ); }
	bool operator!=( T rhs ) const	{ return ( m_value != rhs ); }

	T operator++()					{ return (T)ThreadInterlockedIncrement( (int32_t *)&m_value ); }
	T operator++(int)				{ return operator++() - 1; }

	T operator--()					{ return (T)ThreadInterlockedDecrement( (int32_t *)&m_value ); }
	T operator--(int)				{ return operator--() + 1; }

	bool AssignIf( T conditionValue, T newValue )	{ return ThreadInterlockedAssignIf( (int32_t *)&m_value, (int32_t)newValue, (int32_t)conditionValue ); }

	T operator=( T newValue )		{ ThreadInterlockedExchange((int32_t *)&m_value, newValue); return m_value; }

	// Atomic add is like += except it returns the previous value as its return value
	T AtomicAdd( T add )			{ return (T)ThreadInterlockedExchangeAdd( (int32_t *)&m_value, (int32_t)add ); }
	T CompareExchange( T exchange, T comperand )	{ return (T)ThreadInterlockedCompareExchange( (int32_t *)&m_value, (int32_t)exchange, (int32_t)comperand ); }

	void operator+=( T add )		{ ThreadInterlockedExchangeAdd( (int32_t *)&m_value, (int32_t)add ); }
	void operator-=( T subtract )	{ operator+=( -subtract ); }
	void operator*=( T multiplier )	{ 
		T original, result; 
		do 
		{ 
			original = m_value; 
			result = original * multiplier; 
		} while ( !AssignIf( original, result ) );
	}
	void operator/=( T divisor )	{ 
		T original, result; 
		do 
		{ 
			original = m_value; 
			result = original / divisor;
		} while ( !AssignIf( original, result ) );
	}

	T operator+( T rhs ) const		{ return m_value + rhs; }
	T operator-( T rhs ) const		{ return m_value - rhs; }

private:
	volatile T m_value;
};

typedef CInterlockedIntT<int> CInterlockedInt;
typedef CInterlockedIntT<unsigned> CInterlockedUInt;

#endif