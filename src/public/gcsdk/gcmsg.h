//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Message headers for messages sent to and from the GC
//
//=============================================================================

#ifndef GCMSG_H
#define GCMSG_H

#ifdef _WIN32
#pragma once
#endif

#include "msgbase.h"

namespace GCSDK
{

// Wire format of a protobuf message: this header, a serialized CMsgProtoBufHeader of
// m_cubProtoBufExtHdr bytes, then the serialized body
#pragma pack( push, 1 )
struct ProtoBufMsgHeader_t
{
	int32			m_EMsgFlagged;			// High bit should be set to indicate this message header type is in use.  The rest of the bits indicate message type.
	uint32			m_cubProtoBufExtHdr;	// Size of the extended header which is a serialized protobuf object.  Indicates where it ends and the serialized body protobuf begins.

	ProtoBufMsgHeader_t();
	ProtoBufMsgHeader_t( MsgType_t eMsg, uint32 cubProtoBufExtHdr );
	MsgType_t GetEMsg() const;
};
#pragma pack( pop )

} // namespace GCSDK

#endif // GCMSG_H
