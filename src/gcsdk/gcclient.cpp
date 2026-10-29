#include "public/gcsdk/gcclient.h"

namespace GCSDK {
void CGCClient:: Test_AddEvent( CTestEvent *pEvent )
{}
}

namespace GCSDK {
void CGCClient:: Test_CacheSubscribed( const SOID_t &owner )
{}
}

namespace GCSDK {
ISteamGameCoordinator * CGCClient::GetSteamGameCoordinator() const
{ return m_pSteamGameCoordinator; }
}

namespace GCSDK {
bool CGCClient:: BIsGameserver() const
{ return m_bGameserver; }
}

namespace GCSDK {
CGCClientSharedObjectCache * CGCClient::FindSOCache( const SOID_t &owner ) const
{
	auto i = m_mapSOCache.Find( owner );
	return m_mapSOCache.IsValidIndex( i ) ? m_mapSOCache[i] : NULL;
}
}

namespace GCSDK {
void CGCClient:: AddSOCacheListener( ISharedObjectListener *pListener )
{
	if ( !m_vecSOCacheListeners.HasElement( pListener ) )
		m_vecSOCacheListeners.AddToTail( pListener );
}
}

namespace GCSDK {
bool CGCClient:: RemoveSOCacheListener( ISharedObjectListener *pListener )
{ return m_vecSOCacheListeners.FindAndRemove( pListener ); }
}

namespace GCSDK {
const CUtlVector<ISharedObjectListener *> & CGCClient::GetSOCacheListeners() const
{ return m_vecSOCacheListeners; }
}
