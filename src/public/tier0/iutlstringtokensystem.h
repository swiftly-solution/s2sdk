#ifndef IUTLSTRINGTOKENSYSTEM_H
#define IUTLSTRINGTOKENSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

// AMNOTE: See VStringTokenSystem001
// Interact with stringtokendatabase.txt
PLATFORM_INTERFACE void RegisterStringToken( uint32 nHashCode, const char *pStart, const char *pEnd = NULL, bool bExtraAddToDatabase = true );

#endif // IUTLSTRINGTOKENSYSTEM_H
