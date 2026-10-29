#include "public/tier0/ns_address.h"

const netadr_t & ns_address::GetAddress() const
{ return m_Address; }

const CSteamID& ns_address:: GetSteamID() const
{ return m_ID; }

const uint16 ns_address:: GetRemotePort() const
{ return m_nRemotePort; }

ENSAddressType ns_address:: GetAddressType() const
{ return m_AddressType; }
