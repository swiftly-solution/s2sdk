#ifndef TIER4_H
#define TIER4_H

#if defined( _WIN32 )
#pragma once
#endif

#include "public/tier3/tier3.h"


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier4 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 >
class CTier4AppSystem : public CTier3AppSystem< IInterface, ConVarFlag >
{
public:
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER4;
	}
};


#endif // TIER4_H
