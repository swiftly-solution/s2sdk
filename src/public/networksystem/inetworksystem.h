#ifndef INETWORKSYSTEM_H
#define INETWORKSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "public/tier0/bitbuf.h"
#include "public/tier0/ns_address.h"
#include "public/tier0/platform.h"

#include "inetchannel.h"
#include "inetworkserializer.h"

class SteamNetworkingMessage_t;
class INetPacketMemoryCustomAlloc;
class CPeerToPeerAddress;
class NetScratchBuffer_t;
class ISteamNetworkingUtils;
class ISteamNetworkingSockets;
class ISteamNetworkingSockets;
class ISteamNetworkingSockets;
class ISteamNetworkingMessages;

#ifndef NET_PACKET_ST_DEFINED
#define NET_PACKET_ST_DEFINED
struct NetPacket_t
{
	ns_address m_adrFrom;
	bf_read m_ReadBuffer;
	// The Steam networking message the packet came in, m_pPacketData is its data; released when the packet is reused
	SteamNetworkingMessage_t *m_pSteamMessage;
	uint8_t *m_pPacketData;

	// Owns m_pPacketData for demo and broadcast packets; released when the packet is reused
	INetPacketMemoryCustomAlloc *m_pCustomAlloc;

	// The Steam message's m_usecTimeReceived
	int64_t m_recvUSTime;

	// Where the packet was read in its demo file or broadcast stream, 0 for network packets; copied into each message
	// parsed from it
	int64_t m_nStreamPosition;

	uint32_t m_hConnection;
	int m_nPacketSize;
	int m_nSourceSocket;
};
#endif

class IConnectionlessPacketHandler
{
public:
	virtual	~IConnectionlessPacketHandler( void );

	virtual bool ProcessConnectionlessPacket( const ns_address *addr, bf_read *bf ) = 0;	// process a connectionless packet
};

enum
{
	NS_CLIENT = 0,	// client socket
	NS_SERVER,	// server socket
	NS_HLTV,
	NS_P2P,
	MAX_SOCKETS
};

enum ESteamP2PConnectionOwner {};

class INetworkSystem : public IAppSystem
{
public:
	virtual void InitGameServer() = 0;
	virtual void ShutdownGameServer() = 0;

	virtual int CreateSocket( int, int, int, int, int, const char * ) = 0;
	virtual bool OpenSocket( int sock ) = 0;
	virtual bool ConnectSocket( int sock, const ns_address &adr ) = 0;
	virtual bool IsSocketOpen( int sock ) = 0;
	virtual void CloseSocket( int sock ) = 0;
	virtual bool ConnectLoopback( int sock1, int sock2 ) = 0;
	virtual void SetDefaultBroadcastPort( int port ) = 0;
	virtual void PollSocket( int sock, IConnectionlessPacketHandler * ) = 0;

	virtual void ProcessSocketMessages( int sock ) = 0;

	virtual INetChannel *CreateNetChannel( int sock, const ns_address *adr, HSteamNetConnection hConn, const char *pszName, NetworkCategoryId nSendCategory, NetworkCategoryId nRecvCategory, bool bPlayback ) = 0;
	// AMNOTE: The bool makes the Steam connection close with an app exception end reason and is passed to INetworkChannelNotify::OnShutdownChannel
	virtual void RemoveNetChannel( INetChannel *netchan, bool ) = 0;
	virtual bool RemoveNetChannelByAddress( int sock, const CPeerToPeerAddress &adr ) = 0;

	virtual void SetTime( double time ) = 0;
	virtual void SetTimeScale( float scale ) = 0;
	virtual double GetNetTime() const = 0;

	virtual const char *DescribeSocket( int sock ) = 0;
	virtual bool IsValidSocket( int sock ) = 0;

	virtual bool BufferToBufferCompress( uint8_t *pDest, unsigned int &nDestSize, uint8_t *pIn, unsigned int nInSize ) = 0;
	virtual bool BufferToBufferDecompress( uint8_t *pDest, unsigned int &nDestSize, uint8_t *pIn, unsigned int nInSize ) = 0;

	virtual netadr_t &GetPublicAdr() = 0;
	virtual netadr_t &GetLocalAdr() = 0;
	virtual uint16_t GetUDPPort( int sock ) = 0;
	virtual uint16_t GetUDPPortWithFallback( int sock ) = 0;

	virtual void AddNetworkChannelNotifyCallback( INetworkChannelNotify *pNotify ) = 0;
	virtual void RemoveNetworkChannelNotifyCallback( INetworkChannelNotify *pNotify ) = 0;

	virtual void CloseAllSockets() = 0;

	virtual NetScratchBuffer_t *GetScratchBuffer( void ) = 0;
	virtual void PutScratchBuffer( NetScratchBuffer_t * ) = 0;

	// Plugins must use these instead of SteamGameServerNetworkingSockets(), the engine may use the game's own networking library rather than steamclient
	// Returns SteamNetworkingUtils004 interface
	virtual ISteamNetworkingUtils *GetSteamNetworkUtils() = 0;

	// Returns User SteamNetworkingSockets012 interface
	virtual ISteamNetworkingSockets *GetSteamUserNetworkingSockets() = 0;

	// Returns GameServer SteamNetworkingSockets012 interface
	virtual ISteamNetworkingSockets *GetSteamGameServerNetworkingSockets() = 0;

	// Returns either User or GameServer SteamNetworkingSockets012 interface
	virtual ISteamNetworkingSockets *GetSteamNetworkingSockets() = 0;

	// Returns SteamNetworkingMessages002 interface
	virtual ISteamNetworkingMessages *GetSteamNetworkingMessages() = 0;

	virtual HSteamNetConnection GetSteamNetConnectionForSocket( int sock ) = 0;
	// AMNOTE: Maps the connection's end reason to a disconnection reason: App/AppException codes minus 1000/2000, Local_*/Remote_* codes to LOCALPROBLEM_*/REMOTE_*, eOldState picks the *_CONNECTING variants
	virtual ENetworkDisconnectionReason unk101( const SteamNetConnectionInfo_t *pInfo, ESteamNetworkingConnectionState eOldState ) = 0;

	// The connection is closed with k_ESteamNetConnectionEnd_AppException_Min + reason
	virtual void RejectConnection( HSteamNetConnection hConn, ENetworkDisconnectionReason reason, const char *pszDebug = nullptr ) = 0;

	virtual void RunCallbacks( bool bGameServer, void *pContext, void *pfnCallback ) = 0;
	// AMNOTE: Calls RunCallbacks with the network system's own connection status handler
	virtual void unk201( bool bGameServer ) = 0;

	virtual void InitSteamNetworking() = 0;

	virtual bool IsNetGraphEnabled() = 0;
	virtual HSteamNetConnection EstablishCacheableSharedNetConnection( const ns_address &adr, SteamNetworkingMicroseconds usecKeepAlive ) = 0;

	virtual ~INetworkSystem() = 0;
};

#endif