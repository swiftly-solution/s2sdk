//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef INETWORKSERVERSERVICE_H
#define INETWORKSERVERSERVICE_H

#ifdef _WIN32
#pragma once
#endif

#include "iengineservicemgr.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/checksum_crc.h"
#include "public/tier0/netadr.h"
#include "network_connection.pb.h"

class CNetworkGameServerBase;
class GameSessionConfiguration_t;
class ISource2WorldSession;
class IGameSpawnGroupMgr;
class IEntityReport;

abstract_class INetworkServerService : public IEngineService
{
public:
	virtual ~INetworkServerService();
	virtual CNetworkGameServerBase	*GetIGameServer( void ) = 0;
	virtual bool	IsActiveInGame( void ) const = 0;
	virtual bool	IsMultiplayer( void ) const = 0;
	virtual void	StartupServer( const GameSessionConfiguration_t &config, ISource2WorldSession *pWorldSession, const char * ) = 0;
	virtual void	SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;
	virtual void	AddServerPrerequisites( const GameSessionConfiguration_t &, const char *, ILoopModePrerequisiteRegistry *, bool ) = 0;
	//virtual void	SetServerSocket( int ) = 0;
	virtual bool	IsServerRunning( void ) const = 0;
	virtual void	DisconnectGameNow( ENetworkDisconnectionReason reason ) = 0;
	virtual void	PrintSpawnGroupStatus( void ) const = 0;
	// AMNOTE: CS2 has no GetTickInterval, its tick interval is a fixed 1/64
	//virtual void	ProcessSocket( void ) = 0;
	virtual netadr_t GetServerNetworkAddress( void ) = 0;
	virtual bool	GameLoadFailed( void ) const = 0;
	virtual void	SetGameLoadFailed( bool bFailed ) = 0;
	virtual void	SetGameLoadStarted( void ) = 0;
	virtual void	StartChangeLevel( const char *, const char *pszLandmark, void * ) = 0;
	virtual bool	FinishChangeLevel( void ) = 0;
	virtual bool	IsChangelevelPending( void ) const = 0;
	virtual void	PreserveSteamID( void ) = 0;
	virtual CRC32_t	GetServerSerializersCRC( void ) = 0;
	virtual void	*GetServerSerializersMsg( void ) = 0;
	virtual IGameSpawnGroupMgr *GetGameSpawnGroupMgr( void ) = 0;
	virtual bool	IsSaveRestoreAllowed( CUtlString *pReason ) = 0;
	virtual bool	IsEntityReportActive( void ) = 0;
	// Returns nullptr unless the entity report is active and targets this slot or every slot
	virtual IEntityReport *GetEntityReport( int nSlot ) = 0;
	// AMNOTE: Does nothing
	virtual void	unk101( void ) = 0;
	// AMNOTE: Does nothing
	virtual void	unk102( void ) = 0;
	virtual bool	ThreadInPrimaryOrSecondaryMainThread( void ) = 0;
	virtual void	EnterSyncInterval( void ) = 0;
	// AMNOTE: The bool picks which of two parallel work modes clients enter
	virtual void	ExitSyncInterval( bool ) = 0;
};

#endif // INETWORKSERVERSERVICE_H
