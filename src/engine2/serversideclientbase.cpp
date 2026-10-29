#include "public/engine2/serversideclientbase.h"

CPlayerSlot CServerSideClientBase:: GetPlayerSlot() const
{ return m_nClientSlot; }

CPlayerUserId CServerSideClientBase:: GetUserID() const
{ return m_UserID; }

CEntityIndex CServerSideClientBase:: GetEntityIndex() const
{ return m_nEntityIndex; }

const char * CServerSideClientBase::GetClientName() const
{ return m_Name.Get(); }

const char * CServerSideClientBase::GetNetworkIDString() const
{ return m_NetworkIDString.Get(); }

CSteamID CServerSideClientBase:: GetClientSteamID() const
{ return m_SteamID; }

const netadr_t * CServerSideClientBase::GetRemoteAddress() const
{ return &m_Addr.GetAddress(); }

INetChannel * CServerSideClientBase::GetNetChannel() const
{ return m_pNetChannel; }

CNetworkGameServerBase * CServerSideClientBase::GetServer() const
{ return m_pServer; }

ClientSlotReservation_t CServerSideClientBase:: GetSlotReservation() const
{ return m_nSlotReservation; }

SignonState_t CServerSideClientBase:: GetSignonState() const
{ return m_nSignonState; }

bool CServerSideClientBase:: IsConnected() const
{ return m_nSignonState >= SIGNONSTATE_CONNECTED; }

bool CServerSideClientBase:: IsSpawned() const
{ return m_nSignonState >= SIGNONSTATE_NEW; }

bool CServerSideClientBase:: IsActive() const
{ return m_nSignonState == SIGNONSTATE_FULL; }

bool CServerSideClientBase:: IsFakePlayer() const
{ return m_bFakePlayer; }

bool CServerSideClientBase:: IsHLTV() const
{ return m_bIsHLTV; }

bool CServerSideClientBase:: IsFullyAuthenticated() const
{ return m_bFullyAuthenticated; }
