#ifndef ISOURCE2SERVER_H
#define ISOURCE2SERVER_H

#ifdef _WIN32
#pragma once
#endif

#include "gameshareddefs.h"
#include "public/entity2/entityindex.h"
#include "public/entity2/entitynetworkable.h"
#include "public/tier0/bitbuf.h"
#include "public/tier0/utlmap.h"
#include "public/tier0/utlbuffer.h"
#include "public/const.h"
#include "public/appframework/iappsystem.h"
#include "public/scenesystem/iscenesystem.h"
#include "public/tier0/bitvec.h"
#include "public/tier0/playerslot.h"
#include "public/tier0/utlvector.h"
#include "public/tier2/fieldpath.h"
#include "public/networksystem/networksystemtypes.h"
#include "public/steamworks/steamclientpublic.h"
#include "public/networksystem/inetworkmessages.h"
#include "netmessages.pb.h"
#include "public/resourcefile/resourcetype.h"
#include "public/tier0/keyvalues3.h"
#include "isource2engine.h"
#include "inetworkgameserver.h"
#include "public/networkstrings/tabledefs.h"

#define MAX_CHANGE_OFFSETS	48
#define MAX_EDICT_CHANGE_INFOS	3000

class CNavData;
class INavListener;
class IToolGameSimulationAPI;
class CCreateGameServerLoadInfo;
class SaveGameParams_t;
class ISceneViewDebugOverlays;
class FlattenedSerializerSpewField_t;
class IHLTVServer;
class IHLTVDirector;

struct VarChangeList_t
{
	// Edicts remember the offsets of properties that change
	CUtlVectorFixedGrowable<VarChangeInfo_t, MAX_CHANGE_OFFSETS> m_ChangeOffsets;
	// The changed element indices of networked vectors, up to 64 per list
	CUtlVectorFixedGrowable<uint16_t *, 16> m_ChangedElements;
	// Changed fields under a branch, added only while they and m_ChangeOffsets stay under MAX_CHANGE_OFFSETS
	CUtlVector<CFieldPath> m_ChangedFieldPaths;
};

class CEdictChangeInfo
{
public:
	ChangeAccessorFieldPathIndexInfo_t *m_pChangeAccessorFieldPathInfo;
	VarChangeList_t m_Changes;
};

class CSharedEdictChangeInfo
{
public:
	CSharedEdictChangeInfo();

	CEdictChangeInfo m_ChangeInfos[MAX_EDICT_CHANGE_INFOS];

	uint16_t m_iSerialNumber;

	uint32_t m_nHighWaterMark; // The most change infos any frame has used
	uint32_t m_nChangeInfos;	// How many are in use this frame.
};
extern CSharedEdictChangeInfo *g_pSharedChangeInfo;

class CCheckTransmitInfo
{
public:
	CBitVec<MAX_EDICTS>	*m_pTransmitEntity;	// entity n is already marked for transmission
	CBitVec<MAX_EDICTS>	*m_pNonTransmitEntity; // entity n exists but isn't transmitted; filled from m_pTransmitEntity after the checks, and the client is sent its changes
	CBitVec<MAX_EDICTS>	*m_pTransmitOutOfPVS; // entity n is outside the PVS but still gets out-of-PVS updates (sv_outofpvsentityupdates)
	CBitVec<MAX_EDICTS>	*m_pTransmitAlways; // entity n is always send even if not in PVS (HLTV and Replay only)
	CUtlVector<SpawnGroupHandle_t> m_LoadedSpawnGroups; // sorted; entities from spawn groups the client hasn't loaded aren't checked
	vis_info_t m_VisInfo; // filled by ISource2GameClients::ClientSetupVisibility
	CPlayerSlot m_nPlayerSlot;
	bool m_bFullUpdate; // the client gets a full update instead of a delta
};

class IServerGCLobby
{
public:
	virtual bool HasLobby() const = 0;
	virtual bool SteamIDAllowedToConnect( const CSteamID &steamId ) const = 0;
	virtual void UpdateServerDetails( void ) = 0;
	virtual bool ShouldHibernate() = 0;
	virtual bool SteamIDAllowedToP2PConnect( const CSteamID &steamId ) const = 0;
	virtual bool LobbyAllowsCheats( void ) const = 0;
};

#define INTERFACEVERSION_SERVERGAMEDLL				"Source2Server001"

class ISource2Server : public IAppSystem
{
public:
	virtual bool			IsValveDS() const = 0;

	// Returns the previous globals, or nullptr if they were the defaults
	virtual CGlobalVars		*SetGlobals( CGlobalVars *pGlobals ) = 0;

	// Let the game .dll allocate it's own network/shared string tables
	virtual void			GameCreateNetworkStringTables( void ) = 0;

	virtual void			WriteSignonMessages( const bf_write &buf ) = 0;

	virtual void			PreWorldUpdate( bool simulating ) = 0;

	virtual CUtlOrderedMap<int, Entity2Networkable_t, CDefLess<int>, unsigned short>	*GetEntity2Networkables( void ) const = 0;

	virtual bool			GetEntity2Networkable( CEntityIndex nIndex, Entity2Networkable_t *pOut ) = 0;

	// Called to apply lobby settings to a dedicated server
	virtual void			ApplyGameSettings( KeyValues *pKV ) = 0;

	// The server should run physics/think on all edicts
	// One of these bools is 'simulating'... probably
	virtual void			GameFrame( bool simulating, bool bFirstTick, bool bLastTick ) = 0;

	// Returns true if the game DLL wants the server not to be made public.
	// Used by commentary system to hide multiplayer commentary servers from the master.
	virtual bool			ShouldHideFromMasterServer( bool bServerHasPassword ) = 0;

	virtual void			GetMatchmakingTags( CBufferString &buf ) = 0;

	virtual void			ServerHibernationUpdate( bool bHibernating ) = 0;

	virtual IServerGCLobby	*GetServerGCLobby() = 0;

	virtual void			GetMatchmakingGameData( CBufferString &buf ) = 0;

	// return true to disconnect client due to timeout (used to do stricter timeouts when the game is sure the client isn't loading a map)
	virtual bool			ShouldTimeoutClient( int nUserID, float flTimeSinceLastReceived ) = 0;

	virtual void			PrintStatus( CEntityIndex nPlayerEntityIndex, CBufferString &output ) = 0;

	virtual int				GetServerGameDLLFlags( void ) const = 0;

	// Get the list of cvars that require tags to show differently in the server browser
	virtual void			GetTaggedConVarList( KeyValues *pCvarTagList ) = 0;

	// Give the list of datatable classes to the engine.  The engine matches class names from here with
	//  edict_t::classname to figure out how to encode a class's data for networking
	virtual CUtlVector<ServerClass*> *GetAllServerClasses( void ) = 0;

	virtual const char		*GetActiveWorldName( void ) const = 0;

	virtual bool			IsPaused( void ) const = 0;

	virtual bool			GetNavMeshData( CNavData *pNavMeshData ) = 0;
	virtual void			SetNavMeshData( const CNavData *navMeshData ) = 0;
	virtual void			RegisterNavListener( INavListener *pNavListener ) = 0;
	virtual void			UnregisterNavListener( INavListener *pNavListener ) = 0;
	virtual bool			GetBugReportAttachment( int nIndex, CUtlBuffer &buf, CUtlString &sName, CUtlString &sDescription ) = 0;

	virtual IToolGameSimulationAPI *GetToolGameSimulationAPI( void ) = 0;
	virtual void			GetAnimationActivityList( CUtlVector<CUtlString> &activityList ) = 0;
	virtual void			GetAnimationEventList( CUtlVector<CUtlString> &eventList ) = 0;
	virtual void			FilterPlayerCounts( int *pInOutHumans, int *pInOutHumansSlots, int *pInOutBots ) = 0;

	// Called after the steam API has been activated post-level startup
	virtual void			GameServerSteamAPIActivated( void ) = 0;

	virtual void			GameServerSteamAPIDeactivated( void ) = 0;

	virtual void			OnHostNameChanged( const char *pHostname ) = 0;
	virtual void			PreFatalShutdown( void ) const = 0;
	virtual void			UpdateWhenNotInGame( float flFrameTime ) = 0;

	virtual void			*GetEconItemSystem( void ) = 0;

	virtual void			ServerConVarChanged( const char *pVarName, const char *pValue ) = 0;

	// Returns a list of values and names corresponding to HitGroup_t enum
	virtual void			GetHitGroupInfo( CUtlVector<int> &groupIDs, CUtlVector<CUtlString> &groupNames ) = 0;

	// Adds the game's fields to the server section of the status_json output
	virtual void			WriteStatusJson( KeyValues3 *pServer ) = 0;

	virtual bool			SaveGame_CalcFileName( const char *pszSaveName, CUtlString &fileName ) = 0;
	virtual bool			GetRequiredAddonsFromSaveFile( const char *pszSaveName, CUtlString &requiredAddons ) = 0;
	virtual void			GetLevelsFromSaveFile( const char *pszSaveName, CUtlVector<CCreateGameServerLoadInfo> &levels, bool bWipeAndExtract, int, CUtlString *pComment ) = 0;
	virtual void			ClearSaveDirectory( void ) = 0;
	virtual void			PreSaveGameLoaded( const char *pszSaveName ) = 0;
	virtual void			AppendSaveGameResources( HGameResourceManifest hManifest, ILoadingSpawnGroup *pLoadingSpawnGroup, SpawnGroupHandle_t hSpawnGroup, const void * ) = 0;
	virtual void			AppendTransitionResources( HGameResourceManifest hManifest, ILoadingSpawnGroup *pLoadingSpawnGroup, SpawnGroupHandle_t hSpawnGroup, const void * ) = 0;
	virtual /*SaveGameResult_t*/ int SaveGame( const SaveGameParams_t &params ) = 0;
	virtual bool			IsAsyncSaveInProgress( void ) = 0;
	virtual bool			ProcessPendingSaveRequest( void ) = 0;
	virtual bool			HasPendingSaveRequest( void ) = 0;
	virtual void			FinishAsyncSave( void ) = 0;

	virtual const char		*GetEntityUniqueHammerID( CEntityIndex nEntityIndex ) = 0;

	// AMNOTE: A scope is one subclass VData file, matched by its file path or, in the ByDataType variants,
	// by its generic data type. A null or empty scope matches any scope.
	virtual const char		*GetVDataClassName( const char *pszName, const char *pszScopeFile ) = 0;
	virtual const char		*GetVDataClassNameByDataType( const char *pszName, const char *pszGenericDataType ) = 0;
	virtual const CUtlVector<CUtlString> &GetSubclassNamesInScope( const char *pszScopeFile ) = 0;
	virtual const CUtlVector<CUtlString> &GetSubclassNamesInScopeByDataType( const char *pszGenericDataType ) = 0;
	virtual void			GetAllSubclassNames( CUtlVector<CUtlString> &names ) = 0;
	virtual const char		*GetSubclassDesignerName( const char *pszSubclassName ) = 0;

	virtual void			UpdateGCInformation( bool, bool ) = 0;

	virtual const CUtlVector<CUtlString> &GetDesignerNamesForClass( const char *pszClassName ) = 0;
	// AMNOTE: Does nothing in CS2
	virtual void			unk_101( void *, void * ) = 0;

	virtual bool			ReportGCQueuedMatchStart( int32 iReservationStage, uint32 *puiConfirmedAccounts, int numConfirmedAccounts ) = 0;

	// AMNOTE: Called each main loop iteration with two doubles, the iteration's time and the time slept after the main loop.
	// Forwards to the game rules, which do nothing with it in CS2
	virtual void			unk_201( void * ) = 0;
	virtual ISceneViewDebugOverlays *GetDebugOverlays( void ) = 0;

	virtual const char		*GetNativeClassForScriptClass( const char *pszScriptClassName ) = 0;
	virtual CEntityClass	*GetScriptClassForDesignerName( const char *pszDesignerName ) = 0;
	virtual bool			IsScriptClassDerivedFrom( const char *pszDesignerName, const char *pszBaseName ) = 0;

	virtual bool			ShouldHoldGameServerReservation( float flTimeElapsedWithoutClients ) = 0;

	virtual void			OnBroadcastRelayRequestSucceeded( void * ) = 0;
	virtual void			SendServerFrameTime( float flFrameTime ) = 0;
	virtual void			OnClientHltvReplayStart( CPlayerSlot slot, int ) = 0;
	virtual void			OnClientHltvReplayStop( CPlayerSlot slot ) = 0;

	virtual bool			FormatSerializerFieldValue( CEntityIndex nEntityIndex, FlattenedSerializerSpewField_t &field ) = 0;

	// AMNOTE: Only CS2 has these two
	virtual void			CaptureUserCommands( void ) = 0;
	virtual CNetMessagePB<CSVCMsg_UserCommands> *CreateUserCommandsMessage( void ) = 0;

	virtual bool			ProcessClientStringCommand( CPlayerSlot slot, const CCommand &args, uint32 nPredictionSync ) = 0;
	virtual void			OnPreMatchInterfaceCommand( uint32 uiAccountID, int, const char *pszCommand ) = 0;
	virtual void			SetPlayerTeammatePreferredColor( uint32 uiAccountID, int nColor ) = 0;
	virtual void			UpdateCompTeammateColors( void ) = 0;

	// A reason other than NETWORK_DISCONNECT_INVALID rejects the connecting client
	virtual ENetworkDisconnectionReason GetClientConnectRejectReason( const CSteamID &steamID ) = 0;
	virtual const char		*ClientConnectionValidatePreNetChan( const CSteamID &steamID, const char *pszPlayerName ) = 0;

	virtual bool			LogForHTTPListeners( const char *pszLogLine ) = 0;

	// AMNOTE: These four do nothing in CS2
	virtual void			unk_301( void ) = 0;
	virtual void			unk_302( void ) = 0;
	virtual void			unk_303( void ) = 0;
	virtual void			unk_304( void ) = 0;

	virtual void			RegisterClientNetMessageHandlers( INetChannel *pNetChannel, CPlayerSlot slot, int nAction ) = 0;
	virtual bool			GetAddonForMap( const char *pszMapName, CUtlString &addonName ) = 0;
	virtual uint64			GetMatchID( void ) = 0;
	virtual void			OnSteamAuthWarning( const char *pszMessage ) = 0;
	virtual void			OnNetworkGameServerActivated( INetworkGameServer *pNetworkGameServer ) = 0;
	virtual uint32			GetSteamGroupAccountID( void ) = 0;
};

#define INTERFACEVERSION_SERVERGAMEENTS			"Source2GameEntities001"

class ISource2GameEntities : public IAppSystem
{
public:
	virtual					~ISource2GameEntities() = 0;

	// This sets a bit in pInfo for each edict in the list that wants to be transmitted to the
	// client specified in pInfo.
	//
	// This is also where an entity can force other entities to be transmitted if it refers to them
	// with ehandles.
	virtual void			CheckTransmit( CCheckTransmitInfo **pInfoInfoList, int nInfoCount, CBitVec<16384> &unionTransmitEdicts,
										   CBitVec<16384> &, const Entity2Networkable_t **pNetworkables,
										   const uint16 *pEntityIndicies, int nEntityIndices ) = 0;
	
	// TERROR: Perform any PVS cleanup before a full update
	virtual void			PrepareForFullUpdate( CPlayerSlot slot ) = 0;

	virtual bool			ShouldClientReceiveStringTableUserData( const INetworkStringTable *pTable, int stringNumber, const CCheckTransmitInfo *pInfo ) = 0;
	
	virtual void			ResetChangeAccessorsSerialNumbersToZero() = 0;
	
	virtual bool			GetAlternateBaselineName( CEntityIndex nEntityIndex, const char **ppszName ) const = 0;

	virtual void			OnPrePackEntities( const CUtlVector<Entity2Networkable_t *> &ents ) const = 0;
};

#define INTERFACEVERSION_SERVERGAMECLIENTS		"Source2GameClients001"

struct ClientReplayEventParams_t
{
	int m_nEventType = 0; // ReplayEventType_t
	float m_flSlowdownLength = 0.0f;
	float m_flSlowdownRate = 1.0f;
	int m_nPrimaryTargetEntIndex = -1;
	float m_flEventTime = 0.0f;
	bool m_bForceUseTheseSettings = false;
};

enum EncryptedMessageKeyType_t
{
	kEncryptedMessageKeyType_None = 0, // not encrypted
	kEncryptedMessageKeyType_Private = 1, // tv_encryptdata_key
	kEncryptedMessageKeyType_Public = 2, // tv_encryptdata_key_pub
};

enum ClientPutInServerType_t
{
	CLIENT_PUT_IN_SERVER_PLAYER = 0,
	CLIENT_PUT_IN_SERVER_FAKE_PLAYER,
	CLIENT_PUT_IN_SERVER_HLTV,
};

// The player's usercmd state that WriteDeltaEntities sends in CSVCMsg_PacketEntities
struct PlayerCommandStatus_t
{
	int m_nLastCmdNumberExecuted;
	int m_nLastCmdNumberReceived;
	// In seconds, -1 before the first usercmd; m_CmdRecvStatus is only sent when it isn't negative
	float m_flLastCmdRecvMargin;

	// Sent as cmd_recv_status, a shorter list ends with 65599
	int m_CmdRecvStatus[8];
	int m_nStarvedCommandTicks;
	int m_nDiscardedCommandTicks;

	// AMNOTE: The server zeroes it, nothing else writes or reads it
	int m_unk101;

	// Set when the player's command queue filled m_nStarvedCommandTicks and m_nDiscardedCommandTicks
	bool m_bHasCommandQueueStats;
};

class ISource2GameClients : public IAppSystem
{
public:
	virtual void			OnClientConnected( CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID, const char *pszAddress, bool bFakePlayer ) = 0;

	// Called when the client attempts to connect (doesn't get called for bots)
	// returning false would reject the connection with the pRejectReason message
	virtual bool			ClientConnect( CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID, bool bLoadGame, CBufferString *pRejectReason ) = 0;

	// Client is connected and should be put in the game
	virtual void			ClientPutInServer( CPlayerSlot slot, char const *pszName, ClientPutInServerType_t type, uint64 xuid ) = 0;

	// Client is going active
	// If bLoadGame is true, don't spawn the player because its state is already setup.
	virtual void			ClientActive( CPlayerSlot slot, bool bLoadGame, const char *pszName, uint64 xuid ) = 0;

	virtual void			ClientFullyConnect( CPlayerSlot slot ) = 0;

	// Client is disconnecting from server
	virtual void			ClientDisconnect( CPlayerSlot slot, ENetworkDisconnectionReason reason,
								const char *pszName, uint64 xuid, const char *pszNetworkID ) = 0;

	// Sets the client index for the client who typed the command into his/her console
	// virtual void			SetCommandClient( CPlayerSlot slot) = 0;

	// The client has typed a command at the console
	virtual void			ClientCommand( CPlayerSlot slot, const CCommand &args ) = 0;

	// Set the the client controller's userinfo cvar value set to the specified address.
	virtual void			ClientSetConVarUserInfoSet( CPlayerSlot slot, ConVarUserInfoSet_t pConVarUserInfoSet ) = 0;
	// A player changed one/several replicated cvars (name etc)
	virtual void			ClientSettingsChanged( CPlayerSlot slot ) = 0;

	// Determine PVS origin and set PVS for the player/viewentity
	virtual void			ClientSetupVisibility( CPlayerSlot slot, vis_info_t *visinfo ) = 0;

	// A block of CUserCmds has arrived from the user, decode them and buffer for execution during player simulation
	// Will be called when CNetworkGameServerBase::GetServerState() > SS_Loading
	// A "paused" argument equals CNetworkGameServerBase::GetServerState() == SS_Paused
	virtual void			ProcessUsercmds( CPlayerSlot slot, const CNetMessagePB<CCLCMsg_Move> &msg, bool paused ) = 0;

	virtual bool			IsPlayerSlotOccupied( CPlayerSlot slot ) = 0;

	virtual bool			IsPlayerAlive( CPlayerSlot slot) = 0;

	virtual int				GetPlayerScore( CPlayerSlot slot ) = 0;

	// Get the ear position for a specified client
	virtual void			ClientEarPosition( CPlayerSlot slot, Vector *pEarOrigin ) = 0;

	// Anything this game .dll wants to add to the bug reporter text (e.g., the entity/model under the picker crosshair)
	//  can be added here
	virtual void			GetBugReportInfo( CBufferString &buf ) = 0;

	// TERROR: A player sent a voice packet
	virtual void			ClientVoice( CPlayerSlot slot ) = 0;
	
	// The client has submitted a keyvalues command
	virtual void			ClientCommandKeyValues( CPlayerSlot slot, KeyValues *pKeyValues ) = 0;
	
	virtual void			ClientDiagnostic( CPlayerSlot slot, CCLCMsg_Diagnostic *pDiagnosticMsg ) = 0;
	
	virtual bool			ClientCanPause( CPlayerSlot slot ) = 0;

	virtual void			HLTVClientFullyConnect( int index, const CSteamID &steamID ) = 0;

	virtual bool			CanHLTVClientConnect( int index, const CSteamID &steamID, int *pRejectReason ) = 0;

	virtual void			StartHLTVServer( CEntityIndex index ) = 0;

	virtual void			SendHLTVStatusMessage( IHLTVServer *, bool, bool, const char *, int, int, int ) = 0;

	virtual IHLTVDirector	*GetHLTVDirector( void ) = 0;

	virtual uint32			GetPlayerTickBase( CPlayerSlot slot ) = 0;
	virtual void			GetPlayerCommandStatus( CPlayerSlot slot, PlayerCommandStatus_t *pOut ) = 0;

	// Handles incoming usermessages from the client
	virtual void			ClientSvcUserMessage( CPlayerSlot slot, int um_type, uint32 size, const void *buf ) = 0;

	// Returns this player hltv delay in ticks
	virtual int				GetPlayerHltvDelay( CPlayerSlot slot, CEntityIndex &replay_ent ) = 0;
	virtual bool			ClientReplayEvent( CPlayerSlot slot, const ClientReplayEventParams_t &params ) = 0;

	// True when the server password is empty or "none", or matches the client's password
	virtual bool			CheckHltvPasswordMatch( const char *pszClientPassword, const char *pszServerPassword, const CSteamID &steamID, void * ) = 0;
	// While a client watches an HLTV replay, it is only sent the messages this returns true for:
	// chat, text, radio and raw audio messages, and user messages carrying radio, audio, rank, XP, quest progress or lobby disconnects
	virtual bool			ShouldSendMessageDuringHltvReplay( const NetMessageInfo_t *pInfo, const CNetMessage *pData ) = 0;
	virtual EncryptedMessageKeyType_t GetMessageEncryptionKey( const NetMessageInfo_t *pInfo, CNetMessage *pData ) = 0;
	// Returns false when the config text uses commands or convars that workshop configs may not use, listing them in pErrors
	virtual bool			ValidateConfigCommands( const char *pszConfig, CBufferString *pErrors ) = 0;
	// Returns false to not kick the client right away for failing Steam authentication; CS2 then stores the reason on its pawn
	virtual bool			ShouldKickClientForSteamAuthFailure( CPlayerSlot slot, ENetworkDisconnectionReason reason ) = 0;
};

#endif