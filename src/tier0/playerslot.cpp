#include "public/tier0/playerslot.h"

CPlayerSlot::CPlayerSlot( int slot )
: m_Data( slot )
{}

void CPlayerSlot::Invalidate()
{ m_Data = -1; }

bool CPlayerSlot::IsValid() const
{ return m_Data >= 0 && m_Data < ABSOLUTE_PLAYER_LIMIT; }

int CPlayerSlot::Get() const
{ return m_Data; }

bool CPlayerSlot::operator==( const CPlayerSlot &other ) const
{ return other.m_Data == m_Data; }

bool CPlayerSlot::operator!=( const CPlayerSlot &other ) const
{ return other.m_Data != m_Data; }

CPlayerUserId::CPlayerUserId( int index )
{
	_index = index;
}

int CPlayerUserId::Get() const
{
	return _index;
}

bool CPlayerUserId::operator==( const CPlayerUserId &other ) const
{ return other._index == _index; }

bool CPlayerUserId::operator!=( const CPlayerUserId &other ) const
{ return other._index != _index; }
