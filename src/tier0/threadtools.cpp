#include "public/tier0/threadtools.h"

//-----------------------------------------------------------------------------
//
// CAtomicMutex. Lock and Unlock stay out of line to keep the lock state private
//
//-----------------------------------------------------------------------------

CAtomicMutex::CAtomicMutex( uint16 spin_iters ) :
	m_LockCount( 0 ),
	m_AcquireSpinIterations( spin_iters ),
	m_CurrentOwnerID( 0 )
{
}

void CAtomicMutex::Lock( const char *pFileName, int nLine )
{
	ThreadId_t thisThreadID = ThreadGetCurrentId();

	if(m_CurrentOwnerID == thisThreadID)
	{
		m_LockCount++;
	}
	else if(m_State || m_State.CompareExchange( (uint32)State::MASK, 0 ))
	{
		AcquireLock( thisThreadID );
	}
	else
	{
		m_LockCount = 1;
		m_CurrentOwnerID = thisThreadID;
	}
}

void CAtomicMutex::Lock( const char *pFileName, int nLine ) const
{
	(const_cast<CAtomicMutex *>(this))->Lock( pFileName, nLine );
}

void CAtomicMutex::Unlock( const char *pFileName, int nLine )
{
	m_LockCount--;
	if(m_LockCount == 0)
	{
		m_CurrentOwnerID = 0;
		if(m_State.AtomicAdd(0u - (uint32)State::MASK) != (uint32)State::MASK)
			ThreadAtomicNotifyOne( (uint32 *)&m_State );
	}
}

void CAtomicMutex::Unlock( const char *pFileName, int nLine ) const
{
	(const_cast<CAtomicMutex *>(this))->Unlock( pFileName, nLine );
}

bool CAtomicMutex::AssertOwnedByCurrentThread()
{
	return ThreadGetCurrentId() == m_CurrentOwnerID;
}

//-----------------------------------------------------------------------------
//
// CThreadNullMutex. Every call does nothing
//
//-----------------------------------------------------------------------------

CThreadNullMutex::CThreadNullMutex( const char *pDebugName )
{
}

void CThreadNullMutex::Lock( const char *pFileName, int nLine )
{
}

void CThreadNullMutex::Unlock( const char *pFileName, int nLine )
{
}

bool CThreadNullMutex::TryLock( const char *pFileName, int nLine )
{
	return true;
}

bool CThreadNullMutex::AssertOwnedByCurrentThread()
{
	return true;
}

void CThreadNullMutex::SetTrace( bool b )
{
}

ThreadId_t CThreadNullMutex::GetOwnerId()
{
	return 0;
}

int CThreadNullMutex::GetDepth()
{
	return 0;
}

//-----------------------------------------------------------------------------
//
// CThreadSpinRWLock. Wraps std::shared_mutex, so it has no tier0 side
//
//-----------------------------------------------------------------------------

bool CThreadSpinRWLock::TryLockForWrite( const char *pFileName, int nLine )
{
	return m_mutex.try_lock();
}

bool CThreadSpinRWLock::TryLockForRead( const char *pFileName, int nLine )
{
	return m_mutex.try_lock_shared();
}

void CThreadSpinRWLock::LockForRead( const char *pFileName, int nLine )
{
	m_mutex.lock_shared();
}

void CThreadSpinRWLock::UnlockRead( const char *pFileName, int nLine )
{
	m_mutex.unlock_shared();
}

void CThreadSpinRWLock::LockForWrite( const char *pFileName, int nLine )
{
	m_mutex.lock();
}

void CThreadSpinRWLock::UnlockWrite( const char *pFileName, int nLine )
{
	m_mutex.unlock();
}

bool CThreadSpinRWLock::TryLockForWrite( const char *pFileName, int nLine ) const
{
	return const_cast<CThreadSpinRWLock *>(this)->TryLockForWrite( pFileName, nLine );
}

bool CThreadSpinRWLock::TryLockForRead( const char *pFileName, int nLine ) const
{
	return const_cast<CThreadSpinRWLock *>(this)->TryLockForRead( pFileName, nLine );
}

void CThreadSpinRWLock::LockForRead( const char *pFileName, int nLine ) const
{
	const_cast<CThreadSpinRWLock *>(this)->LockForRead( pFileName, nLine );
}

void CThreadSpinRWLock::UnlockRead( const char *pFileName, int nLine ) const
{
	const_cast<CThreadSpinRWLock *>(this)->UnlockRead( pFileName, nLine );
}

void CThreadSpinRWLock::LockForWrite( const char *pFileName, int nLine ) const
{
	const_cast<CThreadSpinRWLock *>(this)->LockForWrite( pFileName, nLine );
}

void CThreadSpinRWLock::UnlockWrite( const char *pFileName, int nLine ) const
{
	const_cast<CThreadSpinRWLock *>(this)->UnlockWrite( pFileName, nLine );
}

//-----------------------------------------------------------------------------
//
// Free thread helpers
//
//-----------------------------------------------------------------------------

CThreadManualEvent::CThreadManualEvent()
 :	CThreadEvent( true )
{
}

void ThreadPause()
{
#if defined( _WIN32 )
	// Intrinsic for __asm pause; from <intrin.h>
	_mm_pause();
#elif defined( POSIX )
	__asm __volatile( "pause" );
#else
#error "implement me"
#endif
}

bool ThreadSetPriority( int priority )
{
	return ThreadSetPriority( NULL, priority );
}

void ThreadSetDebugName( const char *pszName )
{
	ThreadSetDebugNameS2( NULL, pszName );
}

void CAtomicMutex:: SetTrace( bool )
{}

bool CThreadCrossProcessMutex:: AssertOwnedByCurrentThread() const
{ return true; }

void CThreadCrossProcessMutex:: SetTrace( bool )
{}

void CThreadRWLock:: LockForRead() const
{ const_cast<CThreadRWLock *>(this)->LockForRead(); }

void CThreadRWLock:: UnlockRead() const
{ const_cast<CThreadRWLock *>(this)->UnlockRead(); }

void CThreadRWLock:: LockForWrite() const
{ const_cast<CThreadRWLock *>(this)->LockForWrite(); }

void CThreadRWLock:: UnlockWrite() const
{ const_cast<CThreadRWLock *>(this)->UnlockWrite(); }

bool CThread:: IsSuspended()
{ return !m_NotSuspendedEvent.Check(); }

CThreadClassKind_t CThread:: GetClassKind() const
{ return m_ClassKind; }

void CThread:: Yield()
{ ThreadYield(); }

ThreadHandle_t CThread:: GetThreadHandle() const
{ return m_hThread; }

CThreadRWLock::CThreadRWLock()
:	m_CanRead( true ),
	m_nWriters( 0 ),
	m_nActiveReaders( 0 ),
	m_nPendingReaders( 0 )
{
}

void CThreadRWLock::LockForRead()
{
	m_mutex.Lock( __FILE__, __LINE__ );
	if ( m_nWriters)
	{
		WaitForRead();
	}
	m_nActiveReaders++;
	m_mutex.Unlock( __FILE__, __LINE__ );
}

void CThreadRWLock::UnlockRead()
{
	m_mutex.Lock( __FILE__, __LINE__ );
	m_nActiveReaders--;
	if ( m_nActiveReaders == 0 && m_nWriters != 0 )
	{
		m_CanWrite.Set();
	}
	m_mutex.Unlock( __FILE__, __LINE__ );
}
