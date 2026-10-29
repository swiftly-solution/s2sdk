#include "public/tier0/recipientfilter.h"

IRecipientFilter::~IRecipientFilter()
{}

CRecipientFilter::CRecipientFilter()
:
	m_nPredictedPlayerSlot( -1 ),
	m_nBufType( BUF_DEFAULT ),
	m_bInitMessage( false ),
	m_bUsingPredictionRules( false ),
	m_bIgnorePredictionCull( false )
{
}

CRecipientFilter::~CRecipientFilter()
{}

NetChannelBufType_t CRecipientFilter::GetNetworkBufType( void ) const
{ return m_nBufType; }

bool CRecipientFilter::IsInitMessage( void ) const
{ return m_bInitMessage; }

const CPlayerBitVec & CRecipientFilter::GetRecipients( void ) const
{ return m_Recipients; }

CPlayerSlot CRecipientFilter::GetPredictedPlayerSlot( void ) const
{ return m_nPredictedPlayerSlot; }

void CRecipientFilter::CopyFrom( const CRecipientFilter &src )
{
	m_Recipients = src.m_Recipients;
	m_nBufType = src.GetNetworkBufType();
	m_bInitMessage = src.IsInitMessage();
	m_bUsingPredictionRules = src.m_bUsingPredictionRules;
	m_bIgnorePredictionCull = src.m_bIgnorePredictionCull;
}

void CRecipientFilter::Reset( void )
{
	m_Recipients.ClearAll();
	m_nBufType = BUF_DEFAULT;
	m_bInitMessage = false;
	m_bUsingPredictionRules = false;
	m_bIgnorePredictionCull = false;
}

void CRecipientFilter::MakeInitMessage( void )
{ m_bInitMessage = true; }

void CRecipientFilter::MakeReliable( void )
{ m_nBufType = BUF_RELIABLE; }

void CRecipientFilter::AddAllPlayers( void )
{
	m_Recipients.SetAll();
	m_nPredictedPlayerSlot = CPlayerSlot(-1);
}

void CRecipientFilter::AddRecipient( CPlayerSlot slot )
{
	if ( slot.Get() != -1 )
		m_Recipients.Set( slot.Get() );
}

void CRecipientFilter::RemoveAllRecipients( void )
{
	m_Recipients.ClearAll();
}

void CRecipientFilter::RemoveRecipient( CPlayerSlot slot )
{
	if ( slot.Get() != -1 )
		m_Recipients.Clear( slot.Get() );
}

void CRecipientFilter::AddPlayersFromBitMask( const CPlayerBitVec &playerbits )
{
	for ( int i = 0; i < m_Recipients.GetNumDWords(); i++ )
		m_Recipients.SetDWord( i, m_Recipients.GetDWord( i ) | playerbits.GetDWord( i ) );
}

void CRecipientFilter::RemovePlayersFromBitMask( const CPlayerBitVec &playerbits )
{
	for ( int i = 0; i < m_Recipients.GetNumDWords(); i++ )
		m_Recipients.SetDWord( i, m_Recipients.GetDWord( i ) & ~playerbits.GetDWord( i ) );
}

bool CRecipientFilter::IsUsingPredictionRules( void ) const
{ return m_bUsingPredictionRules; }

bool CRecipientFilter::IgnorePredictionCull( void ) const
{ return m_bIgnorePredictionCull; }

void CRecipientFilter::SetIgnorePredictionCull( bool ignore )
{ m_bIgnorePredictionCull = ignore; }

CSingleUserRecipientFilter::CSingleUserRecipientFilter( CPlayerSlot slot )
{
	AddRecipient( slot );
}

CReliableSingleUserRecipientFilter::CReliableSingleUserRecipientFilter( CPlayerSlot slot )
:
	CSingleUserRecipientFilter( slot )
{
	MakeReliable();
}

CBroadcastRecipientFilter::CBroadcastRecipientFilter( void )
{
	AddAllPlayers();
}

CReliableBroadcastRecipientFilter::CReliableBroadcastRecipientFilter( void )
{
	MakeReliable();
}
