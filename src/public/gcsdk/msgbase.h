//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base types for messages sent to and from the GC
//
//=============================================================================

#ifndef MSGBASE_H
#define MSGBASE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"

namespace GCSDK
{

// used for message types in GCSDK where we don't have the actual enum
typedef uint32 MsgType_t;

// Set in the message type of every message that has a CMsgProtoBufHeader
const uint32 k_EMsgProtoBufFlag = 0x80000000;

} // namespace GCSDK

#endif // MSGBASE_H
