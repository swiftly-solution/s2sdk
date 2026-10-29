#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/bitvec.h"
#include "public/const.h"
#include "clientframe.h"
#include "isource2server.h"
#include "serversideclientbase.h"
#include "public/tier0/jobthread.h"
#include "public/tier0/utlvector.h"

class CHLTVServer;

// AMNOTE: Counted per client and added to the server totals on disconnect
struct HltvReplayStats_t
{
	uint32 m_nClients;
	uint32 m_nStartRequests;
	uint32 m_nSuccessfulStarts;
	uint32 m_nStopRequests;
	uint32 m_nAbortStopRequests;
	uint32 m_nUserCancels;
	uint32 m_nFullReplays;
	uint32 m_nNetAbortReplays;

	// AMNOTE: Already in a replay, rate limited, no frame for the delay (two checks), the delay can't be matched,
	// the HLTV frame isn't ready
	uint32 m_nFailedReplays[6];
};

class CServerSideClient : public CServerSideClientBase
{
public:
	virtual ~CServerSideClient() = 0;

public:
	// Bit n is set when this client hears client n
	CPlayerBitVec m_VoiceStreams;
	// Bit n is set when this client hears client n by proximity
	CPlayerBitVec m_VoiceProximity;

	// Filled for each snapshot and passed to the game's transmit checks
	CCheckTransmitInfo m_CheckTransmitInfo;

	CClientFrameManager m_FrameManager;
	CClientFrame *m_pCurrentFrame;

	// AMNOTE: Move messages in the current packet, the client is disconnected when there's more than one
	int m_nMoveMessagesInPacket;

	// AMNOTE: Server time of ActivatePlayer, -1 before it. Snapshots keep more frames for sv_extra_client_connect_time after it
	float m_flTimeClientBecameFullyConnected;

	// Whether the client hears its own voice
	bool m_bVoiceLoopback;

	// AMNOTE: The HLTV replay plays a stash instead of the HLTV server's frames
	bool m_bHltvReplayFromStash;

	// AMNOTE: The game sends the client another player's view with a delay (sv_maxreplay)
	bool m_bIsInReplayMode;
	int m_nHltvReplayDelay;
	CHLTVServer *m_pHltvReplayServer;

	// AMNOTE: Ticks the HLTV replay stops, starts, and slows down between, sent in CSVCMsg_HltvReplay
	int m_nHltvReplayStopAt;
	int m_nHltvReplayStartAt;
	int m_nHltvReplaySlowdownBeginAt;
	int m_nHltvReplaySlowdownEndAt;
	float m_flHltvReplaySlowdownRate;

	// AMNOTE: Tick of the last HLTV replay frame sent
	int m_nHltvLastSendTick;
	double m_flLastHltvReplayRequestTime;

	// AMNOTE: Clear frees each element, nothing adds any
	CUtlVector<void *> m_unk311;

	// AMNOTE: Formatted by GetHltvReplayStats
	HltvReplayStats_t m_HltvReplayStats;

	// The job sending the client's snapshot, Await waits for it
	CThreadedJobWithDependencies *m_pSendJob;
};

#endif // SERVERSIDECLIENT_H
