#ifndef _RECIPIENTFILTER_H
#define _RECIPIENTFILTER_H

#ifdef _WIN32
#pragma once
#endif

#include "public/networksystem/inetchannel.h"
#include "public/tier0/bitvec.h"

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter();

	virtual NetChannelBufType_t	GetNetworkBufType( void ) const = 0;
	virtual bool	IsInitMessage( void ) const = 0;

	virtual const CPlayerBitVec &GetRecipients( void ) const = 0;
	virtual CPlayerSlot GetPredictedPlayerSlot( void ) const = 0;
};

class CRecipientFilter : public IRecipientFilter
{
public:
	CRecipientFilter();

	virtual			~CRecipientFilter();

	virtual NetChannelBufType_t	GetNetworkBufType( void ) const;
	virtual bool	IsInitMessage( void ) const;

	virtual const CPlayerBitVec &GetRecipients( void ) const;
	virtual CPlayerSlot GetPredictedPlayerSlot( void ) const;

public:

	void			CopyFrom( const CRecipientFilter &src );

	void			Reset( void );

	void			MakeInitMessage( void );

	void			MakeReliable( void );

	// AMNOTE: The game adds only connected players, the engine skips slots without a connected client
	void			AddAllPlayers( void );

	// AMNOTE: The game adds only a slot with a connected player, and with prediction rules it sets the predicted player slot instead for the suppressed host
	void			AddRecipient( CPlayerSlot slot );

	void			RemoveAllRecipients( void );

	void			RemoveRecipient( CPlayerSlot slot );

	// AMNOTE: The game sets and clears only slots with a connected player
	void			AddPlayersFromBitMask( const CPlayerBitVec &playerbits );

	void			RemovePlayersFromBitMask( const CPlayerBitVec &playerbits );

	bool			IsUsingPredictionRules( void ) const;

	bool			IgnorePredictionCull( void ) const;
	void			SetIgnorePredictionCull( bool ignore );

private:

	CPlayerBitVec		m_Recipients;
	CPlayerSlot			m_nPredictedPlayerSlot;
	NetChannelBufType_t	m_nBufType;
	bool				m_bInitMessage;
	// If using prediction rules, the filter itself suppresses local player
	bool				m_bUsingPredictionRules;
	// If ignoring prediction cull, then external systems can determine
	//  whether this is a special case where culling should not occur
	bool				m_bIgnorePredictionCull;
};

class CSingleUserRecipientFilter : public CRecipientFilter
{
public:
	CSingleUserRecipientFilter( CPlayerSlot slot );
};

class CReliableSingleUserRecipientFilter : public CSingleUserRecipientFilter
{
public:
	CReliableSingleUserRecipientFilter( CPlayerSlot slot );
};

class CBroadcastRecipientFilter : public CRecipientFilter
{
public:
	CBroadcastRecipientFilter( void );
};

class CReliableBroadcastRecipientFilter : public CBroadcastRecipientFilter
{
public:
	CReliableBroadcastRecipientFilter( void );
};

#endif