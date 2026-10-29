#include "public/tier0/jobthread.h"

IMultipleWorkerJob::~IMultipleWorkerJob()
{}

IThreadPool::~IThreadPool()
{}

CThreadedJob::CThreadedJob( JobPriority_t priority )
: m_status( JOB_STATUS_UNSERVICED ),
	m_flags( 0 ),
	m_priority( (uint8)priority ),
	m_bHasDescription( false ),
	m_pThreadPool( NULL ),
	m_pCompleteEvent( NULL ),
	m_pWaitCounter( NULL )
{
	m_szDescription[0] = '\0';
}

CThreadedJob::~CThreadedJob()
{
	if ( m_pCompleteEvent )
	{
		ReturnJobManualEventToPool( m_pCompleteEvent );
		m_pCompleteEvent = NULL;
	}
	m_pWaitCounter = NULL;
}

void CThreadedJob::SetPriority( JobPriority_t priority )
{ m_priority = (uint8)priority; }

JobPriority_t CThreadedJob::GetPriority() const
{ return (JobPriority_t)m_priority; }

void CThreadedJob::SetFlags( unsigned flags )
{ m_flags = (uint8)flags; }

unsigned CThreadedJob::GetFlags() const
{ return m_flags; }

bool CThreadedJob::Executed() const
{ return ( m_status == JOB_OK );	}

bool CThreadedJob::CanExecute() const
{ return ( m_status == JOB_STATUS_PENDING || m_status == JOB_STATUS_UNSERVICED ); }

bool CThreadedJob::IsFinished() const
{ return ( m_status != JOB_STATUS_PENDING && m_status != JOB_STATUS_INPROGRESS && m_status != JOB_STATUS_UNSERVICED ); }

JobStatus_t CThreadedJob::GetStatus() const
{ return m_status; }

JobStatus_t CThreadedJob::Execute()
{
	if ( IsFinished() )
	{
		return m_status;
	}

	AddRef();

	WaitOrImmediatelyExecute();
	JobStatus_t result = m_status;

	Release();

	return result;
}

JobStatus_t CThreadedJob::ExecuteAndRelease()
{
	JobStatus_t status = Execute();
	Release();
	return status;
}

JobStatus_t CThreadedJob::TryExecute()
{
	JobStatus_t status = (JobStatus_t)ThreadInterlockedCompareExchange( (int32 volatile *)&m_status, JOB_STATUS_INPROGRESS, JOB_STATUS_PENDING );
	if ( status != JOB_STATUS_PENDING )
		return status;

	DoExecute();
	DoCleanup();
	ThreadInterlockedExchange( (int32 volatile *)&m_status, JOB_OK );

	if ( m_pWaitCounter )
	{
		ThreadInterlockedIncrement( (int32 volatile *)m_pWaitCounter );
		ThreadAtomicNotifyAll( (const uint32 *)m_pWaitCounter );
	}

	if ( m_pCompleteEvent )
		m_pCompleteEvent->Set();

	return JOB_OK;
}

JobStatus_t CThreadedJob::TryExecuteAndRelease()
{
	JobStatus_t status = TryExecute();
	Release();
	return status;
}

char const * CThreadedJob::Describe()
{ return m_szDescription[0] ? m_szDescription : "Job"; }

void CThreadedJob::SetDescription( const char *pszDescription )
{
	if ( pszDescription )
	{
		V_strncpy( m_szDescription, pszDescription, sizeof( m_szDescription ) );
		m_bHasDescription = true;
	}
	else
	{
		m_szDescription[0] = '\0';
	}
}

void CThreadedJob::DoCleanup()
{}

bool CThreadedDependentJob::unk001( uint32 )
{ return false; }

void CThreadedJobWithDependencies::OnPrepareDependencies()
{}

void CThreadedJobWithDependencies::DoExecute()
{ DoExecuteInternal(); }

void CThreadedJobWithDependencies::DoCleanup()
{
	m_UpstreamJobs.RemoveAll();
	m_nPendingUpstreamJobs = 0;
	m_DownstreamJobs.RemoveAll();
}

CParallelProcessorAbstract_Base::CParallelProcessorAbstract_Base( IThreadPool *pThreadPool )
{
	m_pThreadPool = pThreadPool ? pThreadPool : g_pThreadPool;
	m_nThreads = 0;
	if ( m_pThreadPool )
	{
		m_nThreads = m_pThreadPool->NumThreads();

		// A pool thread running this is busy with it
		if ( m_pThreadPool->GetCurrentThreadIndex() >= 0 )
			m_nThreads--;

		if ( m_nThreads < 0 )
			m_nThreads = 0;
	}
}
