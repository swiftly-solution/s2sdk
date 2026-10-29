#include "public/steamworks/steamtypes.h"

bool SteamIPAddress_t:: IsSet() const
{ 
	if ( k_ESteamIPTypeIPv4 == m_eType )
	{
		return m_unIPv4 != 0;
	}
	else 
	{
		return m_ipv6Qword[0] !=0 || m_ipv6Qword[1] != 0; 
	}
}

SteamIPAddress_t SteamIPAddress_t:: IPv4Any()
{
	SteamIPAddress_t ipOut;
	ipOut.m_eType = k_ESteamIPTypeIPv4;
	ipOut.m_unIPv4 = 0;

	return ipOut;
}

SteamIPAddress_t SteamIPAddress_t:: IPv6Any()
{
	SteamIPAddress_t ipOut;
	ipOut.m_eType = k_ESteamIPTypeIPv6;
	ipOut.m_ipv6Qword[0] = 0;
	ipOut.m_ipv6Qword[1] = 0;

	return ipOut;
}

SteamIPAddress_t SteamIPAddress_t:: IPv4Loopback()
{
	SteamIPAddress_t ipOut;
	ipOut.m_eType = k_ESteamIPTypeIPv4;
	ipOut.m_unIPv4 = 0x7f000001;

	return ipOut;
}

SteamIPAddress_t SteamIPAddress_t:: IPv6Loopback()
{
	SteamIPAddress_t ipOut;
	ipOut.m_eType = k_ESteamIPTypeIPv6;
	ipOut.m_ipv6Qword[0] = 0;
	ipOut.m_ipv6Qword[1] = 0;
	ipOut.m_rgubIPv6[15] = 1;

	return ipOut;
}
