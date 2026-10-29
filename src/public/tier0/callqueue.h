//========== Copyright © 2006, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

#ifndef CALLQUEUE_H
#define CALLQUEUE_H

#include <functional>
#include "tslist.h"
#include "jobthread.h"

#if defined( _WIN32 )
#pragma once
#endif

//-----------------------------------------------------

class CCallQueue
{
public:
	CCallQueue();

	~CCallQueue();

	int Count();

	void CallQueued();

	DLL_CLASS_IMPORT void ParallelCallQueued( const char *pszName, IThreadPool *pPool = NULL );

	template <typename FUNCTOR>
	void QueueCall( FUNCTOR &&functor )
	{
		if ( !m_bNoQueue )
		{
			m_queue.PushItem( new std::function<void()>( std::forward<FUNCTOR>( functor ) ) );
		}
		else
		{
			m_bProcessing = true;
			if ( !m_bAbortProcessing )
			{
				functor();
			}
			m_bAbortProcessing = false;
			m_bProcessing = false;
		}
	}

	void Flush();

private:
	CTSQueue<std::function<void()> *> m_queue;
	int16 m_nRefCount;
	bool m_bNoQueue;
	bool m_bAbortProcessing;
	bool m_bProcessing;
};

#endif // CALLQUEUE_H
