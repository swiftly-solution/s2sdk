#include "public/tier0/checksum_crc.h"

void  CRC32_Init( CRC32_t *pulCRC )
{
	*pulCRC = CRC32_INIT_VALUE;
}

void  CRC32_Final( CRC32_t *pulCRC )
{
	*pulCRC ^= CRC32_XOR_VALUE;
}

CRC32_t  CRC32_ProcessSingleBuffer( const void *p, int len )
{
	CRC32_t crc;

	CRC32_Init( &crc );
	CRC32_ProcessBuffer( &crc, p, len );
	CRC32_Final( &crc );

	return crc;
}
