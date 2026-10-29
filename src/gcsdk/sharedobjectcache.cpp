#include "public/gcsdk/sharedobjectcache.h"

namespace GCSDK {
int CSharedObjectTypeCache:: GetTypeID() const
{ return m_nTypeID; }
}

namespace GCSDK {
uint32 CSharedObjectTypeCache:: GetCount() const
{ return m_vecObjects.Count(); }
}

namespace GCSDK {
CSharedObject * CSharedObjectTypeCache::GetObject( uint32 nObj )
{ return m_vecObjects[nObj]; }
}

namespace GCSDK {
const CSharedObject * CSharedObjectTypeCache::GetObject( uint32 nObj ) const
{ return m_vecObjects[nObj]; }
}

namespace GCSDK {
CSharedObjectTypeCache * CSharedObjectCache::FindBaseTypeCache( int nClassID ) const
{
	FOR_EACH_VEC( m_vecTypeCaches, i )
	{
		if ( m_vecTypeCaches[i]->GetTypeID() == nClassID )
			return m_vecTypeCaches[i];
	}

	return NULL;
}
}

namespace GCSDK {
void CSharedObjectCache:: SetVersion( uint64 ulVersion )
{ m_ulVersion = ulVersion; }
}

namespace GCSDK {
uint64 CSharedObjectCache:: GetVersion() const
{ return m_ulVersion; }
}

namespace GCSDK {
void CSharedObjectCache:: MarkDirty()
{}
}

namespace GCSDK {
CSharedObjectTypeCache * CSharedObjectCache::GetTypeCacheByIndex( int nIndex )
{ return m_vecTypeCaches.IsValidIndex( nIndex ) ? m_vecTypeCaches[nIndex] : NULL; }
}

namespace GCSDK {
int CSharedObjectCache:: GetTypeCacheCount() const
{ return m_vecTypeCaches.Count(); }
}
