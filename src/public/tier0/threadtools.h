//========== Copyright © 2005, Valve Corporation, All rights reserved. ========
//
// Purpose: A collection of utility classes to simplify thread handling, and
//			as much as possible contain portability problems. Here avoiding
//			including windows.h.
//
//=============================================================================

#ifndef _TIER0_THREADTOOLS_H
#define _TIER0_THREADTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include <limits.h>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>

#include "platform_base.h"
#include "basetypes.h"
#include "threadinterlocks.h"

#ifdef POSIX
#include <pthread.h>
#include <errno.h>
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT 0x00000102
#define WAIT_FAILED -1
#define THREAD_PRIORITY_HIGHEST 2
#endif

#if defined( _WIN32 )
#pragma once
#pragma warning(push)
#pragma warning(disable:4251)
#endif

// #define THREAD_PROFILER 1

#define THREAD_MUTEX_TRACING_SUPPORTED
#if defined(_WIN32) && defined(_DEBUG)
#define THREAD_MUTEX_TRACING_ENABLED
#endif

#define MAX_THREADS_SUPPORTED 32



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

const unsigned TT_INFINITE = 0xffffffff;

#ifdef _WIN32
	typedef uint32 ThreadId_t;
#else
	typedef uint64 ThreadId_t;
#endif

//-----------------------------------------------------------------------------
//
// Simple thread creation. Differs from VCR mode/CreateThread/_beginthreadex
// in that it accepts a standard C function rather than compiler specific one.
//
//-----------------------------------------------------------------------------
FORWARD_DECLARE_HANDLE( ThreadHandle_t );
typedef uintp (*ThreadFunc_t)( void *pParam );

PLATFORM_INTERFACE ThreadHandle_t CreateSimpleThread( ThreadFunc_t, void *pParam, unsigned stackSize = 0 );
PLATFORM_INTERFACE bool ReleaseThreadHandle( ThreadHandle_t );


//-----------------------------------------------------------------------------

PLATFORM_OVERLOAD void ThreadAtomicNotifyOne( const uint32 *addr );
PLATFORM_OVERLOAD void ThreadAtomicNotifyN( const uint32 *addr, uint32 n );
PLATFORM_OVERLOAD void ThreadAtomicNotifyAll( const uint32 *addr );
PLATFORM_OVERLOAD void ThreadAtomicWait( volatile uint32 *addr, uint32 value );

PLATFORM_INTERFACE void ThreadSleep(unsigned duration = 0);
PLATFORM_INTERFACE void ThreadNanoSleep( uint32 nNanoseconds );
PLATFORM_INTERFACE void ThreadYield();
// Busy waits for nCycles ticks of the CPU's timestamp counter
PLATFORM_INTERFACE void ThreadSpin( uint32 nCycles );
PLATFORM_INTERFACE ThreadId_t ThreadGetCurrentId();
PLATFORM_INTERFACE ThreadHandle_t ThreadGetCurrentHandle();
PLATFORM_INTERFACE int ThreadGetPriority( ThreadHandle_t hThread = NULL );
PLATFORM_INTERFACE bool ThreadSetPriority( ThreadHandle_t hThread, int priority );
bool ThreadSetPriority( int priority );
PLATFORM_INTERFACE bool ThreadInMainThread();
PLATFORM_INTERFACE void DeclareCurrentThreadIsMainThread();
PLATFORM_INTERFACE ThreadId_t ThreadGetMainThreadId();

// AMNOTE: The engine declares a secondary main thread while main thread work runs on another thread,
// like preparing the client updates after a server tick
PLATFORM_INTERFACE void DeclareCurrentThreadIsSecondaryMainThread();
PLATFORM_INTERFACE void ClearSecondaryMainThread();
PLATFORM_INTERFACE bool ThreadInSecondaryMainThread();
PLATFORM_INTERFACE bool ThreadInPrimaryOrSecondaryMainThread();

// NOTE: ThreadedLoadLibraryFunc_t needs to return the sleep time in milliseconds or TT_INFINITE
typedef int (*ThreadedLoadLibraryFunc_t)();
PLATFORM_INTERFACE void SetThreadedLoadLibraryFunc( ThreadedLoadLibraryFunc_t func );
PLATFORM_INTERFACE ThreadedLoadLibraryFunc_t GetThreadedLoadLibraryFunc();

#if defined( _WIN32 )
DLL_IMPORT unsigned long STDCALL GetCurrentThreadId();
#define ThreadGetCurrentId GetCurrentThreadId
#endif

void ThreadPause();

PLATFORM_INTERFACE bool ThreadJoin( ThreadHandle_t, unsigned timeout = TT_INFINITE );

PLATFORM_INTERFACE void ThreadSetDebugNameS2( ThreadHandle_t hThread, const char *pszName );
void ThreadSetDebugName( const char *pszName );
PLATFORM_INTERFACE bool ThreadGetDebugNameForThreadHandle( ThreadHandle_t hThread, char *pszNameOut, size_t nNameOutSize );
PLATFORM_INTERFACE bool ThreadGetDebugNameForThreadId( ThreadId_t nThreadId, char *pszNameOut, size_t nNameOutSize );

PLATFORM_INTERFACE void ThreadSetAffinity( ThreadHandle_t hThread, int nAffinityMask );

// Placeholder until tier0/cpu.h is ported. Only the enum's name and size are known here.
struct CPUInformation
{
	struct ProcessorInfo
	{
		enum EfficiencyType : int32;
	};
};

// Fills pSortIndices with indices into CPUInformation::m_pProcessorInfo, highest m_EfficiencyClass first,
// returning how many it wrote (0 without processor info)
PLATFORM_INTERFACE uint16 GetPerfSortedLogicalProcessors( uint16 *pSortIndices, uint16 nSortIndices );
// On hybrid CPUs, limits the current thread to the logical processors of the given efficiency type
PLATFORM_INTERFACE void ThreadSetPerfSetAffinity( CPUInformation::ProcessorInfo::EfficiencyType eType );


//-----------------------------------------------------------------------------
// Runs code once across threads, with a marker initialized to 0:
//	if ( ThreadInitOnceCall_ShouldRun( &marker ) ) { ...; ThreadInitOnceCall_Finish( &marker ); }
// ShouldRun returns true to the first caller only, and makes the others wait until it finishes.
//-----------------------------------------------------------------------------
typedef volatile int32 ThreadInitOnce_t;

PLATFORM_INTERFACE bool ThreadInitOnceCall_ShouldRun( ThreadInitOnce_t *pMarker );
PLATFORM_INTERFACE void ThreadInitOnceCall_Finish( ThreadInitOnce_t *pMarker );

//-----------------------------------------------------------------------------
// Processes
//-----------------------------------------------------------------------------
// AMNOTE: A HANDLE on Windows; on Linux a pointer to tier0's process record
FORWARD_DECLARE_HANDLE( ProcessHandle_t );

enum ThreadProcessPriority_t
{
	THREAD_PROCESS_PRIORITY_IDLE,
	THREAD_PROCESS_PRIORITY_LOW,
	THREAD_PROCESS_PRIORITY_NORMAL,
	THREAD_PROCESS_PRIORITY_HIGH,
	THREAD_PROCESS_PRIORITY_REALTIME,
};

// A pseudo handle that doesn't need to be closed
PLATFORM_INTERFACE ProcessHandle_t ThreadGetCurrentProcessHandle();
PLATFORM_INTERFACE uint32 ThreadGetCurrentProcessId();
PLATFORM_INTERFACE uint32 ThreadGetProcessIdFromHandle( ProcessHandle_t hProcess );

// Translated to the matching PROCESS_* access rights on Windows.
// On Linux only READ_MEMORY and WRITE_MEMORY have an effect, they ptrace attach to the process.
enum ThreadProcessAccessFlags_t
{
	THREAD_PROCESS_ACCESS_NONE = 0,
	THREAD_PROCESS_ACCESS_SYNCHRONIZE = (1 << 0),
	THREAD_PROCESS_ACCESS_READ_MEMORY = (1 << 1),
	THREAD_PROCESS_ACCESS_WRITE_MEMORY = (1 << 2),
	THREAD_PROCESS_ACCESS_MEMORY_OPERATION = (1 << 3),
	THREAD_PROCESS_ACCESS_QUERY_INFORMATION = (1 << 4),
	THREAD_PROCESS_ACCESS_SET_INFORMATION = (1 << 5),
	THREAD_PROCESS_ACCESS_SUSPEND_RESUME = (1 << 6),
	THREAD_PROCESS_ACCESS_CREATE_THREAD = (1 << 7),
	THREAD_PROCESS_ACCESS_TERMINATE = (1 << 8),
	THREAD_PROCESS_ACCESS_DUPLICATE_HANDLE = (1 << 9),
};
DEFINE_ENUM_BITWISE_OPERATORS( ThreadProcessAccessFlags_t )

// Returns 0 on success, otherwise the OS error code.
PLATFORM_INTERFACE int ThreadOpenProcess( uint32 nProcessId, ThreadProcessAccessFlags_t nAccessFlags, ProcessHandle_t *pProcessOut );
PLATFORM_INTERFACE bool ThreadCloseProcess( ProcessHandle_t hProcess );

PLATFORM_INTERFACE bool ThreadIsProcessIdActive( uint32 nProcessId );
// Return false and the exit code once the process has exited
PLATFORM_INTERFACE bool ThreadIsProcessActive( ProcessHandle_t hProcess, int32 *pExitCode );
PLATFORM_INTERFACE bool ThreadWaitForProcessExit( ProcessHandle_t hProcess, uint32 nMillis, int32 *pExitCode );
// Returns false while the process is running
PLATFORM_INTERFACE bool ThreadGetProcessExitCode( ProcessHandle_t hProcess, int32 *pExitCode );
// On Linux the process is killed, without nExitCode
PLATFORM_INTERFACE bool ThreadTerminateProcessCode( ProcessHandle_t hProcess, int32 nExitCode );

PLATFORM_INTERFACE ThreadProcessPriority_t ThreadGetProcessPriority( ProcessHandle_t hProcess );
PLATFORM_INTERFACE bool ThreadSetProcessPriority( ProcessHandle_t hProcess, ThreadProcessPriority_t ePriority );

// Returns the number of bytes read
PLATFORM_INTERFACE int ThreadReadProcessMemory( ProcessHandle_t hProcess, const void *pAddress, void *pBuffer, int nSize );

// Return the id of the started process, or 0 on failure
PLATFORM_INTERFACE uint32 ThreadShellExecute( const char *lpApplicationName, const char *pchCommandLine, const char *pchCurrentDirectory );
PLATFORM_INTERFACE uint32 ThreadShellExecuteEx( const char *pchVerb, const char *pchFile, const char *pchParameters, const char *pchCurrentDirectory );

//-----------------------------------------------------------------------------
// Access to VTune thread profiling
//-----------------------------------------------------------------------------
#if defined(_WIN32) && defined(THREAD_PROFILER)
PLATFORM_INTERFACE void ThreadNotifySyncPrepare(void *p);
PLATFORM_INTERFACE void ThreadNotifySyncCancel(void *p);
PLATFORM_INTERFACE void ThreadNotifySyncAcquired(void *p);
PLATFORM_INTERFACE void ThreadNotifySyncReleasing(void *p);
#else
#define ThreadNotifySyncPrepare(p)		((void)0)
#define ThreadNotifySyncCancel(p)		((void)0)
#define ThreadNotifySyncAcquired(p)		((void)0)
#define ThreadNotifySyncReleasing(p)	((void)0)
#endif

//-----------------------------------------------------------------------------
// Encapsulation of a thread local datum (needed because THREAD_LOCAL doesn't
// work in a DLL loaded with LoadLibrary()
//-----------------------------------------------------------------------------

#ifndef NO_THREAD_LOCAL

#ifdef POSIX
// linux totally supports compiler thread locals, even across dll's.
#define PLAT_COMPILER_SUPPORTED_THREADLOCALS 1
#define CTHREADLOCALINTEGER( typ ) __thread int
#define CTHREADLOCALINT __thread int
#define CTHREADLOCALPTR( typ ) __thread typ *
#define CTHREADLOCAL( typ ) __thread typ
#define GETLOCAL( x ) ( x )
#endif

#if defined( _WIN32 )
#ifndef __AFXTLS_H__ // not compatible with some Windows headers

#define CTHREADLOCALINT GenericThreadLocals::CThreadLocalInt<int>
#define CTHREADLOCALINTEGER( typ ) GenericThreadLocals::CThreadLocalInt<typ>
#define CTHREADLOCALPTR( typ ) GenericThreadLocals::CThreadLocalPtr<typ>
#define CTHREADLOCAL( typ ) GenericThreadLocals::CThreadLocal<typ>
#define GETLOCAL( x ) ( x.Get() )


namespace GenericThreadLocals
{
	// a (not so efficient) implementation of thread locals for compilers without full support (i.e. visual c).
	// don't use this explicity - instead, use the CTHREADxxx macros above.

	class PLATFORM_CLASS CThreadLocalBase
	{
public:
		CThreadLocalBase();
		~CThreadLocalBase();

		void * Get() const;
		void   Set(void *);

private:
#if defined(POSIX)
		pthread_key_t m_index;
#else
		uint32 m_index;
#endif
	};

	//---------------------------------------------------------

	template <class T>
	class CThreadLocal : public CThreadLocalBase
	{
	public:
		CThreadLocal()
		{
#ifdef PLATFORM_64BITS
			COMPILE_TIME_ASSERT( sizeof(T) <= sizeof(void *) );
#else
			COMPILE_TIME_ASSERT( sizeof(T) == sizeof(void *) );
#endif
		}

		void operator=( T i ) { Set( i ); }

		T Get() const
		{
#ifdef PLATFORM_64BITS
			void *pData = CThreadLocalBase::Get();
			return *reinterpret_cast<T*>( &pData );
#else
	#ifdef _MSC_VER
		#pragma warning ( disable : 4311 )
	#endif
			return reinterpret_cast<T>( CThreadLocalBase::Get() );
	#ifdef _MSC_VER
		#pragma warning ( default : 4311 )
	#endif
#endif
		}

		void Set(T val)
		{
#ifdef PLATFORM_64BITS
			void* pData = 0;
			*reinterpret_cast<T*>( &pData ) = val;
			CThreadLocalBase::Set( pData );
#else
	#ifdef _MSC_VER
		#pragma warning ( disable : 4312 )
	#endif
			CThreadLocalBase::Set( reinterpret_cast<void *>(val) );
	#ifdef _MSC_VER
		#pragma warning ( default : 4312 )
	#endif
#endif
		}
	};


	//---------------------------------------------------------

	template <class T = int32>
	class CThreadLocalInt : public CThreadLocal<T>
	{
	public:
		operator const T() const { return this->Get(); }
		int	operator=( T i ) { Set( i ); return i; }

		T operator++()					{ T i = this->Get(); this->Set( ++i ); return i; }
		T operator++(int)				{ T i = this->Get(); this->Set( i + 1 ); return i; }

		T operator--()					{ T i = this->Get(); this->Set( --i ); return i; }
		T operator--(int)				{ T i = this->Get(); this->Set( i - 1 ); return i; }

		inline CThreadLocalInt(  ) { }
		inline CThreadLocalInt( const T &initialvalue )
		{
			this->Set( initialvalue );
		}
	};


	//---------------------------------------------------------

	template <class T>
	class CThreadLocalPtr : private CThreadLocalBase
	{
	public:
		CThreadLocalPtr() {}

		operator const void *() const          					{ return (const T *)Get(); }
		operator void *()                      					{ return (T *)Get(); }

		operator const T *() const							    { return (const T *)Get(); }
		operator const T *()          							{ return (const T *)Get(); }
		operator T *()											{ return (T *)Get(); }

		T *			operator=( T *p )							{ Set( p ); return p; }

		bool        operator !() const							{ return (!Get()); }
		bool        operator!=( int i ) const					{ AssertMsg( i == 0, "Only NULL allowed on integer compare" ); return (Get() != NULL); }
		bool        operator==( int i ) const					{ AssertMsg( i == 0, "Only NULL allowed on integer compare" ); return (Get() == NULL); }
		bool		operator==( const void *p ) const			{ return (Get() == p); }
		bool		operator!=( const void *p ) const			{ return (Get() != p); }
		bool		operator==( const T *p ) const				{ return operator==((const void*)p); }
		bool		operator!=( const T *p ) const				{ return operator!=((const void*)p); }

		T *  		operator->()								{ return (T *)Get(); }
		T &  		operator *()								{ return *((T *)Get()); }

		const T *   operator->() const							{ return (const T *)Get(); }
		const T &   operator *() const							{ return *((const T *)Get()); }

		const T &	operator[]( int i ) const					{ return *((const T *)Get() + i); }
		T &			operator[]( int i )							{ return *((T *)Get() + i); }

	private:
		// Disallowed operations
		CThreadLocalPtr( T *pFrom );
		CThreadLocalPtr( const CThreadLocalPtr<T> &from );
		T **operator &();
		T * const *operator &() const;
		void operator=( const CThreadLocalPtr<T> &from );
		bool operator==( const CThreadLocalPtr<T> &p ) const;
		bool operator!=( const CThreadLocalPtr<T> &p ) const;
	};
}

#ifndef TIER0_DLL_EXPORT
__declspec( dllimport ) GenericThreadLocals::CThreadLocalInt<int> g_nThreadID;
#endif

#endif /// afx32
#endif //_WIN32

#endif // NO_THREAD_LOCAL

//-----------------------------------------------------------------------------
//
// Platform independent for critical sections management
//
//-----------------------------------------------------------------------------

class CAtomicMutex
{
public:
	enum class State
	{
		NONE = 0,
		Acquiring = (1 << 0),
		Acquired = (1 << 1),

		MASK = (1 << 2) - 1
	};

	CAtomicMutex( uint16 spin_iters = 200 );

	//------------------------------------------------------
	// Mutex acquisition/release. Const intentionally defeated.
	//------------------------------------------------------
	void Lock( const char *pFileName = NULL, int nLine = -1 );
	void Lock( const char *pFileName = NULL, int nLine = -1 ) const;
	void Unlock( const char *pFileName = NULL, int nLine = -1 );
	void Unlock( const char *pFileName = NULL, int nLine = -1 ) const;

	bool AssertOwnedByCurrentThread();
	void SetTrace( bool );

private:
	DLL_CLASS_IMPORT void AcquireLock( ThreadId_t threadid );

private:
	// Uses CAtomicMutex::State enum
	CInterlockedUInt m_State;
	uint16		m_LockCount;
	uint16		m_AcquireSpinIterations;
	ThreadId_t	m_CurrentOwnerID;
};

typedef CAtomicMutex CThreadSpinMutex;
typedef CThreadSpinMutex CThreadFastMutex;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

class CThreadNullMutex
{
public:
	CThreadNullMutex( const char* pDebugName = NULL );

	static void Lock( const char *pFileName = NULL, int nLine = -1 );
	static void Unlock( const char *pFileName = NULL, int nLine = -1 );

	static bool TryLock( const char *pFileName = NULL, int nLine = -1 );
	static bool AssertOwnedByCurrentThread();
	static void SetTrace( bool b );

	static ThreadId_t GetOwnerId();
	static int	GetDepth();
};

//-----------------------------------------------------------------------------
//
// A mutex decorator class used to control the use of a mutex, to make it
// less expensive when not multithreading
//
//-----------------------------------------------------------------------------

template <class BaseClass, bool *pCondition>
class CThreadConditionalMutex : public BaseClass
{
public:
	CThreadConditionalMutex( const char* pDebugName = NULL ) : BaseClass( pDebugName ) {}

	void Lock( const char *pFileName = NULL, int nLine = -1 )			{ if ( *pCondition ) BaseClass::Lock( pFileName, nLine ); }
	void Lock( const char *pFileName = NULL, int nLine = -1 ) const		{ if ( *pCondition ) BaseClass::Lock( pFileName, nLine ); }
	void Unlock( const char *pFileName = NULL, int nLine = -1 )			{ if ( *pCondition ) BaseClass::Unlock( pFileName, nLine ); }
	void Unlock( const char *pFileName = NULL, int nLine = -1 ) const	{ if ( *pCondition ) BaseClass::Unlock( pFileName, nLine ); }

	bool TryLock( const char *pFileName = NULL, int nLine = -1 )		{ if ( *pCondition ) return BaseClass::TryLock( pFileName, nLine ); else return true; }
	bool TryLock( const char *pFileName = NULL, int nLine = -1 ) const 	{ if ( *pCondition ) return BaseClass::TryLock( pFileName, nLine ); else return true; }
	bool AssertOwnedByCurrentThread()									{ if ( *pCondition ) return BaseClass::AssertOwnedByCurrentThread(); else return true; }
	void SetTrace( bool b )												{ if ( *pCondition ) BaseClass::SetTrace( b ); }
};

//-----------------------------------------------------------------------------
// Mutex decorator that blows up if another thread enters
//-----------------------------------------------------------------------------

template <class BaseClass>
class CThreadTerminalMutex : public BaseClass
{
public:
	CThreadTerminalMutex( const char* pDebugName = NULL ) : BaseClass( pDebugName ) {}

	bool TryLock( const char *pFileName = NULL, int nLine = -1 )		{ if ( !BaseClass::TryLock( pFileName, nLine ) ) { DebuggerBreak(); return false; } return true; }
	bool TryLock( const char *pFileName = NULL, int nLine = -1 ) const 	{ if ( !BaseClass::TryLock( pFileName, nLine ) ) { DebuggerBreak(); return false; } return true; }
	void Lock( const char *pFileName = NULL, int nLine = -1 )			{ if ( !TryLock( pFileName, nLine ) ) BaseClass::Lock( pFileName, nLine ); }
	void Lock( const char *pFileName = NULL, int nLine = -1 ) const		{ if ( !TryLock( pFileName, nLine ) ) BaseClass::Lock( pFileName, nLine ); }

};

//-----------------------------------------------------------------------------
//
// Class to Lock a critical section, and unlock it automatically
// when the lock goes out of scope
//
//-----------------------------------------------------------------------------

template <class MUTEX_TYPE = CAtomicMutex>
class CAutoLockT
{
public:
	FORCEINLINE CAutoLockT( MUTEX_TYPE &lock, const char *pFileName = nullptr, int nLine = 0 )
		: m_lock(lock)
	{
		m_lock.Lock( pFileName, nLine );
	}

	FORCEINLINE CAutoLockT( const MUTEX_TYPE &lock, const char *pFileName = nullptr, int nLine = 0 )
		: m_lock(const_cast<MUTEX_TYPE &>(lock))
	{
		m_lock.Lock( pFileName, nLine );
	}

	FORCEINLINE ~CAutoLockT()
	{
		m_lock.Unlock();
	}


private:
	MUTEX_TYPE &m_lock;

	// Disallow copying
	CAutoLockT( const CAutoLockT<MUTEX_TYPE> & );
	CAutoLockT &operator=( const CAutoLockT<MUTEX_TYPE> & );
};

typedef CAutoLockT<CAtomicMutex> CAutoLock;

//---------------------------------------------------------

#define AUTO_LOCK_( type, mutex ) \
	CAutoLockT< type > UNIQUE_ID( static_cast<const type &>( mutex ), __FILE__, __LINE__ )

template<typename T> T strip_cv_quals_for_mutex(T&);
template<typename T> T strip_cv_quals_for_mutex(const T&);
template<typename T> T strip_cv_quals_for_mutex(volatile T&);
template<typename T> T strip_cv_quals_for_mutex(const volatile T&);

#define AUTO_LOCK( mutex ) \
    AUTO_LOCK_( decltype(::strip_cv_quals_for_mutex(mutex)), mutex )

#define AUTO_LOCK_FM( mutex ) \
	AUTO_LOCK_( CThreadFastMutex, mutex )

#define LOCAL_THREAD_LOCK_( tag ) \
	; \
	static CThreadFastMutex autoMutex_##tag; \
	AUTO_LOCK( autoMutex_##tag )

#define LOCAL_THREAD_LOCK() \
	LOCAL_THREAD_LOCK_(_)

//-----------------------------------------------------------------------------
//
// Base class for event, semaphore and mutex objects.
//
//-----------------------------------------------------------------------------

#define TW_TIMEOUT	0xFFFF
#define TW_FAILED	0xFFFE

class PLATFORM_CLASS CThreadSyncObject
{
public:
	~CThreadSyncObject();

	//-----------------------------------------------------
	// Query if object is useful
	//-----------------------------------------------------
	bool operator!() const;

	//-----------------------------------------------------
	// Access handle
	//-----------------------------------------------------
#ifdef _WIN32
	operator HANDLE() { return GetHandle(); }
	const HANDLE GetHandle() const { return m_hSyncObject; }
#endif
	//-----------------------------------------------------
	// Wait for a signal from the object
	//-----------------------------------------------------
	bool Wait( uint32 dwTimeout = TT_INFINITE );
	bool Wait_NoDiagnostics( uint32 dwTimeout = TT_INFINITE );

	//-----------------------------------------------------
	// Wait for a signal from any of the specified objects.
	//
	// Returns the index of the object that signaled the event
	// or THREADSYNC_TIMEOUT if the timeout was hit before the wait condition was met.
	//
	// Returns TW_FAILED if an incoming object is invalid.
	//
	// If bWaitAll=true, then it'll return 0 if all the objects were set.
	//-----------------------------------------------------
	static uint32 WaitForMultiple( int nObjects, CThreadSyncObject **ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );

	// This builds a list of pointers and calls straight through to the other WaitForMultiple.
	static uint32 WaitForMultiple( int nObjects, CThreadSyncObject *ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );

protected:
	CThreadSyncObject();
	void AssertUseable();

#ifdef POSIX
	virtual bool WaitImpl( uint32 dwTimeout );
#endif

#ifdef _WIN32
	HANDLE m_hSyncObject;
	bool m_bCreatedHandle;
#elif defined(POSIX)
	char *m_pszSemName;
	int m_pSemaphore;
	bool m_bSemOwner;
	bool m_bSemAutoUndo;
	pthread_mutex_t	m_Mutex;
	pthread_cond_t	m_Condition;
	bool m_bInitalized;
	bool m_bManualReset;
	CInterlockedInt m_cSet;
	int m_nBroadcastCount;
#else
#error "Implement me"
#endif

private:
	CThreadSyncObject( const CThreadSyncObject & );
	CThreadSyncObject &operator=( const CThreadSyncObject & );
};


//-----------------------------------------------------------------------------
//
// Wrapper for unnamed event objects
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// CThreadSemaphore
//
//-----------------------------------------------------------------------------

class PLATFORM_CLASS CThreadSemaphore : public CThreadSyncObject
{
public:
	// bAllUsers lets other users open a named semaphore on Linux, and does nothing on Windows
	CThreadSemaphore(int32 initialValue, int32 maxValue, const char *pszName = NULL, bool bAllUsers = false);
	~CThreadSemaphore();

	//-----------------------------------------------------
	// Increases the count of the semaphore object by a specified
	// amount.  Wait() decreases the count by one on return.
	//-----------------------------------------------------
	bool Release(int32 releaseCount = 1, int32 * pPreviousCount = NULL );

#ifdef POSIX
protected:
	bool WaitImpl( uint32 dwTimeout ) override;
#endif

private:
	CThreadSemaphore(const CThreadSemaphore &);
	CThreadSemaphore &operator=(const CThreadSemaphore &);
};

#if defined( _WIN32 )

//-----------------------------------------------------------------------------
//
// A mutex suitable for out-of-process, multi-processor usage
//
//-----------------------------------------------------------------------------

class PLATFORM_CLASS CThreadFullMutex : public CThreadSyncObject
{
public:
	CThreadFullMutex( bool bEstablishInitialOwnership = false, const char * pszName = NULL );

	//-----------------------------------------------------
	// Release ownership of the mutex
	//-----------------------------------------------------
	bool Release();

	// To match regular CAtomicMutex:
	void Lock()							{ Wait(); }
	void Lock( unsigned timeout )		{ Wait( timeout ); }
	void Unlock()						{ Release(); }
	bool AssertOwnedByCurrentThread()	{ return true; }
	void SetTrace( bool )				{}

private:
	CThreadFullMutex( const CThreadFullMutex & );
	CThreadFullMutex &operator=( const CThreadFullMutex & );
};
#endif

enum NamedEventResult_t
{
	TT_EventDoesntExist	= 0,
	TT_EventNotSignaled,
	TT_EventSignaled
};

class PLATFORM_CLASS CThreadEvent : public CThreadSyncObject
{
public:
	CThreadEvent( bool fManualReset = false );
#ifdef _WIN32
	CThreadEvent( const char *name, bool initialState = false, bool bManualReset = false );
	static NamedEventResult_t CheckNamedEvent( const char *name, uint32 dwTimeout = 0 );
#endif
#ifdef _WIN32
	CThreadEvent( HANDLE hHandle );
#endif
	//-----------------------------------------------------
	// Set the state to signaled
	//-----------------------------------------------------
	bool Set();

	//-----------------------------------------------------
	// Set the state to nonsignaled
	//-----------------------------------------------------
	bool Reset();

	//-----------------------------------------------------
	// Check if the event is signaled
	//-----------------------------------------------------
	bool Check();

	// See CThreadSyncObject for definitions of these functions.
	static uint32 WaitForMultiple( int nObjects, CThreadEvent **ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );
	static uint32 WaitForMultiple( int nObjects, CThreadEvent *ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );

private:
	CThreadEvent( const CThreadEvent & );
	CThreadEvent &operator=( const CThreadEvent & );
};

// Hard-wired manual event for use in array declarations
class CThreadManualEvent : public CThreadEvent
{
public:
	CThreadManualEvent();
};

//-----------------------------------------------------------------------------
// A mutex shared by the processes that open it with the same name, locked with Wait()
//-----------------------------------------------------------------------------
class PLATFORM_CLASS CThreadCrossProcessMutex : public CThreadSyncObject
{
public:
	// bAllowAllUsers lets the processes of other users open it
	CThreadCrossProcessMutex( const char *pszName = NULL, bool bAllowAllUsers = false );

	bool Release();

#ifdef _WIN32
	bool TryLock( const char *pFileName, int nLine, uint32 dwTimeout );
	void Unlock( const char *pFileName = NULL, int nLine = -1 );
#else
	bool TryLock( const char *pFileName, int nLine, uint32 dwTimeout ) { return Wait( dwTimeout ); }
	void Unlock( const char *pFileName = NULL, int nLine = -1 ) { Release(); }
#endif

	bool AssertOwnedByCurrentThread() const;
	void SetTrace( bool );

private:
	CThreadCrossProcessMutex( const CThreadCrossProcessMutex & );
	CThreadCrossProcessMutex &operator=( const CThreadCrossProcessMutex & );
};

//-----------------------------------------------------------------------------
// Objects that ThreadMultiWait can wait on together
//-----------------------------------------------------------------------------
#ifdef _WIN32
typedef HANDLE ThreadMultiWaitItem_t;
#else
struct pollfd;
typedef pollfd ThreadMultiWaitItem_t;
#endif

enum ThreadMultiWaitObjectFlags_t
{
	THREAD_MULTI_WAIT_NONE = 0,
	THREAD_MULTI_WAIT_SEMAPHORE = (1 << 0),
	THREAD_MULTI_WAIT_AUTO_RESET_EVENT = (1 << 1),
	THREAD_MULTI_WAIT_MANUAL_RESET_EVENT = (1 << 2),
};

struct ThreadMultiWaitExtra_t
{
	// ThreadMultiWaitObjectFlags_t
	uint32 nFlags;
};

// A copy of the objects to wait on, made by ThreadMultiWaitObjectSetCreate
struct ThreadMultiWaitObjectSet_t
{
	ThreadMultiWaitItem_t *m_pItems;
	ThreadMultiWaitExtra_t *m_pExtra;
	int m_nItems;
#ifdef POSIX
	// The epoll instance that watches m_pItems
	int m_hEpoll;
#endif
};

class CThreadMultiWaitObject
{
public:
	DLL_CLASS_IMPORT CThreadMultiWaitObject();
	DLL_CLASS_IMPORT ~CThreadMultiWaitObject();

	// Releases a semaphore or sets an event
	DLL_CLASS_IMPORT void Signal();

protected:
#ifdef _WIN32
	HANDLE m_hEventFd;
#else
	int m_hEventFd;
#endif
	// ThreadMultiWaitObjectFlags_t
	uint32 m_nExtraFlags;
};

class CThreadMultiWaitEvent : public CThreadMultiWaitObject
{
public:
	DLL_CLASS_IMPORT CThreadMultiWaitEvent( bool bManualReset = false );
	DLL_CLASS_IMPORT ~CThreadMultiWaitEvent();

	DLL_CLASS_IMPORT void Set();
	DLL_CLASS_IMPORT void Reset();
};

class CThreadMultiWaitSemaphore : public CThreadMultiWaitObject
{
public:
	DLL_CLASS_IMPORT CThreadMultiWaitSemaphore();
	DLL_CLASS_IMPORT ~CThreadMultiWaitSemaphore();

	DLL_CLASS_IMPORT void Release();
};

// Builds a set from the objects, which must outlive it, to wait on them repeatedly
PLATFORM_INTERFACE ThreadMultiWaitObjectSet_t *ThreadMultiWaitObjectSetCreate( const CThreadMultiWaitObject **ppObjects, int nObjects );
PLATFORM_OVERLOAD ThreadMultiWaitObjectSet_t *ThreadMultiWaitObjectSetCreate( const ThreadMultiWaitItem_t *pItems, const ThreadMultiWaitExtra_t *pExtra, int nItems );
PLATFORM_INTERFACE void ThreadMultiWaitObjectSetDestroy( ThreadMultiWaitObjectSet_t *pSet );
PLATFORM_INTERFACE int ThreadMultiWaitObjectSetGetCount( ThreadMultiWaitObjectSet_t *pSet );

enum ThreadMultiWaitFlags_t
{
	THREAD_MULTI_WAIT_FOR_ANY = 0,
	// Only Windows waits for all of them; elsewhere any signalled object ends the wait
	THREAD_MULTI_WAIT_FOR_ALL = (1 << 0),
};

// Waits on pSet, or pItems and pExtra when pSet is nullptr. nFlags is a combination of ThreadMultiWaitFlags_t.
// Returns how many were signalled, with their bits set in pSignalled, 0 on timeout or -1 on failure.
PLATFORM_INTERFACE int ThreadMultiWait( ThreadMultiWaitObjectSet_t *pSet, ThreadMultiWaitItem_t *pItems, ThreadMultiWaitExtra_t *pExtra, int nItems, uint32 nFlags, uint64 *pSignalled, uint32 nMillis );

//-----------------------------------------------------------------------------
// Blocks threads until the given number of them have arrived, then lets them
// all through and starts over
//-----------------------------------------------------------------------------
class CThreadBarrier
{
public:
	DLL_CLASS_IMPORT CThreadBarrier( uint32 nThreads );
	DLL_CLASS_IMPORT ~CThreadBarrier();

	DLL_CLASS_IMPORT void ArriveAndWait();

private:
	CThreadBarrier( const CThreadBarrier & );
	CThreadBarrier &operator=( const CThreadBarrier & );

	struct BarrierState_t
	{
		uint32 m_nThreadCount;
		uint32 m_nThreadsToArrive;
		// Completed rounds, the waiting threads leave once it changes
		uint32 m_nRound;
		std::mutex m_Mutex;
		std::condition_variable m_RoundComplete;
	};

	BarrierState_t *m_pState;
};

//-----------------------------------------------------------------------------
// Lets a thread pause background work: workers wrap each piece of work in
// OnBeginBackgroundWork, which waits while paused, and OnEndBackgroundWork
//-----------------------------------------------------------------------------
class CThreadBackgroundWorkGate
{
public:
	DLL_CLASS_IMPORT CThreadBackgroundWorkGate();
	DLL_CLASS_IMPORT ~CThreadBackgroundWorkGate();

	DLL_CLASS_IMPORT void OnBeginBackgroundWork();
	DLL_CLASS_IMPORT void OnEndBackgroundWork();
	// For workers to end their work early
	DLL_CLASS_IMPORT bool PauseRequested() const;

	// Waits until no background work is running
	DLL_CLASS_IMPORT void PauseWork();
	DLL_CLASS_IMPORT void ResumeWork();

private:
	CThreadBackgroundWorkGate( const CThreadBackgroundWorkGate & );
	CThreadBackgroundWorkGate &operator=( const CThreadBackgroundWorkGate & );

	struct GateState_t
	{
		std::mutex m_Mutex;
		// Notified when the active work runs out and when work is resumed
		std::condition_variable m_StateChanged;
		int m_nActiveWork;
		bool m_bPauseRequested;
	};

	GateState_t *m_pState;
};


//-----------------------------------------------------------------------------
//
// CThreadRWLock
//
//-----------------------------------------------------------------------------

class PLATFORM_CLASS CThreadRWLock
{
public:
	CThreadRWLock();

	void LockForRead();
	void UnlockRead();
	void LockForWrite();
	void UnlockWrite();

	void LockForRead() const;
	void UnlockRead() const;
	void LockForWrite() const;
	void UnlockWrite() const;

private:
	void WaitForRead();

	CThreadFastMutex m_mutex;
	CThreadEvent m_CanWrite;
	CThreadEvent m_CanRead;

	int m_nWriters;
	int m_nActiveReaders;
	int m_nPendingReaders;
};

//-----------------------------------------------------------------------------
//
// CThreadRWLock_FastRead
//
//-----------------------------------------------------------------------------

enum RWLockTranstionResult_t
{
	// Another writer had the lock in between
	RWLTR_STATE_INVALIDATED = 0,
	RWLTR_STATE_REMAINED_VALID = 1,
};

// AMNOTE: Incomplete, tier0 doesn't export LockForRead, UnlockRead and LockForWrite
class CThreadRWLock_FastRead
{
public:
	enum WriteLockTransition_t
	{
		// AMNOTE: Blocks reads with HaveWriteLock_BlockReadsNow( true )
		TRANSITION_UNK001,
		// AMNOTE: Blocks reads with HaveWriteLock_BlockReadsNow( false )
		TRANSITION_UNK002,
		// AMNOTE: Doesn't block reads
		TRANSITION_UNK003
	};

	DLL_CLASS_IMPORT CThreadRWLock_FastRead();

	DLL_CLASS_IMPORT void UnlockWrite( const char *pFileName = NULL, int nLine = -1 );
	DLL_CLASS_IMPORT RWLockTranstionResult_t UnlockRead_LockForWrite( const char *pFileName, int nLine, WriteLockTransition_t transition );
	DLL_CLASS_IMPORT RWLockTranstionResult_t UnlockWrite_LockForRead( const char *pFileName = NULL, int nLine = -1 );
	DLL_CLASS_IMPORT bool TryUnlockRead_LockForWrite( const char *pFileName, int nLine, bool bForce, WriteLockTransition_t transition );
	DLL_CLASS_IMPORT bool TryUnlockWrite_LockForRead( const char *pFileName = NULL, int nLine = -1 );
	DLL_CLASS_IMPORT void HaveWriteLock_BlockReadsNow( bool bBlock );
	DLL_CLASS_IMPORT void HaveWriteLock_UnblockReads();

protected:
	DLL_CLASS_IMPORT bool EncounteredComplexReadLockOperation( uint32 nInterlock, bool bWait, const char *pFileName, int nLine );
	DLL_CLASS_IMPORT void UnlockReadAccounting( const char *pFileName, int nLine, uint32 nInterlock );

private:
	CInterlockedUInt m_Interlock;
	CInterlockedInt m_ReadlockAccounting;
	CThreadSemaphore m_WriteLockQueue;
	CThreadEvent m_WriteLockUnblocked;
	CThreadEvent m_ReadLockUnblocked;

	ThreadId_t m_WritingThreadID;
	int m_nWritingThreadLockCount;
};

//-----------------------------------------------------------------------------
//
// CThreadSpinRWLock
//
//-----------------------------------------------------------------------------

class CThreadSpinRWLock
{
public:
	bool TryLockForWrite( const char *pFileName = NULL, int nLine = -1 );
	bool TryLockForRead( const char *pFileName = NULL, int nLine = -1 );

	void LockForRead( const char *pFileName = NULL, int nLine = -1 );
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 );
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 );
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 );

	bool TryLockForWrite( const char *pFileName = NULL, int nLine = -1 ) const;
	bool TryLockForRead( const char *pFileName = NULL, int nLine = -1 ) const;

	void LockForRead( const char *pFileName = NULL, int nLine = -1 ) const;
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 ) const;
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 ) const;
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 ) const;

private:
	std::shared_mutex m_mutex;
};

//-----------------------------------------------------------------------------
//
// A thread wrapper similar to a Java thread.
//
//-----------------------------------------------------------------------------

enum ThreadPriorityEnum_t
{
	TP_PRIORITY_DEFAULT = 0,
	TP_PRIORITY_NORMAL = 0,
	TP_PRIORITY_HIGH = 1,
	TP_PRIORITY_LOW = -1,
	TP_PRIORITY_HIGHEST = 2,
	TP_PRIORITY_LOWEST = -2,
};

enum CThreadClassKind_t
{
	// The current thread isn't a CThread
	CTHREAD_CLASS_KIND_NONE = 0,
	CTHREAD_CLASS_KIND_THREAD = 1,
	// A CThreadPool job thread
	CTHREAD_CLASS_KIND_JOB_THREAD = 2,
};

class PLATFORM_CLASS CThread
{
public:
	CThread();
	virtual ~CThread();

	//-----------------------------------------------------

	const char *GetName();
	void SetName( const char * );

	//-----------------------------------------------------
	// Functions for the other threads
	//-----------------------------------------------------

	// Start thread running  - error if already running
	virtual bool Start( uint32 nBytesStack = 0, ThreadPriorityEnum_t nPriority = TP_PRIORITY_DEFAULT );

	// Returns true if thread has been created and hasn't yet exited
	bool IsAlive();

	// This method causes the current thread to wait until this thread
	// is no longer alive. The timeout is ignored, it always waits for the thread to exit.
	bool Join( uint32 timeout = TT_INFINITE );

	// Access the thread handle directly
	ThreadHandle_t GetThreadHandle();

#ifdef _WIN32
	uint32 GetThreadID() const;
#endif

	//-----------------------------------------------------

	int GetResult();

	//-----------------------------------------------------
	// Functions for both this, and maybe, and other threads
	//-----------------------------------------------------

	// Forcibly, abnormally, but relatively cleanly stop the thread
	void Stop( int exitCode = 0 );

#ifdef _WIN32
	// Get the priority
	ThreadPriorityEnum_t GetPriority() const;

	// Set the priority
	bool SetPriority( ThreadPriorityEnum_t nPriority );
#endif

	// Suspend the thread, called from the thread itself. Returns 0.
	uint32 Suspend();

	// Resume a suspended thread. Returns 0.
	uint32 Resume();

	bool IsSuspended();

	// Force hard-termination of thread.  Used for critical failures.
	bool Terminate( int exitCode = 0 );

	CThreadClassKind_t GetClassKind() const;

	//-----------------------------------------------------
	// Global methods
	//-----------------------------------------------------

	// Get the Thread object that represents the current thread, if any.
	// Can return NULL if the current thread was not created using
	// CThread
	static CThread *GetCurrentCThread();
	static CThreadClassKind_t GetCurrentCThreadClassKind();

	// Offer a context switch. Under Win32, equivalent to Sleep(0)
#ifdef Yield
#undef Yield
#endif
	static void Yield();

	// This method causes the current thread to yield and not to be
	// scheduled for further execution until a certain amount of real
	// time has elapsed, more or less. Duration is in milliseconds
	static void Sleep( uint32 duration );

protected:
	// Thread will run this function on startup, must be supplied by
	// derived class, performs the intended action of the thread.
	virtual int Run() = 0;

	// Called when the thread exits
	virtual void OnExit();

	// Allow for custom start waiting
	virtual bool WaitForCreateComplete( CThreadEvent *pEvent );

	virtual bool IsThreadRunning();

	ThreadHandle_t GetThreadHandle() const;

	// Runs Run() and OnExit() on the new thread, which pv gives with the priority to set
	static uintp ThreadProc( void *pv );

	CAtomicMutex m_Lock;
#ifdef POSIX
	CThreadEvent m_ExitEvent;	// Set right before the thread's function exits.
#endif

private:
	enum Flags
	{
		SUPPORT_STOP_PROTOCOL = 1 << 0
	};

	// make copy constructor and assignment operator inaccessible
	CThread( const CThread & );
	CThread &operator=( const CThread & );

	ThreadHandle_t m_hThread;
#ifdef _WIN32
	ThreadId_t m_threadId;
#endif
	int		m_result;
	bool	m_bAlive;
	char	m_szName[32];
	uint32	m_flags;
	CThreadClassKind_t m_ClassKind;
	CThreadManualEvent m_NotSuspendedEvent;
};

//-----------------------------------------------------------------------------
// Simple thread class encompasses the notion of a worker thread, handing
// synchronized communication.
//-----------------------------------------------------------------------------

// These are internal reserved error results from a call attempt
enum WTCallResult_t
{
	WTCR_FAIL			= -1,
	WTCR_TIMEOUT		= -2,
	WTCR_THREAD_GONE	= -3,
};

class PLATFORM_CLASS CWorkerThread : public CThread
{
public:
	CWorkerThread();

	//-----------------------------------------------------
	//
	// Inter-thread communication
	//
	// Calls in either direction take place on the same "channel."
	// Seperate functions are specified to make identities obvious
	//
	//-----------------------------------------------------

	// Master: Signal the thread, and block for a response
	int CallWorker( unsigned, unsigned timeout = TT_INFINITE, bool fBoostWorkerPriorityToMaster = true );

	// Worker: Signal the thread, and block for a response
	int CallMaster( unsigned, unsigned timeout = TT_INFINITE );

	// Wait for the next request
	bool WaitForCall( unsigned dwTimeout, unsigned *pResult = NULL );
	bool WaitForCall( unsigned *pResult = NULL );

	// Is there a request?
	bool PeekCall( unsigned *pParam = NULL );

	// Reply to the request
	void Reply( unsigned );

	// Wait for a reply in the case when CallWorker() with timeout != TT_INFINITE
	int WaitForReply( unsigned timeout = TT_INFINITE );

	// If you want to do WaitForMultipleObjects you'll need to include
	// this handle in your wait list or you won't be responsive
	CThreadEvent& GetCallHandle();	// (returns m_EventSend)

	// Find out what the request was
	unsigned GetCallParam() const;

	// Boost the worker thread to the master thread, if worker thread is lesser, return old priority
	int BoostPriority();

protected:
	int Call( unsigned, unsigned timeout, bool fBoost );

private:
	CWorkerThread( const CWorkerThread & );
	CWorkerThread &operator=( const CWorkerThread & );

	CThreadEvent	m_EventSend;
	CThreadEvent	m_EventComplete;

	unsigned        m_Param;
	int				m_ReturnVal;
};


// a unidirectional message queue. A queue of type T. Not especially high speed since each message
// is malloced/freed. Note that if your message class has destructors/constructors, they MUST be
// thread safe!
template<class T> class CMessageQueue
{
	CThreadEvent SignalEvent;								// signals presence of data
	CAtomicMutex QueueAccessMutex;

	// the parts protected by the mutex
	struct MsgNode
	{
		MsgNode *Next;
		T Data;
	};

	MsgNode *Head;
	MsgNode *Tail;

public:
	CMessageQueue( void )
	{
		Head = Tail = NULL;
	}

	// check for a message. not 100% reliable - someone could grab the message first
	bool MessageWaiting( void )
	{
		return ( Head != NULL );
	}

	void WaitMessage( T *pMsg )
	{
		for(;;)
		{
			while( ! MessageWaiting() )
				SignalEvent.Wait();
			QueueAccessMutex.Lock( __FILE__, __LINE__ );
			if (! Head )
			{
				// multiple readers could make this null
				QueueAccessMutex.Unlock( __FILE__, __LINE__ );
				continue;
			}
			*( pMsg ) = Head->Data;
			MsgNode *remove_this = Head;
			Head = Head->Next;
			if (! Head)										// if empty, fix tail ptr
				Tail = NULL;
			QueueAccessMutex.Unlock( __FILE__, __LINE__ );
			delete remove_this;
			break;
		}
	}

	void QueueMessage( T const &Msg)
	{
		MsgNode *new1=new MsgNode;
		new1->Data=Msg;
		new1->Next=NULL;
		QueueAccessMutex.Lock( __FILE__, __LINE__ );
		if ( Tail )
		{
			Tail->Next=new1;
			Tail = new1;
		}
		else
		{
			Head = new1;
			Tail = new1;
		}
		SignalEvent.Set();
		QueueAccessMutex.Unlock( __FILE__, __LINE__ );
	}
};


//-----------------------------------------------------------------------------
//
// CThreadRWLock inline functions
//
//-----------------------------------------------------------------------------




// read data from a memory address
template<class T> FORCEINLINE T ReadVolatileMemory( T const *pPtr )
{
	volatile const T * pVolatilePtr = ( volatile const T * ) pPtr;
	return *pVolatilePtr;
}

//-----------------------------------------------------------------------------

#if defined( POSIX )
DLL_GLOBAL_IMPORT __thread int g_nThreadID;
#endif

#if defined( _WIN32 )
#pragma warning(pop)
#endif

#endif // _TIER0_THREADTOOLS_H
