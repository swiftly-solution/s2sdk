#include "public/worldrenderer/worldgroupid.h"

WorldGroupId_t::WorldGroupId_t( uint32_t nId )
: m_nId( nId )
{}

bool WorldGroupId_t::operator==( const WorldGroupId_t &other ) const
{ return m_nId == other.m_nId; }

bool WorldGroupId_t::operator!=( const WorldGroupId_t &other ) const
{ return m_nId != other.m_nId; }

bool WorldGroupId_t::operator<( const WorldGroupId_t &other ) const
{ return m_nId < other.m_nId; }

uint32_t WorldGroupId_t::GetId() const
{ return m_nId; }

void WorldGroupId_t::SetId(uint32_t id)
{ m_nId = id; }
