#ifndef PLAYERSLOT_H
#define PLAYERSLOT_H

#if _WIN32
#pragma once
#endif

#include "public/const.h"

class CPlayerSlot
{
public:
	CPlayerSlot( int slot );

	void Invalidate();
	bool IsValid() const;
	int Get() const;

	bool operator==( const CPlayerSlot &other ) const;
	bool operator!=( const CPlayerSlot &other ) const;

private:
	int m_Data;
};

class CPlayerUserId
{
public:
	CPlayerUserId( int index );

	int Get() const;

	bool operator==( const CPlayerUserId &other ) const;
	bool operator!=( const CPlayerUserId &other ) const;

private:
	unsigned short _index;
};

#endif // PLAYERSLOT_H