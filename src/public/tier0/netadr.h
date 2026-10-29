#ifndef NETADR_H
#define NETADR_H
#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "bitbuf.h"

typedef enum
{ 
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

struct netadr_t
{
public:
	netadr_t();
	netadr_t( uint32_t unIP, uint16_t usPort );
	netadr_t( const char *pch );

	// invalids Address
	DLL_CLASS_IMPORT void	Clear();

	DLL_CLASS_IMPORT void	SetType( netadrtype_t type );
	DLL_CLASS_IMPORT void	SetPort( unsigned short port );
	DLL_CLASS_IMPORT bool	SetFromSockadr( const struct sockaddr *s );

	DLL_CLASS_IMPORT void	SetIP( uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4 );
	// unIP is in host order (little-endian)
	DLL_CLASS_IMPORT void	SetIP( uint32_t unIP );
	void   SetIPAndPort( uint32_t unIP, unsigned short usPort );

	// if bUseDNS is true then do a DNS lookup if needed
	DLL_CLASS_IMPORT bool	SetFromString( const char *pch, bool bUseDNS = false );

	DLL_CLASS_IMPORT bool	CompareAdr( const netadr_t &a, bool onlyBase = false ) const;
	DLL_CLASS_IMPORT bool	CompareClassBAdr( const netadr_t &a ) const;
	DLL_CLASS_IMPORT bool	CompareClassCAdr( const netadr_t &a ) const;

	DLL_CLASS_IMPORT netadrtype_t	GetType() const;
	DLL_CLASS_IMPORT unsigned short	GetPort() const;

	// returns xxx.xxx.xxx.xxx:ppppp
	DLL_CLASS_IMPORT const char *ToString( bool onlyBase = false ) const;
	DLL_CLASS_IMPORT void ToString( char *pchBuffer, uint32_t unBufferSize, bool onlyBase = false ) const;

	DLL_CLASS_IMPORT void			ToSockadr( struct sockaddr *s ) const;

	DLL_CLASS_IMPORT unsigned int	GetIPHostByteOrder() const;
	DLL_CLASS_IMPORT unsigned int	GetIPNetworkByteOrder() const;
	unsigned int	GetIP() const;

	// true, if this is the localhost IP 
	DLL_CLASS_IMPORT bool	IsLocalhost() const;
	// true if engine loopback buffers are used
	DLL_CLASS_IMPORT bool	IsLoopback() const;
	// true, if this is a private LAN IP
	DLL_CLASS_IMPORT bool	IsReservedAdr() const;
	// ip & port != 0
	DLL_CLASS_IMPORT bool	IsValid() const;
	// ip != 0
	DLL_CLASS_IMPORT bool	IsBaseAdrValid() const;

	DLL_CLASS_IMPORT void    SetFromSocket( int hSocket );

	DLL_CLASS_IMPORT bool    Serialize( bf_write &writeBuf );
	DLL_CLASS_IMPORT bool    Unserialize( bf_read &readBuf );

	bool operator==( const netadr_t &netadr ) const;
	DLL_CLASS_IMPORT bool operator<( const netadr_t &netadr ) const;

public:	// members are public to avoid to much changes

	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
};

#endif