#include "public/tier0/splitscreenslot.h"

CSplitScreenSlot::CSplitScreenSlot()
:
	m_Data(0)
{}

CSplitScreenSlot::CSplitScreenSlot( int index )
{
	m_Data = index;
}

int CSplitScreenSlot::Get() const
{
	return m_Data;
}

CSplitScreenSlot::operator int() const
{
	return m_Data;
}
