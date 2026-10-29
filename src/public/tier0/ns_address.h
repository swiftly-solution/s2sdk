#ifndef NS_ADDRESS_H
#define NS_ADDRESS_H

#ifdef _WIN32
#pragma once
#endif

#include "netadr.h"
#include "public/steamworks/steamclientpublic.h"

enum ENSAddressType
{
	kAddressDirect,
	kAddressP2P,
	kAddressProxiedGameServer,
	kAddressProxiedClient,
	kAddressLoopback,

	kAddressMax
};

class ns_address
{
public:
	const netadr_t &GetAddress() const;
	const CSteamID& GetSteamID() const;
	const uint16 GetRemotePort() const;
	ENSAddressType GetAddressType() const;
private:
	netadr_t m_Address;
	CSteamID m_ID;
	uint16 m_nRemotePort;
	int m_Unknown;
	ENSAddressType m_AddressType;
};

#endif