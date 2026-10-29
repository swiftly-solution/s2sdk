//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef TIER2_H
#define TIER2_H

#if defined( _WIN32 )
#pragma once
#endif

#include "public/tier1/tier1.h"
struct ResourceManifestDesc_t;

//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 2 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier2Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier2Libraries();


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier2 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 > 
class CTier2AppSystem : public CTier1AppSystem< IInterface, ConVarFlag >
{
	typedef CTier1AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual bool Connect( CreateInterfaceFn factory ) 
	{
		if ( !BaseClass::Connect( factory ) )
			return false;

		ConnectTier2Libraries( &factory, 1 );
		return true;
	}

	virtual InitReturnVal_t Init()
	{
		InitReturnVal_t nRetVal = BaseClass::Init();
		if ( nRetVal != INIT_OK )
			return nRetVal;

		return INIT_OK;
	}

	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER2;
	}

	virtual void Shutdown()
	{
		BaseClass::Shutdown();
	}

	virtual void Disconnect() 
	{
		DisconnectTier2Libraries();
		BaseClass::Disconnect();
	}
private:
	CUtlVector<ResourceManifestDesc_t *> m_manualManifests;
	int m_nAppSysTier2LibraryConnects;
};


//-----------------------------------------------------------------------------
// Used by the resource system for fast resource frame counter
//-----------------------------------------------------------------------------
extern uint32 g_nResourceFrameCount;


#endif // TIER2_H

