#ifndef INETCHANNEL_H
#define INETCHANNEL_H
#ifdef _WIN32
#pragma once
#endif

#include "networksystemtypes.h"
#include "network_connection.pb.h"
#include "public/steamworks/steamnetworkingtypes.h"
#include "public/tier0/bitbuf.h"
#include "public/tier0/convar.h"
#include "public/tier0/splitscreenslot.h"
#include "public/tier0/utldelegate.h"
#include "public/tier0/utlqueue.h"
#include "inetchannelinfo.h"

class	IDemoRecorderBase;
class	IInstantReplayIntercept;
class	CNetMessage;
class	INetChannelHandler;
class	INetChannel;
class	INetChannelInfo;
class	INetMessageBinder;
class	INetworkMessageProcessingPreFilter;
class	INetworkMessageInternal;
class	INetMessageDispatcher;
class	InstantReplayMessage_t;
class	CUtlSlot;
class	CNETMsg_Tick;
struct	NetPacket_t;

template<typename PROTO_TYPE>
class CNetMessagePB;

class INetworkChannelNotify
{
public:
	virtual void OnShutdownChannel( INetChannel *pChannel, bool ) = 0;
};

class INetworkMessageProcessingPreFilter
{
public:
	// Filter incoming messages from the netchan, return true to filter out (block) the further processing of the message
	virtual bool FilterMessage( const CNetMessage *pData, INetChannel *pChannel ) = 0;
};

class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel( void );

	virtual void	Reset( void ) = 0;
	virtual void	Clear( void ) = 0;
	virtual void	Shutdown( ENetworkDisconnectionReason reason ) = 0;
	
	virtual HSteamNetConnection GetSteamNetConnection( void ) const = 0;
	
	virtual bool	SendNetMessage( const CNetMessage *pData, NetChannelBufType_t bufType ) = 0;
	virtual bool	SendData( bf_write &msg, NetChannelBufType_t bufferType ) = 0;
	virtual int		Transmit( const char *pDebugName, bf_write *data ) = 0;
	virtual void	SetCheckpoint( void ) = 0;
	virtual int		TransmitUpToCheckpoint( const char *pDebugName, bf_write *data ) = 0;
	virtual void	ClearCheckpoint( void ) = 0;

	virtual void	UpdateMessageStats( int msggroup, int bits, bool ) = 0;
	
	// AMNOTE: Moves the encoding stats collected since the last call into the current incoming frame
	virtual void	StoreEncodingStatsInFrame() = 0;
	
	virtual bool	CanPacket( void ) const = 0;
	virtual bool	IsOverflowed( void ) const = 0;
	virtual bool	HasPendingReliableData( void ) = 0;

	// For routing messages to a different handler
	virtual bool	SetActiveChannel( INetChannel *pNewChannel ) = 0;
	
	virtual void	AttachSplitPlayer( CSplitScreenSlot nSlot, INetChannel *pChannel ) = 0;
	virtual void	DetachSplitPlayer( CSplitScreenSlot nSlot ) = 0;

	virtual void	SetMinDataRate( int rate ) = 0;
	virtual void	SetMaxDataRate( int rate ) = 0;

	virtual void	SetTimeout( float seconds, bool bForceExact = false ) = 0;
	virtual bool	IsTimedOut( void ) const = 0;
	virtual void	UpdateLastReceivedTime( void ) = 0;

	// AMNOTE: Jitter in milliseconds above which incoming frames count as late (GetAvgChoke)
	virtual void	SetLateFrameJitterThreshold( float flJitterMs ) = 0;
	// AMNOTE: Also sets the outgoing flow's loss and choke from the message
	virtual void	SetRemoteFramerate( const CNetMessagePB<CNETMsg_Tick> *pMsg ) = 0;
	
	virtual bool	IsRemoteDisconnected( ENetworkDisconnectionReason &reason ) const = 0;

	virtual void	SetNetMessageDispatcher( INetMessageDispatcher *pDispatcher ) = 0;
	virtual INetMessageDispatcher *GetNetMessageDispatcher( void ) const = 0;
	
	virtual void	StartRegisteringMessageHandlers( void ) = 0;
	virtual void	FinishRegisteringMessageHandlers( bool ) = 0;
	
	virtual void	RegisterNetMessageHandlerAbstract( CUtlSlot *nSlot, const CUtlAbstractDelegate &delegate, int nParamCount, INetworkMessageInternal *pNetMessage, int nPriority ) = 0;
	virtual void	UnregisterNetMessageHandlerAbstract( CUtlSlot *nSlot, const CUtlAbstractDelegate &delegate, INetworkMessageInternal *pNetMessage ) = 0;
	
	virtual int		GetNumBitsWritten( NetChannelBufType_t bufferType ) const = 0;
	virtual void	SetDemoRecorder( IDemoRecorderBase *pDemoRecorder ) = 0;
	virtual void	SetInstantReplayIntercept( IInstantReplayIntercept *pInstantReplayIntercept ) = 0;
	virtual bool	IsNull( void ) const = 0;
	virtual bool	ProcessDemoPacket( NetPacket_t *packet ) = 0;
	virtual bool	ProcessMessages() = 0;
	
	virtual void	InstallMessageFilter( INetworkMessageProcessingPreFilter *pFilter ) = 0;
	virtual void	UninstallMessageFilter( INetworkMessageProcessingPreFilter *pFilter ) = 0;
	
	virtual void	PostReceivedNetMessage( CNetMessage *pData ) = 0;
	virtual void	PostReceivedNetMessageToFront( CNetMessage *pData ) = 0;
	virtual void	InsertReplayMessage( InstantReplayMessage_t &msg ) = 0;
	virtual bool	HasQueuedNetMessages( int nMessageId ) const = 0;

	virtual void	SetPendingDisconnect( ENetworkDisconnectionReason reason ) = 0;
	virtual ENetworkDisconnectionReason GetPendingDisconnect( void ) const = 0;

	virtual void	SuppressTransmit( bool suppress ) = 0;
	virtual bool	IsSuppressingTransmit( void ) const = 0;
	
	// nSendFlags are k_nSteamNetworkingSend_* flags
	virtual EResult	SendMessageRaw( const void *pData, uint32 cbData, int nSendFlags ) = 0;
	
	virtual void	NotifyOutboundTick( int nTick ) = 0;
	// AMNOTE: Returns the queue of received messages that ProcessMessages dispatches
	virtual CUtlQueue<CNetMessage *> *GetReceivedMessageQueue() = 0;
	// AMNOTE: Writes the incoming flow's dropped and late frame percentages, which the peer reads in SetRemoteFramerate
	virtual void	WriteFrameQualityToTick( CNetMessagePB<CNETMsg_Tick> *pMsg ) = 0;
	// AMNOTE: Received messages beyond the convar's value are reported
	virtual void	SetQueuedMessageLimitConVar( CConVar<int> *pQueuedMessageLimit ) = 0;
};

#endif