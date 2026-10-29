#include "public/gcsdk/gcclient_sharedobjectcache.h"

namespace GCSDK {
bool CGCClientSharedObjectCache:: BIsInitialized() const
{ return m_bInitialized; }
}

namespace GCSDK {
bool CGCClientSharedObjectCache:: BIsSubscribed() const
{ return m_bSubscribed; }
}

namespace GCSDK {
SOID_t CGCClientSharedObjectCache:: GetOwner() const
{ return m_IDOwner; }
}

namespace GCSDK {
CGCClientSharedObjectTypeCache * CGCClientSharedObjectCache::FindTypeCache( int nClassID ) const
{ return (CGCClientSharedObjectTypeCache *)FindBaseTypeCache( nClassID ); }
}
