//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef IVENGINESERVER2_H
#define IVENGINESERVER2_H

#ifdef _WIN32
#pragma once
#endif

#include "isource2engine.h"
#include "public/networksystem/inetchannelinfo.h"
#include "public/tier0/keyvalues.h"
#include "public/tier0/bufferstring.h"
#include "gameshareddefs.h"
#include "public/const.h"
#include "public/tier0/playerslot.h"
#include "public/mathlib/vector.h"
#include "public/tier0/logging.h"
#include "public/tier0/convar.h"
#include "public/tier0/utlvector.h"
#include "public/entity2/entityidentity.h"
#include "public/steamworks/steamclientpublic.h"
#include "network_connection.pb.h"

class IAchievementMgr;
class CGamestatsData;
class CSharedEdictChangeInfo;
class IPVS;
class CGameInfo;
class INetworkMessageInternal;
class CNetMessage;
enum SignonState_t : int;

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

#define INTERFACEVERSION_VENGINESERVER	"Source2EngineToServer001"

// Times are in seconds, relative to the current time
struct HltvReplayParams_t
{
	int m_nPrimaryTargetEntIndex = -1;
	float m_flDelay = 0.0f;
	float m_flStopAt = 0.0f;
	float m_flPlaybackSpeed = 1.0f;
	float m_flSlowdownBeginAt = 0.0f;
	float m_flSlowdownEndAt = 0.0f;
	float m_flSlowdownRate = 1.0f;
	bool m_bAbortCurrentReplay = false;
	int m_nReason = 0;
	// 1 = replay the stash m_nStashId, 2 = replay all of the stash instead of its last m_flDelay seconds
	uint32 m_nFlags = 0;
	uint32 m_nStashId = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the game DLL
//-----------------------------------------------------------------------------
abstract_class IVEngineServer2 : public ISource2Engine
{
public:
	virtual EUniverse	GetSteamUniverse() const = 0;

	virtual WorldGroupId_t	FindWorldGroupByName( const char *pszName ) = 0;
	virtual const char	*GetWorldGroupName( WorldGroupId_t hWorldGroup ) = 0;
	virtual WorldGroupId_t	GetFirstWorldGroupId( bool bClient ) = 0;
	virtual WorldGroupId_t	MaxWorldGroupId( bool bClientSide ) = 0;
	virtual bool		IsWorldGroupValid( WorldGroupId_t hWorldGroup ) = 0;

	// Points to two floats for the last main loop iteration: the time it took before running its frame, and the frame's time
	virtual float		*GetMainLoopTimes() = 0;

	virtual void		SetFrameTimeAmnesty( const char *amnesty, int, float frametime ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool check_cvar ) = 0;

	virtual void		SetFramePerformanceTag( const char *pszTag, int nValue, int, float flDuration ) = 0;

	virtual void		ShowFrameTimeReport( void *, bool, LoggingChannelID_t channel = -1 ) = 0;

	virtual void		DumpNetStats( void *, void (*pfnPrint)( const char * ) ) = 0;

	virtual bool		ShouldForceMaxFrametimeToTickInterval() = 0;
	virtual uint32		GetLongFrameCount() = 0;

	// Tell engine to change level ( "changelevel s1\n" or "changelevel2 s1 s2\n" )
	virtual void		ChangeLevel( const char *s1, const char *s2 ) = 0;

	// Ask engine whether the specified map is a valid map file (exists and has valid version number).
	virtual int			IsMapValid( const char *filename ) = 0;

	// Is this a dedicated server?
	virtual bool		IsDedicatedServer( void ) = 0;

	// Is this an HLTV relay?
	virtual bool		IsHLTVRelay( void ) = 0;

	// Is server only accepting local connections?
	virtual bool		IsServerLocalOnly( void ) = 0;

	virtual int			PrecacheGeneric( const char *s, bool preload = false ) = 0;
	virtual bool		IsGenericPrecached( char const *s ) const = 0;

	// Returns the server assigned userid for this player.  Useful for logging frags, etc.
	//  returns -1 if the edict couldn't be found in the list of players.
	virtual CPlayerUserId GetPlayerUserId( CPlayerSlot nSlot ) = 0;
	virtual const char	*GetPlayerNetworkIDString( CPlayerSlot nSlot ) = 0;
	// Get stats info interface for a client netchannel
	virtual INetChannelInfo* GetPlayerNetInfo( CPlayerSlot nSlot ) = 0;

	// Returns the entities transmitted in the client's frame for its acknowledged delta tick, or nullptr
	virtual const CBitVec<MAX_EDICTS> *GetEntityTransmitBitsForClient( CPlayerSlot nSlot ) = 0;
	// Returns -1 for an invalid slot
	virtual int			GetClientDeltaTick( CPlayerSlot nSlot ) = 0;

	// Given the current PVS(or PAS) and origin, determine which players should hear/receive the message
	virtual void		Message_DetermineMulticastRecipients( bool usepas, const Vector& origin, CPlayerBitVec& playerbits ) = 0;

	// Issue a command to the command parser as if it was typed at the server console.
	virtual void		ServerCommand( const char *str ) = 0;
	// Issue the specified command to the specified client (mimics that client typing the command at the console).
	virtual void		ClientCommand( CPlayerSlot nSlot, const char *szFmt, ... ) FMTFUNCTION( 3, 4 ) = 0;

	// Print szMsg to the client console.
	virtual void		ClientPrintf( CPlayerSlot nSlot, const char *szMsg ) = 0;

	virtual bool		IsLowViolence() = 0;
	virtual void		SetHLTVChatBan( const CSteamID &steamID, bool bBanned ) = 0;
	virtual bool		IsAnyClientLowViolence() = 0;

	// Get the current game directory (hl2, tf2, hl1, cstrike, etc.)
	virtual void        GetGameDir( CBufferString &gameDir ) = 0;

	// Create a bot with the given name.  Player index is -1 if fake client can't be created
	virtual CPlayerSlot	CreateFakeClient( const char *netname ) = 0;

	// Get a convar keyvalue for s specified client
	virtual const char	*GetClientConVarValue( CPlayerSlot nSlot, const char *name ) = 0;

	// Print a message to the server log file
	virtual void		LogPrint( const char *msg ) = 0;
	virtual bool		IsLogEnabled() = 0;

	virtual bool IsSplitScreenPlayer( CPlayerSlot nSlot ) = 0;
	virtual CPlayerSlot GetSplitScreenPlayerAttachToEdict( CPlayerSlot nSlot ) = 0;
	virtual CPlayerSlot GetSplitScreenPlayerForEdict( CPlayerSlot nSlot, int nSplitScreenSlot ) = 0;

	virtual void	UnloadSpawnGroup( SpawnGroupHandle_t spawnGroup, ESpawnGroupUnloadOption eUnloadOption ) = 0;
	virtual SpawnGroupHandle_t LoadSpawnGroup( const SpawnGroupDesc_t & ) = 0;
	virtual void	SetSpawnGroupDescription( SpawnGroupHandle_t spawnGroup, const char *pszDescription ) = 0;
	virtual bool	IsSpawnGroupLoaded( SpawnGroupHandle_t spawnGroup ) const = 0;
	virtual bool	IsSpawnGroupLoading( SpawnGroupHandle_t spawnGroup ) const = 0;
	virtual void	MakeSpawnGroupActive( SpawnGroupHandle_t spawnGroup ) = 0;
    virtual bool    IsSpawnGroupLoadedForClient( CPlayerSlot nSlot, SpawnGroupHandle_t spawnGroup ) = 0;
	virtual void	SynchronouslySpawnGroup( SpawnGroupHandle_t spawnGroup ) = 0;
	virtual void	SynchronizeAndBlockUntilLoaded( SpawnGroupHandle_t spawnGroup ) = 0;

	virtual void SetTimescale( float flTimescale ) = 0;

	virtual uint32		GetAppID() = 0;

	// Returns the SteamID of the specified player. It'll be NULL if the player hasn't authenticated yet.
	virtual const CSteamID	*GetClientSteamID( CPlayerSlot nSlot ) = 0;

	// Methods to set/get a gamestats data container so client & server running in same process can send combined data
	virtual void SetGamestatsData( CGamestatsData *pGamestatsData ) = 0;
	virtual CGamestatsData *GetGamestatsData() = 0;

	// Send a client command keyvalues
	// keyvalues are deleted inside the function
	virtual void ClientCommandKeyValues( CPlayerSlot nSlot, KeyValues *pCommand ) = 0;

	// This makes the host run 1 tick per frame instead of checking the system timer to see how many ticks to run in a certain frame.
	// i.e. it does the same thing timedemo does.
	virtual void SetDedicatedServerBenchmarkMode( bool bBenchmarkMode ) = 0;

	// Returns true if this client has been fully authenticated by Steam
	virtual bool IsClientFullyAuthenticated( CPlayerSlot nSlot ) = 0;

	virtual CGlobalVars	*GetServerGlobals() = 0;

	// Sets a USERINFO client ConVar for a fakeclient
	virtual void		SetFakeClientConVarValue( CPlayerSlot nSlot, const char *cvar, const char *value ) = 0;

	virtual CSharedEdictChangeInfo* GetSharedEdictChangeInfo() = 0;

	virtual void SetAchievementMgr( IAchievementMgr *pAchievementMgr ) =0;
	virtual IAchievementMgr *GetAchievementMgr() = 0;

	// Fill in the player info structure for the specified player index (name, model, etc.)
	virtual bool GetPlayerInfo( CPlayerSlot nSlot, google::protobuf::Message &info ) = 0;

	// Returns the XUID of the specified player. It'll be NULL if the player hasn't connected yet.
	virtual uint64 GetClientXUID( CPlayerSlot nSlot ) = 0;

	virtual IPVS				*GetPVSForSpawnGroup( SpawnGroupHandle_t spawnGroup ) = 0;
	virtual SpawnGroupHandle_t	FindSpawnGroupByName( const char *szName ) = 0;

	// Returns the SteamID of the game server
	virtual CSteamID	GetGameServerSteamID() = 0;

	virtual int GetBuildVersion( void ) const = 0;

	virtual bool IsClientLowViolence( CPlayerSlot nSlot ) = 0;

	// Kicks the slot with the specified NetworkDisconnectionReason
	virtual void DisconnectClient( CPlayerSlot nSlot, ENetworkDisconnectionReason reason, const char *szInternalReason = nullptr ) = 0;

	virtual void DisconnectAllClients( ENetworkDisconnectionReason reason ) = 0;
	virtual void GetAllSpawnGroupsWithPVS( CUtlVector<SpawnGroupHandle_t> *spawnGroups, CUtlVector<IPVS *> *pOut ) = 0;

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).
	virtual bool GetClientListening(CPlayerSlot iReceiver, CPlayerSlot iSender) = 0;
	virtual bool SetClientListening(CPlayerSlot iReceiver, CPlayerSlot iSender, bool bListen) = 0;
	virtual bool SetClientProximity(CPlayerSlot iReceiver, CPlayerSlot iSender, bool bUseProximity) = 0;

	// AMNOTE: Creates or reuses a client without a connection, returns its slot
	virtual CPlayerSlot CreateClient( CPlayerSlot nSlot, CSteamID steamID, const char *pszName ) = 0;
	// AMNOTE: Has no effect on clients made by CreateClient, which lock their slot
	virtual void SetRecyclePlayerSlot( CPlayerSlot nSlot, bool bRecycle ) = 0;
	virtual SignonState_t GetClientSignonState( CPlayerSlot nSlot ) = 0;

	virtual void KickClient( CPlayerSlot nSlot, const char *szInternalReason, ENetworkDisconnectionReason reason ) = 0;
	virtual void BanClient( CPlayerSlot nSlot, float flDuration, bool bKick ) = 0;
	virtual void BanClient( CSteamID steamId, float flDuration, bool bKick ) = 0;

	virtual bool StartHltvReplay( CPlayerSlot nSlot, const HltvReplayParams_t &params ) = 0;
	virtual void ForceStopHltvReplay( CPlayerSlot nSlot ) = 0;
	virtual void StopAllHltvReplays() = 0;
	virtual int GetClientHltvReplayDelay( CPlayerSlot nSlot ) = 0;
	virtual bool IsHltvReplayBufferAvailable() = 0;
	virtual bool CanStartHltvReplay( CPlayerSlot nSlot, int nDelay ) = 0;
	virtual void ResetHltvReplayRequestTime( CPlayerSlot nSlot ) = 0;
	virtual bool IsAnyHltvReplayActive() = 0;

	virtual void SetClientUpdateRate( CPlayerSlot nSlot, float flUpdateRate ) = 0;

	virtual void SetClientJitterBadThresholdUp( CPlayerSlot nSlot, float flThreshold ) = 0;
	virtual bool StashHltvReplay( uint32 nStashId, float flSeconds ) = 0;
	virtual void AddHltvRelayProxyWhitelist( uint32 a, uint32 b, uint32 c, uint32 d, uint32 numbits ) = 0;
	virtual bool WasShutDownRequested() const = 0;
	// Extends the matchmaking reservation timeout and sends the two bytes and the data in an S2A_RESERVE_CHECK_RESPONSE
	// packet to every reserved client, or only to the one with nAccountID when it is non-zero
	virtual void SendReserveCheckResponse( uint32 nAccountID, uint8 nUnk1, uint8 nUnk2, uint32 nDataSize, const void *pData ) = 0;
	virtual bool IsTvRecording() = 0;
	virtual void StartAutoRecording() = 0;
	virtual void RecordDemo( const char *pszFilename ) = 0;
	virtual void StopRecordingDemo( const CGameInfo *pGameInfo ) = 0;
	virtual void BroadcastEvent( INetworkMessageInternal *pEvent, const CNetMessage *pData ) = 0;
	// Returns an empty string when not recording
	virtual const char *GetTvRecordingDemoFilename() = 0;
	virtual const char *GetMapName() = 0;
	virtual ConVarUserInfoSet_t GetClientUserInfo( CPlayerSlot nSlot ) = 0;
	virtual bool IsRecordingDemo() = 0;
};

typedef IVEngineServer2 IVEngineServer;

#endif // IVENGINESERVER2_H
