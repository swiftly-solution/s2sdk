#ifndef MESSAGES_BASE_H
#define MESSAGES_BASE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

PLATFORM_INTERFACE void Msg( const char* pMsg, ... );
PLATFORM_INTERFACE void Warning( const char *pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void DevMsg( int level, const char* pMsg, ... ) FMTFUNCTION( 2, 3 );
PLATFORM_INTERFACE void DevWarning( int level, const char *pMsg, ... ) FMTFUNCTION( 2, 3 );
PLATFORM_OVERLOAD void DevMsg( const char* pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_OVERLOAD void DevWarning( const char *pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_OVERLOAD void ConMsg( const char* pMsg, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void COM_TimestampedLog( char const *fmt, ... ) FMTFUNCTION( 1, 2 );

#endif