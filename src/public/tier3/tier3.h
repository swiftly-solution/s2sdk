//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef TIER3_H
#define TIER3_H

#if defined( _WIN32 )
#pragma once
#endif

#include "public/tier2/tier2.h"


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier3 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 > 
class CTier3AppSystem : public CTier2AppSystem< IInterface, ConVarFlag >
{
public:
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER3;
	}
};


#endif // TIER3_H

