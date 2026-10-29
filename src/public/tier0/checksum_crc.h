#ifndef CHECKSUM_CRC_H
#define CHECKSUM_CRC_H
#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#define CRC32_INIT_VALUE 0xFFFFFFFFUL
#define CRC32_XOR_VALUE  0xFFFFFFFFUL

typedef uint32_t CRC32_t;

void CRC32_Init( CRC32_t *pulCRC );

void CRC32_Final( CRC32_t *pulCRC );

PLATFORM_INTERFACE void CRC32_ProcessBuffer( CRC32_t *pulCRC, const void *p, int len );
PLATFORM_INTERFACE CRC32_t CRC32_GetTableEntry( unsigned int slot );

CRC32_t CRC32_ProcessSingleBuffer( const void *p, int len );

#endif