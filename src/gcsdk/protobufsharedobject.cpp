#include "public/gcsdk/protobufsharedobject.h"

namespace GCSDK {
const ::google::protobuf::Message * CProtoBufSharedObjectBase::GetPObject() const
{ return const_cast<CProtoBufSharedObjectBase *>( this )->GetPObject(); }
}
