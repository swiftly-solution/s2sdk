//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef NETWORKGAMESERVERBASE_H
#define NETWORKGAMESERVERBASE_H

#ifdef _WIN32
#pragma once
#endif

#include "inetworkgameserver.h"
#include "gamesessionconfiguration.h"
#include "public/steamworks/steamclientpublic.h"
#include "serverclass.h"
#include "public/networksystem/inetworksystem.h"
#include "public/tier0/icvar.h"
#include "public/tier0/playerslot.h"
#include "public/tier0/bitbuf.h"
#include "public/tier0/jobthread.h"
#include "public/tier0/threadtools.h"
#include "public/tier0/utlhashmap.h"
#include "public/tier0/utldict.h"
#include "public/tier0/utlhashtable.h"
#include "public/tier0/utlleanvector.h"
#include "public/tier0/utllinkedlist.h"
#include "public/tier0/utlmap.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlstringtoken.h"
#include "public/tier0/utlvector.h"
#include "public/worldrenderer/worldgroupid.h"

class KeyValues;
class KeyValues3;
class CServerSideClientBase;
class CSVCMsg_ServerInfo_t;
class C2S_CONNECT_Message;
class CCompressedResourceManifestRefCounted;
class CFrameSnapshotManager;
class CNetworkServerSpawnGroup;
class CNetworkServerSpawnGroupCreatePrerequisites;
class CSerializedEntity;
class CTvDirectDemoRecorder;
class INetworkStringTable;
class INetworkStringTableContainer;
class ISource2WorldSession;

template <class T>
class CDelayedCallBase;

// The auth protocol GetChallengeType sends in S2C_CHALLENGE and clients send back as C2S_CONNECT's auth_protocol
enum ChallengeType_t : int
{
	PROTOCOL_HASHEDCDKEY = 2,
	PROTOCOL_STEAM = 3,
};

// The instance baselines of one ServerClass with a baseline counter
struct CounterInstanceBaselines_t
{
	CThreadFastMutex m_Mutex;
	// Counter name to its index in m_BaselineIndices
	CUtlDict<int> m_CounterIndices;
	// The "instancebaseline" table index per counter, -1 until its baseline is made
	CUtlVector<int> m_BaselineIndices;
};

typedef int PauseGroup_t;

abstract_class CNetworkGameServerBase : public INetworkGameServer, protected IConnectionlessPacketHandler, public IConVarListener
{
public:
	virtual ~CNetworkGameServerBase() = 0;

	virtual void	SetMaxClients( int nMaxClients ) = 0;
	
	// AMNOTE: Creates or reuses a client without a connection, returns its slot
	virtual CPlayerSlot CreateClient( CPlayerSlot slot, CSteamID steamID, const char *pszName ) = 0;

	virtual bool	ProcessConnectionlessPacket( const ns_address *addr, bf_read *bf ) = 0; // process a connectionless packet

	virtual void	OnConVarCreated( ConVarRefAbstract *pNewCvar ) = 0;
	virtual void	OnConCommandCreated( ConCommand *pNewCommand ) = 0;

	virtual CPlayerUserId GetPlayerUserId( CPlayerSlot slot ) = 0;
	virtual const char *GetPlayerNetworkIDString( CPlayerSlot slot ) = 0;
	
	// Returns udp port of this server instance
	virtual uint16		GetUDPPort() = 0;
	// Returns hostname of this server instance
	virtual const char *GetHostName() = 0;

	// Sums up across all the connected players.
	virtual void	GetNetStats( float &inflow, float &outflow ) = 0;

	virtual void	FillKV3ServerInfo( KeyValues3 *out ) = 0;

	virtual bool	IsHLTV() = 0;
	
	virtual bool	IsPausable( PauseGroup_t ) = 0;

	// Returns sv_password cvar value, if it's set to "none" nullptr would be returned!
	virtual const char *GetPassword() = 0;

	virtual void	RemoveClientFromGame( CServerSideClientBase *pClient, ENetworkDisconnectionReason reason ) = 0;

	virtual void	FillServerInfo( CSVCMsg_ServerInfo_t *pServerInfo ) = 0;
	virtual void	UserInfoChanged( CPlayerSlot slot ) = 0;

	// The first argument is unused. ppBaseline gets nullptr when no baseline is stored for the class;
	// a class without an instance baseline index is a fatal error.
	virtual bool	GetClassBaseline( void *, ServerClass *pClass, void **ppBaseline ) = 0;

	virtual void	StartHLTVMaster() = 0;

	virtual CServerSideClientBase *ConnectClient( const char *pszName, ns_address *pAddr, uint32 steam_handle, C2S_CONNECT_Message *pConnectMsg,
												  const char *pszChallenge, const byte *pAuthTicket, int nAuthTicketLength, bool bIsLowViolence ) = 0;
	virtual CServerSideClientBase *CreateNewClient( CPlayerSlot slot ) = 0;
	
	// PROTOCOL_HASHEDCDKEY is refused for remote clients while Steam authentication is up, other values than the two are refused
	virtual bool	FinishCertificateCheck( const ns_address *pAddr, int socket, ChallengeType_t nAuthProtocol ) = 0;

	virtual ChallengeType_t	GetChallengeType( const ns_address &addr ) = 0;
	virtual bool	CheckPassword( const ns_address &addr, const char *password ) = 0;

	virtual void	CalculateCPUUsage() = 0;

public:
	struct WorldGroup_t
	{
		CUtlStringToken m_Token;
		CUtlString m_Name;
		int m_nRefCount;
	};

	// AMNOTE: Prefer IVEngineServer2's world group methods (FindWorldGroupByName, IsWorldGroupValid, ...) instead of direct access
	struct WorldGroupList_t
	{
		// A world group's WorldGroupId_t is its index here, with bit 31 set when m_bClient is set
		WorldGroupId_t GetWorldGroupId( int nIndex ) const;

		// nullptr when hWorldGroup isn't one of this list's groups
		const WorldGroup_t *GetWorldGroup( WorldGroupId_t hWorldGroup ) const;
		WorldGroupId_t FindWorldGroupByName( const char *pszName ) const;

		bool IsWorldGroupValid( WorldGroupId_t hWorldGroup ) const;

		CUtlVector<WorldGroup_t> m_Groups;
		bool m_bClient;
	};

	struct PendingSpawnGroupUnload_t
	{
		SpawnGroupHandle_t m_hSpawnGroup;
		ESpawnGroupUnloadOption m_eUnloadOption;
	};

	struct PendingSplitScreenRemoval_t
	{
		CServerSideClientBase *m_pClient;
		CServerSideClientBase *m_pSplitScreenClient;
	};

	WorldGroupList_t m_WorldGroups;
	server_state_t m_nServerState;
	int m_nRefCount;
	int m_nSocket;
	int m_nServerTick;
	int m_nMinClientsLimit;
	int m_nMaxClientsLimit;

	// Bit 1 << PauseGroup_t for each group that paused the server; it is paused while any is set
	int m_nPausedGroups;

	CGlobalVars m_Globals;
	GameSessionConfiguration_t m_GameSessionConfig;
	CUtlString m_szMapName;
	CUtlString m_szAddonName;
	CUtlString m_szSkyName;

	// AMNOTE: Nothing uses it
	CUtlString m_unk101;

	KeyValues *m_pLaunchOptions;
	INetworkStringTableContainer *m_pServerStringTables;
	INetworkStringTableContainer *m_pClientStringTables;
	INetworkStringTable *m_pInstanceBaselineTable;
	INetworkStringTable *m_pUserInfoTable;
	INetworkStringTable *m_pServerQueryInfoTable;

	CFrameSnapshotManager *m_pFrameSnapshotManager;

	// The instance baselines GetClassBaseline looks up, by the class's "instancebaseline" table index
	CThreadFastMutex m_InstanceBaselinesMutex;
	CUtlHashMap<int, CSerializedEntity *> m_InstanceBaselines;

	bf_write m_Signon;
	CUtlLeanVector<uint8> m_SignonBuffer;
	bool m_bIsBackgroundMap;
	CUtlVector<CServerSideClientBase *> m_Clients;
	CCompressedResourceManifestRefCounted *m_pCompressedGameSessionManifest;

	// The instance baselines of entities with a baseline counter, made on first use per ServerClass
	CThreadFastMutex m_CounterInstanceBaselinesMutex;
	CUtlHashtable<ServerClass *, CounterInstanceBaselines_t *, PointerHashFunctor> m_CounterInstanceBaselines;

	SpawnGroupHandle_t m_hActiveSpawnGroup;
	int m_nMaxClients;
	int m_nServerCount;
	int m_nMaxClasses;
	int m_nServerClassBits;
	time_t m_nCreationTime;
	bool m_bChangelevelPending;
	// Set on the new server once it took over the previous level's clients; ActivateServer then keeps the Steam game
	// server as it is, unless m_bPreserveSteamID is set
	bool m_bChangelevelFinished;
	bool m_bPreserveSteamID;
	bool m_bHibernating;

	CUtlVector<CDelayedCallBase<CNetworkGameServerBase> *> m_DelayedCalls;
	CUtlLinkedList<CNetworkServerSpawnGroupCreatePrerequisites *, uint16> m_LoadingSpawnGroups;
	CUtlOrderedMap<SpawnGroupHandle_t, CNetworkServerSpawnGroup *, CDefLess<SpawnGroupHandle_t>, uint16> m_SpawnGroups;
	// Unloaded once m_nPendingSpawnGroupManifestUpdates is 0
	CUtlVector<PendingSpawnGroupUnload_t> m_PendingSpawnGroupUnloads;
	int m_nPendingSpawnGroupManifestUpdates;

	// The spawn group that loads the resource system's just-in-time manifests, rebuilt when their count changes
	SpawnGroupHandle_t m_hJustInTimeSpawnGroup;

	// The Steam game server's game data and tags
	CUtlVector<char> m_GameData;
	CUtlString m_szGameTags;
	int m_nDisconnectSequence;
	CUtlVector<PendingSplitScreenRemoval_t> m_PendingSplitScreenRemovals;

	// The client whose packet is being processed, from its ProcessPacketStart to its ProcessPacketEnd
	CServerSideClientBase *m_pHostClient;
	IGameSpawnGroupMgr *m_pSpawnGroupMgr;
	ISource2WorldSession *m_pWorldSession;
	HGameResourceManifest m_hGameSessionManifest;
	SpawnGroupHandle_t m_hNextSpawnGroup;

	// These point into the server reservation; IsReserved checks for a non-zero cookie
	uint64 *m_pReservationCookie;
	float *m_pflLastClientLeftTime;

	// AMNOTE: Nothing uses the float it points to
	float *m_unk201;

	float *m_pflReservationExpireTime;

	int m_nMaxSpawnGroupCreationSequence;
	// The just-in-time manifest count m_hJustInTimeSpawnGroup was built for
	int m_nJustInTimeManifestCount;
	// The serial in each player slot's CPlayerUserId (slot | serial << 8), 0xFFFF while the slot was never used
	uint16 m_PlayerUserIdSerials[64];
	// Incremented each time UpdateGameData produces different game data
	int m_nGameDataChangeCount;
	// The game type config's "Members/numSlots"
	int m_nNumSlots;

	float m_flLastCPUUsageSampleTime;
	float m_flCPUUsage;
	// Set by IVEngineServer2::SetTimescale, 1 by default
	float m_flTimescale;
	// The peer list is resent to the clients
	bool m_bUpdatePeerList;
	bool m_bMultiplayer;

	// AMNOTE: Nothing uses it
	uint8 m_unk301;

	// The "ticks_per_interval" and "framecomputetime_ms_per_interval" histograms of FillKV3ServerInfo
	int m_nTicksPerIntervalBuckets;
	uint64 m_TicksPerIntervalBuckets[11];
	uint64 m_nTicksPerIntervalSum;
	uint64 m_nTicksPerIntervalCount;
	int m_nFrameComputeTimeBuckets;
	uint64 m_FrameComputeTimeBuckets[61];
	double m_flFrameComputeTimeSum;
	uint64 m_nFrameComputeTimeCount;

	// From the reservation ReserveServerForQueuedGame takes; SourceTV broadcasts send it as their match_id
	uint64 m_nMatchID;
	CTvDirectDemoRecorder *m_pDirectDemoRecorder;
	uint64 m_nUserInfoCvarsTotalByteSize;
	// The connected clients in this process (loopback address, or localhost passing the same-process check)
	int m_nLoopbackClientCount;
	// The job running ISource2Server::GameFrame on another thread ("ServerGameFrame"), queued by ServerBeginSimulate
	CSmartPtr<CThreadedJob> m_pServerGameFrameJob;
};



typedef CNetworkGameServerBase IServer;

#endif // NETWORKGAMESERVERBASE_H
