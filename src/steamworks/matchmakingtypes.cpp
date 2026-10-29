#include "public/steamworks/matchmakingtypes.h"

MatchMakingKeyValuePair_t::MatchMakingKeyValuePair_t()
{ m_szKey[0] = m_szValue[0] = 0; }

MatchMakingKeyValuePair_t::MatchMakingKeyValuePair_t( const char *pchKey, const char *pchValue )
{
	strncpy( m_szKey, pchKey, sizeof(m_szKey) ); // this is a public header, use basic c library string funcs only!
	m_szKey[ sizeof( m_szKey ) - 1 ] = '\0';
	strncpy( m_szValue, pchValue, sizeof(m_szValue) );
	m_szValue[ sizeof( m_szValue ) - 1 ] = '\0';
}

servernetadr_t::servernetadr_t()
: m_usConnectionPort( 0 ), m_usQueryPort( 0 ), m_unIP( 0 )
{}

void servernetadr_t::operator=( const servernetadr_t &that )
{
	m_usConnectionPort = that.m_usConnectionPort;
	m_usQueryPort = that.m_usQueryPort;
	m_unIP = that.m_unIP;
}

void servernetadr_t::Init( unsigned int ip, uint16 usQueryPort, uint16 usConnectionPort )
{
	m_unIP = ip;
	m_usQueryPort = usQueryPort;
	m_usConnectionPort = usConnectionPort;
}

uint16 servernetadr_t::GetQueryPort() const
{
	return m_usQueryPort;
}

void servernetadr_t::SetQueryPort( uint16 usPort )
{
	m_usQueryPort = usPort;
}

uint16 servernetadr_t::GetConnectionPort() const
{
	return m_usConnectionPort;
}

void servernetadr_t::SetConnectionPort( uint16 usPort )
{
	m_usConnectionPort = usPort;
}

uint32 servernetadr_t::GetIP() const
{
	return m_unIP;
}

void servernetadr_t::SetIP( uint32 unIP )
{
	m_unIP = unIP;
}

const char * servernetadr_t::ToString( uint32 unIP, uint16 usPort ) const
{
	static char s[4][64];
	static int nBuf = 0;
	unsigned char *ipByte = (unsigned char *)&unIP;
#ifdef VALVE_BIG_ENDIAN
	snprintf(s[nBuf], sizeof( s[nBuf] ), "%u.%u.%u.%u:%i", (int)(ipByte[0]), (int)(ipByte[1]), (int)(ipByte[2]), (int)(ipByte[3]), usPort );
#else
	snprintf(s[nBuf], sizeof( s[nBuf] ), "%u.%u.%u.%u:%i", (int)(ipByte[3]), (int)(ipByte[2]), (int)(ipByte[1]), (int)(ipByte[0]), usPort );
#endif
	const char *pchRet = s[nBuf];
	++nBuf;
	nBuf %= ( (sizeof(s)/sizeof(s[0])) );
	return pchRet;
}

const char* servernetadr_t::GetConnectionAddressString() const
{
	return ToString( m_unIP, m_usConnectionPort );
}

const char* servernetadr_t::GetQueryAddressString() const
{
	return ToString( m_unIP, m_usQueryPort );	
}

bool servernetadr_t::operator<(const servernetadr_t &netadr) const
{
	return ( m_unIP < netadr.m_unIP ) || ( m_unIP == netadr.m_unIP && m_usQueryPort < netadr.m_usQueryPort );
}

gameserveritem_t::gameserveritem_t()
{
	m_szGameDir[0] = m_szMap[0] = m_szGameDescription[0] = m_szServerName[0] = 0;
	m_bHadSuccessfulResponse = m_bDoNotRefresh = m_bPassword = m_bSecure = false;
	m_nPing = m_nAppID = m_nPlayers = m_nMaxPlayers = m_nBotPlayers = m_ulTimeLastPlayed = m_nServerVersion = 0;
	m_szGameTags[0] = 0;
}

const char* gameserveritem_t::GetName() const
{
	// Use the IP address as the name if nothing is set yet.
	if ( m_szServerName[0] == 0 )
		return m_NetAdr.GetConnectionAddressString();
	else
		return m_szServerName;
}

void gameserveritem_t::SetName( const char *pName )
{
	strncpy( m_szServerName, pName, sizeof( m_szServerName ) );
	m_szServerName[ sizeof( m_szServerName ) - 1 ] = '\0';
}
