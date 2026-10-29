//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Holds the CGCClient class
//
//=============================================================================

#ifndef GCCLIENT_H
#define GCCLIENT_H

#ifdef _WIN32
#pragma once
#endif

#include "public/steamworks/steam_api.h"
#include "public/steamworks/isteamgamecoordinator.h"
#include "public/tier0/threadtools.h"
#include "public/tier0/utlleanvector.h"
#include "public/tier0/utlmap.h"
#include "gcclient_sharedobjectcache.h"
#include "jobmgr.h"

class CTestEvent;

namespace GCSDK
{

//-----------------------------------------------------------------------------
// Purpose: The game's connection to the GC and its shared object caches
//-----------------------------------------------------------------------------
class CGCClient
{
public:
	virtual ~CGCClient();

	// AMNOTE: Both are empty in CS2
	virtual void Test_AddEvent( CTestEvent *pEvent );
	virtual void Test_CacheSubscribed( const SOID_t &owner );

	ISteamGameCoordinator *GetSteamGameCoordinator() const;
	bool BIsGameserver() const;

	/// Find the shared object cache for the specified owner, NULL if there is none
	CGCClientSharedObjectCache *FindSOCache( const SOID_t &owner ) const;

	/// Adds a listener to the shared object caches.  A call to re-add
	/// a listener that's already listening is harmlessly ignored.
	// AMNOTE: The listener gets no calls for the caches that are already subscribed
	void AddSOCacheListener( ISharedObjectListener *pListener );

	/// Removes a listener for the shared object caches.  Returns true
	/// if we were listening and were successfully removed, false
	/// otherwise
	bool RemoveSOCacheListener( ISharedObjectListener *pListener );

	const CUtlVector<ISharedObjectListener *> &GetSOCacheListeners() const;

protected:
	ISteamUser *m_pSteamUser;
	ISteamGameServer *m_pSteamGameServer;
	ISteamGameCoordinator *m_pSteamGameCoordinator;
	ISteamUtils *m_pSteamUtils;
	CUtlLeanVector<uint8> m_memMsg;

	// local job handling
	CJobMgr m_JobMgr;

	// Shared object caches
	CUtlOrderedMap<SOID_t, CGCClientSharedObjectCache *, CDefLess<SOID_t>, unsigned short> m_mapSOCache;

	// AMNOTE: The listeners of every cache, see ISharedObjectListener
	CUtlVector<ISharedObjectListener *> m_vecSOCacheListeners;

	uint8 m_unk001[72];
	bool m_bGameserver;
	uint8 m_unk101[47];

	CCallback<CGCClient, GCMessageAvailable_t, false> m_CallbackGCMessageAvailable;
	CCallback<CGCClient, SteamServersDisconnected_t, false> m_CallbackSteamServersDisconnected;
	CCallback<CGCClient, SteamServerConnectFailure_t, false> m_CallbackSteamServerConnectFailure;
	CCallback<CGCClient, SteamServersConnected_t, false> m_CallbackSteamServersConnected;
};

} // namespace GCSDK

#endif // GCCLIENT_H
