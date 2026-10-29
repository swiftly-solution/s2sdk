#include "public/tier0/netadr.h"

netadr_t::netadr_t()
{ SetIP( 0 ); SetPort( 0 ); SetType( NA_IP ); }

netadr_t::netadr_t( uint32_t unIP, uint16_t usPort )
{ SetIP( unIP ); SetPort( usPort ); SetType( NA_IP ); }

netadr_t::netadr_t( const char *pch )
{ SetFromString( pch ); }

void netadr_t::SetIPAndPort( uint32_t unIP, unsigned short usPort )
{ SetIP( unIP ); SetPort( usPort ); }

unsigned int netadr_t::GetIP() const
{ return GetIPNetworkByteOrder(); }

bool netadr_t::operator==( const netadr_t &netadr ) const
{ return (CompareAdr( netadr )); }
