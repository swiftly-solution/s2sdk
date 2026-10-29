//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef ISOURCE2SERVERCONFIG_H
#define ISOURCE2SERVERCONFIG_H

#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/keyvalues.h"
#include "public/tier0/playerslot.h"
#include "public/steamworks/steamclientpublic.h"


enum HostStateLoopModeType_t
{
	HOST_STATE_LOOP_MODE_IDLE = 0,
	HOST_STATE_LOOP_MODE_GAME,
	HOST_STATE_LOOP_MODE_SOURCETV_RELAY,

	HOST_STATE_LOOP_MODE_COUNT
};

#define INTERFACEVERSION_SERVERCONFIG			"Source2ServerConfig001"

abstract_class ISource2ServerConfig : public IAppSystem
{
public:
	// Returns string describing current .dll.  e.g., TeamFortress 2, Half-Life 2.
	//  Hey, it's more descriptive than just the name of the game directory
	virtual const char *GetGameDescription( void ) = 0;

	virtual int			GetNetworkVersion( void ) = 0;
	// AMNOTE: CS2 has no GetTickInterval, its tick interval is a fixed 1/64

	// Get server maxplayers and lower bound for same
	virtual void		GetPlayerLimits( int& minplayers, int& maxplayers, int &defaultMaxPlayers, bool &bIsMultiplayer ) const = 0;

	// Returns max splitscreen slot count ( 1 == no splits, 2 for 2-player split screen )
	virtual int			GetMaxSplitscreenPlayers( void ) = 0;

	// Return # of human slots, -1 if can't determine or don't care (engine will assume it's == maxplayers )
	virtual int			GetMaxHumanPlayers() = 0;

	virtual bool		ShouldNotifyLocalClientConnectionStateChanges() = 0;

	virtual void		OnClientFullyConnect( CPlayerSlot slot ) = 0;

	virtual void		GetHostStateLoopModeInfo( HostStateLoopModeType_t type, CUtlString &loopModeName, KeyValues **ppLoopModeOptions ) = 0;

	virtual bool		AllowDedicatedServers( EUniverse universe ) const = 0;

	virtual void		GetConVarPrefixesToResetToDefaults( CUtlString &sSemicolonDelimitedPrefixList ) const = 0;

	virtual bool		AllowSaveRestore() = 0;

	virtual bool		unk101() = 0;

	virtual bool		IsCommandQueueEnabled() = 0;
	virtual float		GetCommandQueueDilationPercentage() = 0;

	// When true, the map command changes level instead of loading the map while a server is running
	virtual bool		ShouldMapCommandChangeLevel() = 0;
	// When true, a connecting client gets a new client object while below the limit instead of reusing a free one
	virtual bool		ShouldAllocateNewClients() = 0;
	virtual bool		Uses64TickInterval() = 0;
	virtual bool		InitGameEvents( CreateInterfaceFn factory ) = 0;
};

#endif // ISOURCE2SERVERCONFIG_H
