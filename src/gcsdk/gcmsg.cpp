#include "public/gcsdk/gcmsg.h"

namespace GCSDK {
ProtoBufMsgHeader_t::ProtoBufMsgHeader_t()
: m_EMsgFlagged( 0 ), m_cubProtoBufExtHdr( 0 )
{}
}

namespace GCSDK {
ProtoBufMsgHeader_t::ProtoBufMsgHeader_t( MsgType_t eMsg, uint32 cubProtoBufExtHdr )
: m_EMsgFlagged( eMsg | k_EMsgProtoBufFlag ), m_cubProtoBufExtHdr( cubProtoBufExtHdr )
{}
}

namespace GCSDK {
MsgType_t ProtoBufMsgHeader_t::GetEMsg() const
{ return (MsgType_t)( m_EMsgFlagged & ( ~k_EMsgProtoBufFlag ) ); }
}
