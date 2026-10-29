#ifndef SPLITSCREENSLOT_H
#define SPLITSCREENSLOT_H

#if _WIN32
#pragma once
#endif

struct CSplitScreenSlot
{
	CSplitScreenSlot();
	
	CSplitScreenSlot( int index );
	
	int Get() const;

	operator int() const;
	
	int m_Data;
};

#endif