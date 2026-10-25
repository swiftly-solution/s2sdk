#ifndef _TIER0_THREADTOOLS_H
#define _TIER0_THREADTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "threadinterlocks.h"

#include <shared_mutex>

#ifdef _WIN32
	typedef uint32_t ThreadId_t;
#else
	typedef uint64_t ThreadId_t;
#endif

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

	CAtomicMutex( uint16_t spin_iters = 200 ) :
		m_LockCount( 0 ),
		m_AcquireSpinIterations( spin_iters ),
		m_CurrentOwnerID( 0 )
	{}

	//------------------------------------------------------
	// Mutex acquisition/release. Const intentionally defeated.
	//------------------------------------------------------
	void Lock( const char *pFileName = nullptr, int nLine = -1 );
	void Lock( const char *pFileName = nullptr, int nLine = -1 ) const { (const_cast<CAtomicMutex *>(this))->Lock( pFileName, nLine ); }
	void Unlock( const char *pFileName = nullptr, int nLine = -1 );
	void Unlock( const char *pFileName = nullptr, int nLine = -1 ) const { (const_cast<CAtomicMutex *>(this))->Unlock( pFileName, nLine ); }

	bool AssertOwnedByCurrentThread();
	void SetTrace( bool ) {}

private:
	DLL_CLASS_IMPORT void AcquireLock( ThreadId_t threadid );

private:
	CInterlockedUInt    m_State;
	uint16_t		    m_LockCount;
	uint16_t		    m_AcquireSpinIterations;
	ThreadId_t	        m_CurrentOwnerID;
};

typedef CAtomicMutex CThreadSpinMutex;
typedef CThreadSpinMutex CThreadFastMutex;

class CThreadSpinRWLock
{
public:
	bool TryLockForWrite( const char *pFileName = NULL, int nLine = -1 ) { return m_mutex.try_lock(); }
	bool TryLockForRead( const char *pFileName = NULL, int nLine = -1 ) { return m_mutex.try_lock_shared(); }

	void LockForRead( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.lock_shared(); }
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.unlock_shared(); }
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.lock(); }
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.unlock(); }

	bool TryLockForWrite( const char *pFileName = NULL, int nLine = -1 ) const { return const_cast<CThreadSpinRWLock *>(this)->TryLockForWrite( pFileName, nLine ); }
	bool TryLockForRead( const char *pFileName = NULL, int nLine = -1 ) const { return const_cast<CThreadSpinRWLock *>(this)->TryLockForRead( pFileName, nLine ); }

	void LockForRead( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->LockForRead( pFileName, nLine ); }
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->UnlockRead( pFileName, nLine ); }
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->LockForWrite( pFileName, nLine ); }
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->UnlockWrite( pFileName, nLine ); }

private:
	std::shared_mutex m_mutex;
};

template <class MUTEX_TYPE = CAtomicMutex>
class CAutoLockT
{
public:
	CAutoLockT( MUTEX_TYPE &lock, const char *pFileName = nullptr, int nLine = 0 )
		: m_lock(lock)
	{
		m_lock.Lock( pFileName, nLine );
	}

	CAutoLockT( const MUTEX_TYPE &lock, const char *pFileName = nullptr, int nLine = 0 )
		: m_lock(const_cast<MUTEX_TYPE &>(lock))
	{
		m_lock.Lock( pFileName, nLine );
	}

	~CAutoLockT()
	{
		m_lock.Unlock();
	}


private:
	MUTEX_TYPE &m_lock;

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

#endif