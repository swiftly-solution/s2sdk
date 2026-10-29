#ifndef SERVERSIDECLIENTBASE_H
#define SERVERSIDECLIENTBASE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/bitvec.h"
#include "public/const.h"
#include "public/networksystem/inetchannel.h"
#include "public/tier0/playerslot.h"
#include "public/entity2/entityidentity.h"
#include "public/networksystem/inetworksystem.h"
#include "public/networksystem/inetworkmessages.h"
#include "public/steamworks/steamclientpublic.h"
#include "public/tier0/annotations.h"
#include "public/tier0/tslist.h"
#include "public/tier0/circularbuffer.h"
#include "public/tier0/convar.h"
#include "public/tier0/utlcommon.h"
#include "public/tier0/utldict.h"
#include "public/tier0/utlsignalslot.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlvector.h"
#include "public/networksystem/netmessages.h"
#include "netmessages.pb.h"

class CClientFrame;
class CFrameSnapshot;
class CNetworkGameServerBase;
class INetworkStringTable;
class ITvStreamSink;
struct HltvReplayParams_t;
struct PlayerCommandStatus_t;

class NetMessagePacketStart;
class NetMessagePacketEnd;
class NetMessageConnectionClosed;
class NetMessageConnectionCrashed;
class NetMessageSplitscreenUserChanged;

template <class T>
class CDelayedCallBase;

// AMNOTE: While the client's send job runs, the deferred methods (Disconnect, SendNetMessage, SendInitialSpawnGroups,
// StopHltvReplay, ...) are queued in m_DelayedCalls
enum ClientSendJobState_t : uint8
{
	CLIENT_SEND_JOB_IDLE = 0,
	CLIENT_SEND_JOB_RUNNING,
	// AMNOTE: The job is done, but its queued calls aren't flushed yet
	CLIENT_SEND_JOB_FINISHED,
};

// m_nConnectionTypeFlags bits; 0 is a fake client too
enum ConnectionTypeFlags_t : uint8
{
	// Counted in CNetworkGameServerBase::m_nLoopbackClientCount
	CONNECTION_TYPE_LOOPBACK = (1 << 0),
	CONNECTION_TYPE_LOCALHOST = (1 << 1),
	CONNECTION_TYPE_REMOTE = (1 << 2),
	// AMNOTE: Connect treats the client as fake; no engine caller found that passes it
	CONNECTION_TYPE_FAKE = (1 << 3),
};

// AMNOTE: While it's not CLIENT_SLOT_RECYCLABLE, the slot shows as reserved and keeps its address on disconnect
enum ClientSlotReservation_t : int
{
	CLIENT_SLOT_RECYCLABLE = 0,
	// AMNOTE: INetworkGameServer::SetRecyclePlayerSlot switches between this and CLIENT_SLOT_RECYCLABLE,
	// but leaves CLIENT_SLOT_CREATED as is
	CLIENT_SLOT_NOT_RECYCLABLE,
	// AMNOTE: Set by CServerSideClientBase::Create
	CLIENT_SLOT_CREATED,
};

class CServerSideClientBase : public CUtlSlot, public INetworkChannelNotify, public INetworkMessageProcessingPreFilter
{
public:
	struct BuildServerInfoMessageAsync_t;

	// AMNOTE: Net spike trace, written to netspike.txt. Each sent net message adds a record of its description and size while m_nMinWarningBytes isn't 0
	struct NetSpikeTrace_t
	{
		struct Record_t
		{
			CUtlString m_Description;
			int m_nBits;
		};

		CUtlVector<Record_t> m_Records;

		// AMNOTE: Set from the command line when the client is created, the records are discarded when the snapshot is smaller
		int m_nMinWarningBytes;

		// AMNOTE: Bits written when the snapshot started, and when the last record was added
		int m_nStartBit;
		int m_nCurBit;
	};

	virtual ~CServerSideClientBase() = 0;

	// AMNOTE: The first argument is unused
	virtual void Connect( int, const char *pszName, CPlayerUserId nUserID, INetChannel *pNetChannel, uint8 nConnectionTypeFlags, uint32 nChallengeNumber ) = 0;
	virtual void Inactivate( const char *pszAddons ) = 0;
	virtual void Reactivate( CPlayerSlot nSlot ) = 0;
	virtual void SetServer( CNetworkGameServerBase *pServer ) = 0;
	virtual void Reconnect() = 0;
	virtual void Disconnect( ENetworkDisconnectionReason reason, const char *pszInternalReason ) = 0;
	virtual bool CheckConnect() = 0;

	// AMNOTE: Clears the client and gives it the SteamID and name without a connection, returns its slot
	virtual CPlayerSlot Create( CSteamID steamID, const char *pszName ) = 0;

	virtual void SetRate( int nRate ) = 0;
	virtual void SetUpdateRate( float flUpdateRate ) = 0;
	virtual int GetRate() = 0;

	virtual void Clear() = 0;

	// Returning false counts towards kicking the client for too many failed commands
	virtual bool ExecuteStringCommand( const CNetMessagePB<CNETMsg_StringCmd> &msg ) = 0;
	virtual bool SendNetMessage( const CNetMessage *pData, NetChannelBufType_t bufType = BUF_DEFAULT ) = 0;
	virtual bool FilterMessage( const CNetMessage *pData, INetChannel *pChannel ) override = 0;

	virtual void ClientPrintf( PRINTF_FORMAT_STRING const char *pszFormat, ... ) = 0;

	virtual bool IsFakeClient() const = 0;
	// Is an actual human player or split screen player (not a bot and not a HLTV slot)
	virtual bool IsHumanPlayer() const = 0;
	virtual bool IsHearingClient( CPlayerSlot nSlot ) const = 0;
	virtual bool IsProximityHearingClient( CPlayerSlot nSlot ) const = 0;
	virtual bool IsLowViolenceClient() const = 0;
	virtual bool IsSplitScreenUser() const = 0;

	virtual bool ProcessTick( const CNetMessagePB<CNETMsg_Tick> &msg ) = 0;
	virtual bool ProcessStringCmd( const CNetMessagePB<CNETMsg_StringCmd> &msg ) = 0;
	virtual bool ProcessSetConVar( const CNetMessagePB<CNETMsg_SetConVar> &msg ) = 0;
	virtual bool ProcessSignonState( const CNetMessagePB<CNETMsg_SignonState> &msg ) = 0;
	virtual bool ProcessSpawnGroup_LoadCompleted( const CNetMessagePB<CNETMsg_SpawnGroup_LoadCompleted> &msg ) = 0;
	virtual bool ProcessClientInfo( const CNetMessagePB<CCLCMsg_ClientInfo> &msg ) = 0;
	virtual bool ProcessBaselineAck( const CNetMessagePB<CCLCMsg_BaselineAck> &msg ) = 0;
	virtual bool ProcessLoadingProgress( const CNetMessagePB<CCLCMsg_LoadingProgress> &msg ) = 0;
	virtual bool ProcessSplitPlayerConnect( const CNetMessagePB<CCLCMsg_SplitPlayerConnect> &msg ) = 0;
	virtual bool ProcessSplitPlayerDisconnect( const CNetMessagePB<CCLCMsg_SplitPlayerDisconnect> &msg ) = 0;
	virtual bool ProcessCmdKeyValues( const CNetMessagePB<CBaseCmdKeyValues<CCLCMsg_CmdKeyValues>> &msg ) = 0;
	virtual bool ProcessRconServerDetails( const CNetMessagePB<CCLCMsg_RconServerDetails> &msg ) = 0;
	virtual bool ProcessDiagnostic( const CNetMessagePB<CCLCMsg_Diagnostic> &msg ) = 0;
	virtual bool ProcessMove( const CNetMessagePB<CCLCMsg_Move> &msg ) = 0;
	virtual bool ProcessVoiceData( const CNetMessagePB<CCLCMsg_VoiceData> &msg ) = 0;
	virtual bool ProcessRespondCvarValue( const CNetMessagePB<CCLCMsg_RespondCvarValue> &msg ) = 0;
	virtual bool ProcessPacketStart( const CNetMessagePB<NetMessagePacketStart> &msg ) = 0;
	virtual bool ProcessPacketEnd( const CNetMessagePB<NetMessagePacketEnd> &msg ) = 0;
	virtual bool ProcessConnectionClosed( const CNetMessagePB<NetMessageConnectionClosed> &msg ) = 0;
	virtual bool ProcessConnectionCrashed( const CNetMessagePB<NetMessageConnectionCrashed> &msg ) = 0;
	virtual bool ProcessSplitscreenUserChanged( const CNetMessagePB<NetMessageSplitscreenUserChanged> &msg ) = 0;
	virtual bool ProcessHltvReplay( const CNetMessagePB<CCLCMsg_HltvReplay> &msg ) = 0;
	virtual bool ProcessHltvFixupOperatorTick( const CNetMessagePB<CCLCMsg_HltvFixupOperatorTick> &msg ) = 0;
	virtual bool ProcessUserMessage( const CNetMessagePB<CSVCMsg_UserMessage> &msg ) = 0;

	// Registers the message handlers above on the channel
	virtual void ConnectionStart( INetChannel *pNetChannel ) = 0;

	// AMNOTE: The first collects the initial spawn groups into the info and sets m_bInitialSpawnGroupsSent, the second runs as
	// a delayed call, builds the server info from it and sends the spawn groups through SendInitialSpawnGroups. They
	// share a name so that both compilers order the three slots like the engine
	virtual void BuildServerInfoMessageAsync( CUtlVector<void *> &spawnGroups ) = 0;
	virtual void SendInitialSpawnGroups( const CUtlVector<void *> &spawnGroups, bool bAsync ) = 0;
	virtual void BuildServerInfoMessageAsync( BuildServerInfoMessageAsync_t &info ) = 0;

	virtual bool UpdateAcknowledgedFramecount( int nTick ) = 0;
	virtual bool ShouldSendMessages() = 0;
	virtual void UpdateSendState() = 0;

	virtual const CMsgPlayerInfo &GetPlayerInfo() const = 0;

	// Applies the name, rate and other settings from the userinfo convars
	virtual void UpdateUserSettings() = 0;
	// Calls SetRate with the "rate" userinfo convar, 5000 when it's unset
	virtual void UpdateRateFromUserInfo() = 0;

	virtual CClientFrame *GetClientFrame( int nTick ) = 0;

	virtual void SendSignonData() = 0;
	virtual void SpawnPlayer() = 0;
	virtual void ActivatePlayer() = 0;

	virtual void SetName( const char *pszName ) = 0;
	virtual void SetUserCVar( const char *pszConVar, const char *pszValue ) = 0;

	virtual void FreeBaselines() = 0;

	virtual CServerSideClientBase *GetSplitScreenOwner() = 0;

	// AMNOTE: Counts the players sharing this client's connection, itself included (non-null m_SplitScreenUsers).
	// Split screen users forward to the client they're attached to
	virtual int GetSplitScreenPlayerCount() = 0;

	virtual bool ShouldReceiveStringTableUserData( const INetworkStringTable *pTable, int nStringNumber ) = 0;

	virtual void GetPlayerCommandStatus( PlayerCommandStatus_t *pOut ) = 0;

	virtual bool StartHltvReplay( const HltvReplayParams_t &params ) = 0;
	// bForce runs the stop even when no replay is active
	virtual void StopHltvReplay( bool bForce ) = 0;
	virtual int GetHltvReplayDelay() = 0;
	virtual bool CanStartHltvReplay( int nDelay ) = 0;
	virtual void ResetHltvReplayRequestTime() = 0;
	virtual const char *GetHltvReplayStats() = 0;

	// Waits for the client's pending send job
	virtual void Await() = 0;

	// AMNOTE: Called once the delayed calls queued during the send job are flushed, and when the send job starts
	virtual void OnSendJobFinished() = 0;
	virtual void OnSendJobStarted() = 0;

	virtual bool ProcessSignonStateMsg( int nState, int nSpawnCount ) = 0;
	virtual void PerformDisconnection( ENetworkDisconnectionReason reason ) = 0;

public:
	CPlayerSlot GetPlayerSlot() const;
	CPlayerUserId GetUserID() const;
	CEntityIndex GetEntityIndex() const;
	const char *GetClientName() const;
	const char *GetNetworkIDString() const;
	CSteamID GetClientSteamID() const;
	const netadr_t *GetRemoteAddress() const;
	INetChannel *GetNetChannel() const;
	CNetworkGameServerBase *GetServer() const;

	ClientSlotReservation_t GetSlotReservation() const;
	SignonState_t GetSignonState() const;
	bool IsConnected() const;
	bool IsSpawned() const;
	bool IsActive() const;
	bool IsFakePlayer() const;

	bool IsHLTV() const;
	bool IsFullyAuthenticated() const;

public:
	// AMNOTE: "BOT", "STEAM_ID_PENDING", "STEAM_ID_LAN" or the SteamID as text
	CUtlString m_NetworkIDString;
	CUtlString m_Name;
	CPlayerSlot m_nClientSlot;
	CEntityIndex m_nEntityIndex;
	CNetworkGameServerBase *m_pServer;
	INetChannel *m_pNetChannel;
	// ConnectionTypeFlags_t; ConnectClient gives CONNECTION_TYPE_LOOPBACK | CONNECTION_TYPE_LOCALHOST for the loopback
	// address or a client in the same process, CONNECTION_TYPE_LOCALHOST or CONNECTION_TYPE_REMOTE otherwise
	uint8 m_nConnectionTypeFlags;

	ClientSendJobState_t m_nSendJobState;

	// AMNOTE: Set on construction, cleared by OnSendJobStarted and set again by OnSendJobFinished,
	// so it's false while the client's send job runs
	bool m_bSendJobIdle;
	SignonState_t m_nSignonState;
	bool m_bSplitScreenUser;
	// AMNOTE: A split screen user goes through the full disconnection only when it's set, otherwise the server sends "ss_remove"
	bool m_bSplitAllowFastDisconnect;
	CSplitScreenSlot m_nSplitScreenPlayerSlot;
	CServerSideClientBase *m_SplitScreenUsers[4];
	CServerSideClientBase *m_pAttachedTo;
	bool m_unk301;

	ClientSlotReservation_t m_nSlotReservation;

	bool m_bFakePlayer;
	// AMNOTE: Full updates are logged with their reason while it's set
	bool m_bLogFullUpdate;
	// AMNOTE: Snapshots sent without a delta frame
	int m_nFullUpdateCount;
	CPlayerUserId m_UserID;
	bool m_bReceivedPacket;
	CSteamID m_SteamID;
	// AMNOTE: Kept when the client is cleared, a reconnecting player gets the slot that has their SteamID back
	CSteamID m_LastSteamID;
	// AMNOTE: The SteamID of the auth session, ended on disconnect
	CSteamID m_AuthSessionSteamID;
	CSteamID m_FriendsID;
	ns_address m_Addr;
	// AMNOTE: Kept on disconnect, a reconnecting player gets the slot that has their address back
	ns_address m_LastAddr;

	// AMNOTE: Set from a server-wide counter on disconnect, the disconnected slot with the lowest one is reused first
	int m_nDisconnectSequence;

	KeyValues *m_pUserInfo;
	// AMNOTE: Unknown userinfo convars that were already warned about
	CUtlDict<empty_t, int> m_UnknownUserInfoConVars;
	bool m_bConVarsChanged;

	// AMNOTE: Every userinfo convar is accepted until it's set, then only known ones
	bool m_bInitialConVarsApplied;
	bool m_bIsHLTV;

	// AMNOTE: Set when the server removes the TV sink
	bool m_bTvStreamSinkRemoved;

	// AMNOTE: The HLTV client's downstream TV sink, SendNetMessage forwards to it
	ITvStreamSink *m_pTvStreamSink;

	uint32 m_nSendTableCRC;
	uint32 m_nChallengeNumber;

	// AMNOTE: Server tick the server info was built at
	int m_nSignonTick;

	int m_nDeltaTick;

	// AMNOTE: Tick of the last delta snapshot sent reliably while sv_disable_reliable_delta_retransmit is set,
	// snapshots delta from it as if the client acknowledged it
	int m_nLastReliableDeltaTick;
	int m_nStringTableAckTick;
	// AMNOTE: Tick the string tables flagged (1 << 2) were last sent at. Those tables go to the client as reliable messages of
	// their own and, while m_nDeltaTick is above 0, as changes since this tick. 0 when no table has the flag, as with
	// all of CS2's own tables
	int m_nLastReliableStringTableTick;
	CFrameSnapshot *m_pLastSnapshot;
	CUtlVector<SpawnGroupHandle_t> m_LoadedSpawnGroups;
	CMsgPlayerInfo m_PlayerInfo;
	CFrameSnapshot *m_pBaseline;

	// AMNOTE: Reset to -1 once the client acknowledges a later tick
	int m_nBaselineUpdateTick;

	CBitVec<MAX_EDICTS> m_BaselinesSent;
	int m_nBaselineUsed;
	int m_nLoadingProgress;
	int m_nForceWaitForTick;

	// AMNOTE: History of the acknowledged ticks, logged when they arrive out of order
	CCircularBuffer m_AckTickHistory;

	bool m_bLowViolence;

	// AMNOTE: The client connected through a loopback address
	bool m_bLoopback;
	bool m_bFullyAuthenticated;
	bool m_bClanOfficer;
	bool m_bClanMember;
	bool m_bInitialSpawnGroupsSent;
	float m_flNextMessageTime;
	float m_flSnapshotInterval;

	// AMNOTE: Plat_FloatTime when Steam canceled the auth ticket, -1 when there's none pending
	float m_flAuthTicketCanceledTime;

	// AMNOTE: Plat_FloatTime of the last "userinfo ignored" warning, which is printed at most every 10 seconds
	float m_flLastUserInfoIgnoredTime;

	// AMNOTE: Holds a CNetMessagePB<CSVCMsg_PacketEntities>, which the SDK only declares as an abstract class
	alignas( CNetMessagePB<CSVCMsg_PacketEntities> ) uint8 m_PacketEntitiesMsg[sizeof( CNetMessagePB<CSVCMsg_PacketEntities> )];

	CTSQueue<CDelayedCallBase<CServerSideClientBase> *> m_DelayedCalls;

	// AMNOTE: The send job runs the delayed calls of the first, which it swaps in from the second, where they're queued meanwhile
	CUtlVector<CDelayedCallBase<CServerSideClientBase> *> m_SendJobDelayedCalls;
	CUtlVector<CDelayedCallBase<CServerSideClientBase> *> m_PendingSendJobDelayedCalls;

	// ProcessStringCmd's rate limit: at most m_nMaxCommandsPerWindow commands
	// and m_nMaxBytesPerWindow bytes per m_nMaxTicksPerWindow ticks
	int m_nMaxTicksPerWindow;
	int m_nMaxCommandsPerWindow;
	uint64 m_nMaxBytesPerWindow;
	int m_nWindowStartTick;
	int m_nWindowCommands;
	uint64 m_nWindowBytes;

	NetSpikeTrace_t m_NetSpikeTrace;

	// Failed commands within a second of each other, the client is kicked after 16
	int m_nFailedCommands;
	double m_flLastFailedCommandTime;

	// AMNOTE: Set when the signon state goes back to connected, a connected signon state message is processed only while it's set
	bool m_bConnectedSignonStatePending;

	// Values of the client's userinfo convars
	ConVarUserInfoSet_t m_ConVarUserInfoSet;
};

#endif // SERVERSIDECLIENTBASE_H
