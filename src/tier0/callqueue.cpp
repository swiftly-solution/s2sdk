#include "public/tier0/callqueue.h"

CCallQueue::CCallQueue()
: m_nRefCount( 0 ),
	m_bNoQueue( false ),
	m_bAbortProcessing( false ),
	m_bProcessing( false )
{
}

CCallQueue::~CCallQueue()
{
	Flush();
}

int CCallQueue::Count()
{
	return m_queue.Count();
}

void CCallQueue::CallQueued()
{
	m_bProcessing = true;

	if ( m_queue.Count() )
	{
		m_queue.PushItem( NULL );

		std::function<void()> *pFunctor;

		while ( m_queue.PopItem( &pFunctor ) && pFunctor != NULL )
		{
			if ( !m_bAbortProcessing )
			{
				(*pFunctor)();
			}
			delete pFunctor;
		}
	}

	m_bAbortProcessing = false;
	m_bProcessing = false;
}

void CCallQueue::Flush()
{
	m_queue.PushItem( NULL );

	std::function<void()> *pFunctor;

	while ( m_queue.PopItem( &pFunctor ) && pFunctor != NULL )
	{
		delete pFunctor;
	}

	m_bAbortProcessing = false;
}
