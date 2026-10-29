#include "public/tier0/fibertools.h"

CEventListNode::CEventListNode()
: m_pNext( NULL ), m_pUserData( NULL )
{}

CEventList::CEventList()
: m_pTail( (CEventListNode *)&m_pHead ), m_pHead( NULL )
{}

uint32 CFiber::AddRef()
{ return ++m_nRefCount; }

uint32 CFiber::Release()
{
	uint32 nRefCount = --m_nRefCount;
	if ( nRefCount == 0 )
		ReachedZeroReferences();
	return nRefCount;
}

void * CFiber::SetFiberJob( FiberJobFunc_t pJob, void *pContext, size_t nCopyContextParamSize, bool bJobAcceptsInitialResume )
{
	return InternalSetFiberJob( pJob, pContext, nCopyContextParamSize, bJobAcceptsInitialResume );
}

void * CFiber::GetFiberJobContext() const
{ return m_pCurrentJobContext; }

bool CFiber::HasAJob() const
{ return m_pCurrentJobFunc != NULL; }

bool CFiber::IsThreadFiber() const
{ return ( m_nFlags & IS_THREAD_FIBER ) != 0; }

bool CFiber::IsMemoryInFiberRange( const void *p ) const
{ return m_StackMemoryRange.Contains( p ); }

CFiber * CFiber::GetPrevFiberInSameThread() const
{ return m_pPrevFiberInSameThread; }

StackContext_StartData_t * CFiber::GetStackContextStartData() const
{ return m_pStackContextStartData; }

const MemoryRange_t & CFiber::GetStackMemoryRange() const
{ return m_StackMemoryRange; }

ThreadId_t CFiber::GetJobOriginatingThreadId() const
{ return m_hJobsOriginatingThreadId; }

CThreadFiber::~CThreadFiber()
{
	if ( m_pFiber )
		m_pFiber->Release();
}
