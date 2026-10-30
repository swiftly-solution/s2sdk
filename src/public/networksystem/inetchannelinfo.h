#ifndef INETCHANNELINFO_H
#define INETCHANNELINFO_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/tier0/netadr.h"
#include "source2_steam_stats.pb.h"

struct SNetChannelLatencyStats;

enum class Flows
{
    Outgoing = 0,
    Incoming = 1,
    Max = 2
};

struct NetChanStat_t
{
	float m_flAvg;
	float m_flMin;
	float m_flMax;
	float m_flStdDev;
};

class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};
	
	virtual const char  *GetName( void ) const = 0;	// get channel name
	virtual const char  *GetAddress( void ) const = 0; // get channel IP address as string
	virtual const netadr_t &GetRemoteAddress( void ) const = 0;
	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec
	
	virtual bool		IsLocalHost( void ) const = 0;	// true if localhost
	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;

	virtual float		GetAvgLatency( void ) const = 0;	// average packet latency in seconds
	virtual float		GetEngineLatency( void ) const = 0;	// current latency (RTT), more accurate but jittering
	
	virtual float		GetAvgLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;
	// AMNOTE: Highest frame jitter in milliseconds over the recent frames, -1 when there are none
	virtual float		GetPeakJitter( int flow ) const = 0;
	virtual float		GetAvgData( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual uint64	GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets( int flow ) const = 0;
	virtual int			GetSequenceNr( int flow ) const = 0;	// last send seq number
	virtual float		GetTimeSinceLastReceived( void ) const = 0;	// get time since last recieved packet in seconds
	virtual void		GetRemoteFramerate( float *pflFrameTime, float *pflFrameTimeStdDeviation, float *pflUnfilteredFrameTime ) const = 0;

	virtual float		GetTimeoutSeconds( void ) const = 0;
	virtual float		GetTimeUntilTimeout( void ) const = 0;
	
	virtual float		GetLateFrameJitterThreshold() const = 0;
	
	virtual void		ResetLatencyStats( int channel ) = 0;
	virtual SNetChannelLatencyStats *GetLatencyStats( int channel ) const = 0;
	virtual void		SetLatencyStats( int channel, const SNetChannelLatencyStats &stats ) = 0;
	
	virtual void		SetInterpolationAmount( float flInterpolationAmount, float flUpdateRate ) = 0;
	virtual void		SetNumPredictionErrors( int num ) = 0;
	virtual void		SetShowNetMessages( bool show ) = 0;

	// AMNOTE: Returns 0 when the frame was delivered, 1 dropped, 2 out of order, 3 unknown (not in the last 128 frames),
	// and its jitter in milliseconds, -1 when unknown
	virtual int			GetFrameStatus( int flow, int nSeqNr, float *pflJitter = nullptr ) const = 0;
	// AMNOTE: Writes the jitter of up to nMaxEntries recent frames, newest first, in 0.1 ms units, or -1 dropped,
	// -2 out of order, -3 unknown, and returns the count written
	virtual int			GetFrameJitterHistory( int flow, int16_t *pOut, int nMaxEntries ) const = 0;
	// AMNOTE: Fills pMsg with the flow's quality stats
	virtual void		unk101( int flow, CMsgSource2NetworkFlowQuality *pMsg, bool ) = 0;
};

#endif