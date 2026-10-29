//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Ownership id for a shared object cache
//
//=============================================================================

#ifndef SOID_H
#define SOID_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"

namespace GCSDK
{

//----------------------------------------------------------------------------
// Shared type for object caches. This can hold SteamIDs, LobbyIDs, PartyIds,
// etc.  Make sure they don't conflict!
//----------------------------------------------------------------------------

const uint32_t k_SOID_Type_SteamID = 1;
const uint32_t k_SOID_Type_PartyGroupID = 2;
const uint32_t k_SOID_Type_LobbyGroupID = 3;
const uint32_t k_SOID_Type_PartyInvite = 4;
const uint32_t k_SOID_Type_CheatReport = 5;
const uint32_t k_SOID_Type_NqmmRating = 6;

//----------------------------------------------------------------------------
// Purpose: Owner of a shared object cache, CMsgSOIDOwner on the wire
//----------------------------------------------------------------------------
struct SOID_t
{
	bool operator==( const SOID_t &rhs ) const;
	bool operator!=( const SOID_t &rhs ) const;
	// The order the GC client's cache map is sorted in
	bool operator<( const SOID_t &rhs ) const;

	uint64 m_id;
	uint32 m_type;
};

} // namespace GCSDK

#endif // SOID_H
