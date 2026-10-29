#ifndef CLIENTFRAME_H
#define CLIENTFRAME_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/bitvec.h"
#include "public/const.h"
#include "public/tier0/threadtools.h"
#include "public/tier0/mempool.h"
#include "public/tier0/utllinkedlist.h"

class CFrameSnapshot;

// What was sent to a client in one snapshot
class CClientFrame
{
public:
	virtual ~CClientFrame() = 0;

	// AMNOTE: Decides how CClientFrameManager frees the frame: when true, it destructs the frame and returns it
	// to m_ClientFramePool, otherwise it deletes it. CClientFrame returns true, while CHLTVFrame returns false
	// and is allocated through g_pMemAlloc
	virtual bool IsAllocatedFromPool() = 0;

public:
	// The snapshot's tick
	int m_nTickCount;

	// AMNOTE: Zeroed on construction and copied along with m_nTickCount when a frame is copied
	int m_unk001;

	// The next frame in the manager's chain
	CClientFrame *m_pNext;
	CFrameSnapshot *m_pSnapshot;

	// Filled through the client's CCheckTransmitInfo, see its pointers of the same names
	CBitVec<MAX_EDICTS> m_TransmitEntity;
	CBitVec<MAX_EDICTS> m_NonTransmitEntity;
	CBitVec<MAX_EDICTS> m_TransmitOutOfPVS;
	CBitVec<MAX_EDICTS> *m_pTransmitAlways;
};

// The frames kept for a client, to delta snapshots from
class CClientFrameManager
{
public:
	virtual ~CClientFrameManager();

public:
	// AMNOTE: Nothing reads it and the constructor doesn't set it
	int m_unk001;

	CThreadFastMutex m_Mutex;

	// Ordered from the oldest to the newest tick
	CUtlLinkedList<CClientFrame *, unsigned short> m_Frames;
	CUtlMemoryPool<CClientFrame> m_ClientFramePool;
	int m_nNewestTick;
	int m_nOldestTick;

	// AMNOTE: Frames older than m_nOldestTick are looked up in it when it's set, and the manager hands it
	// frames older than a tick limit instead of freeing them. CHLTVServerAsync points it at its frame stash
	void *m_pFrameStash;
};

#endif // CLIENTFRAME_H
