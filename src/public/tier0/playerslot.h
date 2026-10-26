#ifndef PLAYERSLOT_H
#define PLAYERSLOT_H

#if _WIN32
#pragma once
#endif

class CPlayerSlot
{
public:
	CPlayerSlot( int slot )
	{
		_slot = slot;
	}

	int Get() const
	{
		return _slot;
	}

	bool operator==( const CPlayerSlot &other ) const { return other._slot == _slot; }
	bool operator!=( const CPlayerSlot &other ) const { return other._slot != _slot; }
	
private:
	int _slot;
};

#endif