#ifndef _WORLDRENDERER_H
#define _WORLDRENDERER_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform_base.h"

// AMNOTE: Index into the engine's world group table, with the high bit set for client-side groups; -1 is invalid
class WorldGroupId_t
{
public:
	WorldGroupId_t( uint32_t nId = 0 ) : m_nId( nId ) {}

	bool operator==( const WorldGroupId_t &other ) const { return m_nId == other.m_nId; }
	bool operator!=( const WorldGroupId_t &other ) const { return m_nId != other.m_nId; }
	bool operator<( const WorldGroupId_t &other ) const { return m_nId < other.m_nId; }

    uint32_t GetId() { return m_nId; }
    void SetId(uint32_t id) { m_nId = id; }

private:
	uint32_t m_nId;
};

#define INVALID_WORLD_GROUP_ID WorldGroupId_t( 0xFFFFFFFF )

#endif