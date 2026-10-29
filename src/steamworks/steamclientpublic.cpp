#include "public/steamworks/steamclientpublic.h"

CSteamID::CSteamID()
{
	m_steamid.m_comp.m_unAccountID = 0;
	m_steamid.m_comp.m_EAccountType = k_EAccountTypeInvalid;
	m_steamid.m_comp.m_EUniverse = k_EUniverseInvalid;
	m_steamid.m_comp.m_unAccountInstance = 0;
}

CSteamID::CSteamID( uint32 unAccountID, EUniverse eUniverse, EAccountType eAccountType )
{
	Set( unAccountID, eUniverse, eAccountType );
}

CSteamID::CSteamID( uint32 unAccountID, unsigned int unAccountInstance, EUniverse eUniverse, EAccountType eAccountType )
{
#if defined(_SERVER) && defined(Assert)
	Assert( ( k_EAccountTypeIndividual != eAccountType ) || ( unAccountInstance == k_unSteamUserDefaultInstance ) );	// enforce that for individual accounts, instance is always 1
#endif // _SERVER
	InstancedSet( unAccountID, unAccountInstance, eUniverse, eAccountType );
}

CSteamID::CSteamID( uint64 ulSteamID )
{
	SetFromUint64( ulSteamID );
}

void CSteamID::Set( uint32 unAccountID, EUniverse eUniverse, EAccountType eAccountType )
{
	m_steamid.m_comp.m_unAccountID = unAccountID;
	m_steamid.m_comp.m_EUniverse = eUniverse;
	m_steamid.m_comp.m_EAccountType = eAccountType;

	if ( eAccountType == k_EAccountTypeClan || eAccountType == k_EAccountTypeGameServer )
	{
		m_steamid.m_comp.m_unAccountInstance = 0;
	}
	else
	{
		m_steamid.m_comp.m_unAccountInstance = k_unSteamUserDefaultInstance;
	}
}

void CSteamID::InstancedSet( uint32 unAccountID, uint32 unInstance, EUniverse eUniverse, EAccountType eAccountType )
{
	m_steamid.m_comp.m_unAccountID = unAccountID;
	m_steamid.m_comp.m_EUniverse = eUniverse;
	m_steamid.m_comp.m_EAccountType = eAccountType;
	m_steamid.m_comp.m_unAccountInstance = unInstance;
}

void CSteamID::FullSet( uint64 ulIdentifier, EUniverse eUniverse, EAccountType eAccountType )
{
	m_steamid.m_comp.m_unAccountID = ( ulIdentifier & k_unSteamAccountIDMask );						// account ID is low 32 bits
	m_steamid.m_comp.m_unAccountInstance = ( ( ulIdentifier >> 32 ) & k_unSteamAccountInstanceMask );			// account instance is next 20 bits
	m_steamid.m_comp.m_EUniverse = eUniverse;
	m_steamid.m_comp.m_EAccountType = eAccountType;
}

void CSteamID::SetFromUint64( uint64 ulSteamID )
{
	m_steamid.m_unAll64Bits = ulSteamID;
}

void CSteamID::Clear()
{
		m_steamid.m_comp.m_unAccountID = 0;
		m_steamid.m_comp.m_EAccountType = k_EAccountTypeInvalid;
		m_steamid.m_comp.m_EUniverse = k_EUniverseInvalid;
		m_steamid.m_comp.m_unAccountInstance = 0;
	}

uint64 CSteamID::ConvertToUint64() const
{
	return m_steamid.m_unAll64Bits;
}

uint64 CSteamID::GetStaticAccountKey() const
{
	// note we do NOT include the account instance (which is a dynamic property) in the static account key
	return (uint64) ( ( ( (uint64) m_steamid.m_comp.m_EUniverse ) << 56 ) + ((uint64) m_steamid.m_comp.m_EAccountType << 52 ) + m_steamid.m_comp.m_unAccountID );
}

void CSteamID::CreateBlankAnonLogon( EUniverse eUniverse )
{
	m_steamid.m_comp.m_unAccountID = 0;
	m_steamid.m_comp.m_EAccountType = k_EAccountTypeAnonGameServer;
	m_steamid.m_comp.m_EUniverse = eUniverse;
	m_steamid.m_comp.m_unAccountInstance = 0;
}

void CSteamID::CreateBlankAnonUserLogon( EUniverse eUniverse )
{
	m_steamid.m_comp.m_unAccountID = 0;
	m_steamid.m_comp.m_EAccountType = k_EAccountTypeAnonUser;
	m_steamid.m_comp.m_EUniverse = eUniverse;
	m_steamid.m_comp.m_unAccountInstance = 0;
}

bool CSteamID::BBlankAnonAccount() const
{
	return m_steamid.m_comp.m_unAccountID == 0 && BAnonAccount() && m_steamid.m_comp.m_unAccountInstance == 0;
}

bool CSteamID::BGameServerAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeGameServer || m_steamid.m_comp.m_EAccountType == k_EAccountTypeAnonGameServer;
}

bool CSteamID::BPersistentGameServerAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeGameServer;
}

bool CSteamID::BAnonGameServerAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeAnonGameServer;
}

bool CSteamID::BContentServerAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeContentServer;
}

bool CSteamID::BClanAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeClan;
}

bool CSteamID::BChatAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeChat;
}

bool CSteamID::IsLobby() const
{
	return ( m_steamid.m_comp.m_EAccountType == k_EAccountTypeChat )
		&& ( m_steamid.m_comp.m_unAccountInstance & k_EChatInstanceFlagLobby );
}

bool CSteamID::BIndividualAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeIndividual || m_steamid.m_comp.m_EAccountType == k_EAccountTypeConsoleUser;
}

bool CSteamID::BAnonAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeAnonUser || m_steamid.m_comp.m_EAccountType == k_EAccountTypeAnonGameServer;
}

bool CSteamID::BAnonUserAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeAnonUser;
}

bool CSteamID::BConsoleUserAccount() const
{
	return m_steamid.m_comp.m_EAccountType == k_EAccountTypeConsoleUser;
}

void CSteamID::SetAccountID( uint32 unAccountID )
{ m_steamid.m_comp.m_unAccountID = unAccountID; }

void CSteamID::SetAccountInstance( uint32 unInstance )
{ m_steamid.m_comp.m_unAccountInstance = unInstance; }

AccountID_t CSteamID::GetAccountID() const
{ return m_steamid.m_comp.m_unAccountID; }

uint32 CSteamID::GetUnAccountInstance() const
{ return m_steamid.m_comp.m_unAccountInstance; }

EAccountType CSteamID::GetEAccountType() const
{ return ( EAccountType ) m_steamid.m_comp.m_EAccountType; }

EUniverse CSteamID::GetEUniverse() const
{ return m_steamid.m_comp.m_EUniverse; }

void CSteamID::SetEUniverse( EUniverse eUniverse )
{ m_steamid.m_comp.m_EUniverse = eUniverse; }

bool CSteamID::operator==( const CSteamID &val ) const
{ return m_steamid.m_unAll64Bits == val.m_steamid.m_unAll64Bits; }

bool CSteamID::operator!=( const CSteamID &val ) const
{ return !operator==( val ); }

bool CSteamID::operator<( const CSteamID &val ) const
{ return m_steamid.m_unAll64Bits < val.m_steamid.m_unAll64Bits; }

bool CSteamID::operator>( const CSteamID &val ) const
{ return m_steamid.m_unAll64Bits > val.m_steamid.m_unAll64Bits; }

bool CSteamID::IsValid() const
{
	if ( m_steamid.m_comp.m_EAccountType <= k_EAccountTypeInvalid || m_steamid.m_comp.m_EAccountType >= k_EAccountTypeMax )
		return false;
	
	if ( m_steamid.m_comp.m_EUniverse <= k_EUniverseInvalid || m_steamid.m_comp.m_EUniverse >= k_EUniverseMax )
		return false;

	if ( m_steamid.m_comp.m_EAccountType == k_EAccountTypeIndividual )
	{
		if ( m_steamid.m_comp.m_unAccountID == 0 || m_steamid.m_comp.m_unAccountInstance != k_unSteamUserDefaultInstance )
			return false;
	}

	if ( m_steamid.m_comp.m_EAccountType == k_EAccountTypeClan )
	{
		if ( m_steamid.m_comp.m_unAccountID == 0 || m_steamid.m_comp.m_unAccountInstance != 0 )
			return false;
	}

	if ( m_steamid.m_comp.m_EAccountType == k_EAccountTypeGameServer )
	{
		if ( m_steamid.m_comp.m_unAccountID == 0 )
			return false;
		// Any limit on instances?  We use them for local users and bots
	}
	return true;
}

CGameID::CGameID()
{
	m_gameID.m_nType = k_EGameIDTypeApp;
	m_gameID.m_nAppID = k_uAppIdInvalid;
	m_gameID.m_nModID = 0;
}

CGameID::CGameID( uint64 ulGameID )
{
	m_ulGameID = ulGameID;
}

CGameID::CGameID( int32 nAppID )
{
	m_ulGameID = 0;
	m_gameID.m_nAppID = nAppID;
}

CGameID::CGameID( uint32 nAppID )
{
	m_ulGameID = 0;
	m_gameID.m_nAppID = nAppID;
}

CGameID::CGameID( uint32 nAppID, uint32 nModID, CGameID::EGameIDType nType )
{
	m_gameID.m_nAppID = nAppID;
	m_gameID.m_nModID = nModID;
	m_gameID.m_nType = nType;
}

CGameID::CGameID( const CGameID &that )
{
	m_ulGameID = that.m_ulGameID;
}

CGameID& CGameID::operator=( const CGameID & that )
{
	m_ulGameID = that.m_ulGameID;
	return *this;
}

uint64 CGameID::ToUint64() const
{
	return m_ulGameID;
}

uint64 * CGameID::GetUint64Ptr()
{
	return &m_ulGameID;
}

void CGameID::Set( uint64 ulGameID )
{
	m_ulGameID = ulGameID;
}

bool CGameID::IsMod() const
{
	return ( m_gameID.m_nType == k_EGameIDTypeGameMod );
}

bool CGameID::IsShortcut() const
{
	return ( m_gameID.m_nType == k_EGameIDTypeShortcut );
}

bool CGameID::IsP2PFile() const
{
	return ( m_gameID.m_nType == k_EGameIDTypeP2P );
}

bool CGameID::IsSteamApp() const
{
	return ( m_gameID.m_nType == k_EGameIDTypeApp );
}

uint32 CGameID::ModID() const
{
	return m_gameID.m_nModID;
}

bool CGameID::operator == ( const CGameID &rhs ) const
{
	return m_ulGameID == rhs.m_ulGameID;
}

bool CGameID::operator != ( const CGameID &rhs ) const
{
	return !(*this == rhs);
}

bool CGameID::operator < ( const CGameID &rhs ) const
{
	return ( m_ulGameID < rhs.m_ulGameID );
}

bool CGameID::IsValid() const
{
	// each type has it's own invalid fixed point:
	switch( m_gameID.m_nType )
	{
	case k_EGameIDTypeApp:
		return m_gameID.m_nAppID != k_uAppIdInvalid;

	case k_EGameIDTypeGameMod:
		return m_gameID.m_nAppID != k_uAppIdInvalid && (m_gameID.m_nModID & 0x80000000);

	case k_EGameIDTypeShortcut:
		return m_gameID.m_nAppID == k_uAppIdInvalid
			&& (m_gameID.m_nModID & 0x80000000)
			&& m_gameID.m_nModID >= (5000 | 0x80000000); // k_unMaxExpectedLocalAppId - shortcuts are pushed beyond that range

	case k_EGameIDTypeP2P:
		return m_gameID.m_nAppID == k_uAppIdInvalid && (m_gameID.m_nModID & 0x80000000);

	default:
		return false;
	}

}

void CGameID::Reset()
{
	m_ulGameID = 0;
}
