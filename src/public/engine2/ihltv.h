//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef IHLTV_H
#define IHLTV_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/interface.h"
#include "public/interfaces/interfaces.h"

class CNetworkGameServerBase;
class CPlayerSlot;
class IHLTVDirector;
class INetworkMessageInternal;
class CNetMessage;
class CUtlBuffer;
class CSteamID;
class CGameInfo;
struct netadr_t;

//-----------------------------------------------------------------------------
// Interface the HLTV module exposes to the engine
//-----------------------------------------------------------------------------
class IHLTVServer : public IBaseInterface
{
public:
	virtual	~IHLTVServer();

	virtual	CNetworkGameServerBase *GetBaseServer( void ) = 0; // get HLTV base server interface
	virtual	IHLTVDirector *GetDirector( void ) = 0;	// get director interface
	virtual	CPlayerSlot GetHLTVSlot( void ) = 0; // return the player slot of HLTV in game
	virtual float	GetOnlineTime( void ) = 0; // seconds since broadcast started
	virtual void	GetLocalStats( int &proxies, int &slots, int &specs ) = 0; 
	// AMNOTE: Sums the hltv_proxies, hltv_slots and hltv_clients of the connected relay proxies
	virtual void	GetRelayStats( int &proxies, int &slots, int &specs ) = 0;
	virtual void	GetGlobalStats( int &proxies, int &slots, int &specs ) = 0; 

	// AMNOTE: Network system's local address with the SourceTV port, on master and relay alike
	virtual const netadr_t *GetLocalAddress( void ) = 0;
	virtual const netadr_t *GetRelayAddress( void ) = 0; // returns relay address

	virtual bool	IsMasterProxy( void ) = 0; // true, if this is the HLTV master proxy
	virtual bool	IsTVRelay( void ) = 0;
	virtual bool	IsDemoPlayback( void ) = 0; // true if this is a HLTV demo
	virtual bool	IsActive( void ) = 0;

	virtual void	BroadcastEvent( INetworkMessageInternal *pEvent, const CNetMessage *pData ) = 0; // send a director command to all specs
	
	virtual bool	IsRecording() = 0;
	virtual const char	*GetRecordingDemoFilename ( void ) = 0;
	// AMNOTE: -1 until the first broadcast frame arrives
	virtual int	GetFirstTick( void ) = 0;
	virtual void	StartAutoRecording( void ) = 0;
	virtual void	StopRecording( const CGameInfo *pGameInfo ) = 0;
	virtual void	AddSaveGame( CSteamID steamID, uint64, int, const CUtlBuffer &buf ) = 0;
};

#endif
