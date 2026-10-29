#ifndef FIBERTOOLS_H
#define FIBERTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "threadtools.h"
#include "stackcontext.h"

class CFiber;

typedef void (*FiberJobFunc_t)( void *pContext );
typedef bool (*FiberResumeHintFunc_t)( void *pContext );
typedef intp (*FiberRunFunc_t)( void *pContext );

//-----------------------------------------------------------------------------
// An event that can be queued on a CEventList
//-----------------------------------------------------------------------------
class CEventListNode
{
public:
	CEventListNode();

	// Takes a node from a shared pool, give it back with FreeTempNode()
	DLL_CLASS_IMPORT static CEventListNode *AllocTempNode();
	DLL_CLASS_IMPORT static void FreeTempNode( CEventListNode *pNode );

	CEventListNode *m_pNext;
	// Data for the owner to keep with the node, tier0 only clears it
	void *m_pUserData;
	CThreadManualEvent m_Event;
};

//-----------------------------------------------------------------------------
// A lock free queue of event nodes
//-----------------------------------------------------------------------------
class CEventList
{
public:
	CEventList();

private:
	// Points at the last node, or at m_pHead when the list is empty
	CEventListNode *volatile m_pTail;
	CEventListNode *volatile m_pHead;
};

/*
	CFiber runs a job on its own small stack. The job can suspend itself with FiberYield(), and
	ResumeJob() continues it later from wherever it's called, so a job can wait for something
	without blocking a thread. ResumeJob() returns true once the job returned.

	Example usage:

	Running a job on the game thread over several frames:

		static bool IsTimeReached( void *pContext )
		{
			return Plat_FloatTime() >= *(double *)pContext;
		}

		static void MyJob( void *pContext )
		{
			// SetFiberJob() copied the context onto the fiber's stack
			MyJobContext_t *pJob = (MyJobContext_t *)pContext;
			...
			// Suspends the job for a second, IsReadyToResume() returns false until then
			double flResumeTime = Plat_FloatTime() + 1.0;
			CFiber::GetCurrentFiber()->FiberYield( IsTimeReached, &flResumeTime );
			...
		}

		CFiber *pFiber = CFiber::AllocPooledFiber( CFiber::STACKSIZE_64KB, 0 );
		pFiber->SetFiberJob( MyJob, &context, sizeof( context ) );

		// Every frame, until the job returns
		if ( pFiber->IsReadyToResume() && pFiber->ResumeJob( nullptr ) )
			pFiber->Release();

	Releasing a fiber whose job didn't return doesn't unwind its stack, so the destructors there never run.

	Running a job on the engine's thread pool and waiting for it:

		pFiber->AddRef();
		// Takes the reference of AllocPooledFiber() and releases it once the job returned
		g_pFiberJobPool->AddFiberToJobPool( pFiber, true );

		CEventListNode *pNode = CEventListNode::AllocTempNode();
		pFiber->WaitForJobCompletion( *pNode );
		CEventListNode::FreeTempNode( pNode );
		pFiber->Release();

	These jobs run on other threads, so they must not touch the game state.

	Running the jobs of the pool on the current thread while waiting:

		CThreadFiber threadFiber;
		// Returns once IsDone( pContext ) returns true
		CFiber::GetCurrentFiber()->FiberYield( IsDone, pContext );

	How a job runs:

	The first ResumeJob() calls the job on the fiber's stack. FiberYield() keeps the job's locals
	and call chain on that stack and returns to the caller of ResumeJob(), which returns false. The
	next ResumeJob() continues after the FiberYield(), which returns the value given to ResumeJob().

	IsReadyToResume() asks the resume hint given to FiberYield(), so the job is only resumed once
	what it waits for is done. A job that didn't start yet is always ready. Once the job returns,
	ResumeJob() returns true and WaitForJobCompletion() stops waiting.

	A job resumed by the job pool can continue on a different thread each time. Off a fiber, on a
	CThreadFiber, FiberYield() doesn't suspend anything and runs the jobs of the pool instead.
*/
class CFiber
{
public:
	enum SizeBuckets_t
	{
		STACKSIZE_NONE,
		STACKSIZE_2KB,
		STACKSIZE_4KB,
		STACKSIZE_8KB,
		STACKSIZE_16KB,
		STACKSIZE_32KB,
		STACKSIZE_64KB,
		STACKSIZE_128KB,
	};

	enum Flags_t
	{
		IS_THREAD_FIBER = (1 << 0),
		HAS_MEMORY_SENTINEL = (1 << 1),
		IS_POOLED_FIBER = (1 << 2),
		IS_USING_POOL_MEMORY = (1 << 3),
		DELETE_MEMORY_ON_ZERO_REFCOUNT = (1 << 4),
		CURRENT_JOB_HAS_BEEN_IN_JOB_POOL_BEFORE = (1 << 5),
		JOB_LOCK = (1 << 6),
		JOB_ACCEPTS_INITIAL_RESUME = (1 << 7),
		// AbortJob() resumed the job to make it stop, SetFiberJob() clears it
		JOB_ABORT_REQUESTED = (1 << 8),
	};

	DLL_CLASS_IMPORT CFiber();
	DLL_CLASS_IMPORT ~CFiber();

	DLL_CLASS_IMPORT static CFiber *AllocPooledFiber( SizeBuckets_t nStackMemorySize, size_t nClearToFFEEFFEE_DWORD_Interval );

	uint32 AddRef();
	uint32 Release();

	DLL_CLASS_IMPORT void InitStackContext( void *pStackMemory, size_t nStackMemorySize, bool bDeleteMemoryOnZeroRefCount, size_t nClearToFFEEFFEE_Interval );
	DLL_CLASS_IMPORT void *DetachFromStackContext();
	DLL_CLASS_IMPORT size_t EstimateBytesUsed( size_t nTestDWORDInterval ) const;

	void *SetFiberJob( FiberJobFunc_t pJob, void *pContext, size_t nCopyContextParamSize = 0, bool bJobAcceptsInitialResume = false );
	void *GetFiberJobContext() const;
	bool HasAJob() const;

	DLL_CLASS_IMPORT bool IsReadyToResume();
	DLL_CLASS_IMPORT bool ResumeJob( void *pYieldRetVal );
	DLL_CLASS_IMPORT bool AbortJob( void * );
	// pFnResumeHint tells the job pool when the fiber is worth resuming
	DLL_CLASS_IMPORT void *FiberYield( FiberResumeHintFunc_t pFnResumeHint, void *pFnResumeHintContext );

	DLL_CLASS_IMPORT static CFiber *GetCurrentFiber();
	// Returns the previous fiber
	DLL_CLASS_IMPORT static CFiber *SetCurrentFiber( CFiber *pNew );

	bool IsThreadFiber() const;
	bool IsMemoryInFiberRange( const void *p ) const;

	DLL_CLASS_IMPORT static bool IsRunningInThreadMemory();
	// Runs pFnRun on the thread's own stack when called from a fiber
	DLL_CLASS_IMPORT static intp RunInThreadMemory( FiberRunFunc_t pFnRun, void *pContext );

	DLL_CLASS_IMPORT void WaitForJobCompletion( CEventListNode &waitEventNode );

	CFiber *GetPrevFiberInSameThread() const;
	StackContext_StartData_t *GetStackContextStartData() const;
	const MemoryRange_t &GetStackMemoryRange() const;
	ThreadId_t GetJobOriginatingThreadId() const;

protected:
	DLL_CLASS_IMPORT void ConstructThreadFiber( size_t nCreatedThreadSize );
	DLL_CLASS_IMPORT void SetThreadFiberMemoryRange( size_t nCreatedThreadSize );
	DLL_CLASS_IMPORT void ClearUnusedSpaceToFFEEFFEE( void *pHigh );

private:
	DLL_CLASS_IMPORT void ReachedZeroReferences();
	DLL_CLASS_IMPORT void *InternalSetFiberJob( FiberJobFunc_t pJob, void *pContext, size_t nCopyContextParamSize, bool bJobAcceptsInitialResume );

public:
	CFiber *volatile m_pNext;
	CEventListNode m_EventListNode;

protected:
	JmpBuf_AlternateStack_t *volatile m_pJmpBuf_IN;
	JmpBuf_AlternateStack_t *volatile m_pJmpBuf_OUT;
	StackContext_StartData_t *m_pStackContextStartData;
	volatile FiberJobFunc_t m_pCurrentJobFunc;
	void *volatile m_pCurrentJobContext;
	FiberResumeHintFunc_t m_pResumeHint;
	void *m_pResumeHintContext;
	CEventList m_NotifyOnComplete;
	MemoryRange_t m_StackMemoryRange;
	CInterlockedUInt m_nFlags;
	CInterlockedUInt m_nRefCount;
	CFiber *m_pPrevFiberInSameThread;
	ThreadId_t m_hJobsOriginatingThreadId;
	size_t m_nClearedMemoryInterval;

public:
	void *m_unk001;

private:
	CFiber( const CFiber & );
	CFiber &operator=( const CFiber & );
};

//-----------------------------------------------------------------------------
// Makes the current thread a fiber for the scope, if it isn't one already
//-----------------------------------------------------------------------------
class CThreadFiber
{
public:
	DLL_CLASS_IMPORT CThreadFiber( size_t nCreatedThreadSize = 0 );
	~CThreadFiber();

private:
	CFiber *m_pFiber;

	CThreadFiber( const CThreadFiber & );
	CThreadFiber &operator=( const CThreadFiber & );
};

abstract_class IFiberThreadWorkerPool
{
public:
	virtual void FiberJobAvailable() = 0;
	// Wakes the worker up and asks it to stop, without waiting for it
	virtual void RequestStop() = 0;
};

abstract_class IFiberJobPool
{
public:
	virtual void AddFiberToJobPool( CFiber *pFiber, bool bAssumeCallersReference ) = 0;
	virtual CFiber *GetWaitingFiberJob( bool *pHasJobsThatArentReadyToResume ) = 0;
	virtual bool RemoveFiberJob( volatile CFiber *pFiber ) = 0;

	virtual void RegisterWorkerThreadPool( IFiberThreadWorkerPool *pThreadPool ) = 0;
	virtual void UnregisterWorkerThreadPool( IFiberThreadWorkerPool *pThreadPool ) = 0;

	// Unregisters every worker thread pool and calls RequestStop() on it
	virtual void StopWorkerThreadPools() = 0;
};

PLATFORM_INTERFACE IFiberJobPool *g_pFiberJobPool;

#endif // FIBERTOOLS_H
