#ifndef GAMESHAREDDEFS_H
#define GAMESHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/threadtools.h"
#include "public/tier0/utlstring.h"

enum MapLoadType_t
{
	MapLoad_NewGame = 0,
	MapLoad_LoadGame,
	MapLoad_Transition,
	MapLoad_Background,
};

enum GlobalVarsUsageWarning_t
{
	GV_RENDERTIME_CALLED_DURING_SIMULATION,
	GV_CURTIME_CALLED_DURING_RENDERING
};

typedef void (*FnGlobalVarsWarningFunc)(GlobalVarsUsageWarning_t);

// Entities can span this many clusters before we revert to a slower area checking algorithm
#define	MAX_FAST_ENT_CLUSTERS	4
#define	MAX_ENT_CLUSTERS	64
#define MAX_WORLD_AREAS		8

struct PVSInfo_t
{
	// headnode for the entity's bounding box
	short		m_nHeadNode;			

	// number of clusters or -1 if too many
	short		m_nClusterCount;		

	// cluster indices
	unsigned short *m_pClusters;	

	// For dynamic "area portals"
	short		m_nAreaNum;
	short		m_nAreaNum2;

	// current position
	float		m_vCenter[3];

private:
	unsigned short m_pClustersInline[MAX_FAST_ENT_CLUSTERS];
};

class CGlobalVarsBase
{
public:

	CGlobalVarsBase();

public:
	// Absolute time (per frame still - Use Plat_FloatTime() for a high precision real time 
	//  perf clock, but not that it doesn't obey host_timescale/host_framerate)
	float realtime;
	// Absolute frame counter - continues to increase even if game is paused
	int framecount;

	// Clamped to [0.0001, 0.1]
	float absoluteframetime;
	// Frametime before clamping, also sent as CNETMsg_Tick::host_unfiltered_frametime
	float absoluteframetime_unbounded;

	int maxClients;

	// Index of the tick being simulated within this frame
	int m_nCurrentTickThisFrame;
	// Number of ticks simulated this frame
	int m_nTotalTicksThisFrame;

	// Client only: real time per usercmd tick (tick interval / host_timescale)
	float m_flUsercmdTickInterval;
	// Client only: Plat_FloatTime() at which the current usercmd tick started
	double m_flUsercmdTickStartTime;

	FnGlobalVarsWarningFunc m_pfnWarningFunc;


	// Current time 
	//
	// On the client, this (along with tickcount) takes a different meaning based on what
	// piece of code you're in:
	// 
	//   - While receiving network packets (like in PreDataUpdate/PostDataUpdate and proxies),
	//     this is set to the SERVER TICKCOUNT for that packet. There is no interval between
	//     the server ticks.
	//     [server_current_Tick * tick_interval]
	//
	//   - While rendering, this is the exact client clock 
	//     [client_current_tick * tick_interval + interpolation_amount]
	//
	//   - During prediction, this is based on the client's current tick:
	//     [client_current_tick * tick_interval]
	float curtime;
	
	// Time spent on last server or client frame (has nothing to do with think intervals)
	float frametime;

	// Client only, zero during simulation: fraction into the next simulation tick
	float interpolation_amount;
	// Client only, zero during simulation: fraction into the current usercmd tick, stamps subtick moves
	float m_flUsercmdTickFraction;

	bool m_bInSimulation;
	bool m_bEnableAssertions;

	// Simulation ticks - does not increase when game is paused
	int tickcount;

	// Client only: ticks simulated since the client started ticking, reset per connection
	int m_nClientTickCount;
	// Client only: m_nClientTickCount in seconds
	float m_flClientTime;

	// Non-zero when during movement processing, it's the part after the decimal point of the "when" field in player's subtick moves.
	float m_flSubtickFraction;

	// AMNOTE: CS2 has no m_flIntervalPerTick, its tick interval is a fixed 1/64

	bool m_bIsOncePerFrameAsyncWorkPhase;

	ThreadId_t m_nThreadId;
};


class CGlobalVars : public CGlobalVarsBase
{	
public:

	CGlobalVars();

public:
	// Current map
	CUtlString		mapname;
	CUtlString		startspot;
	MapLoadType_t	eLoadType;		// How the current map was loaded
	bool mp_teamplay;

	// current maxentities
	int				maxEntities;

	int				serverCount;
};


#endif