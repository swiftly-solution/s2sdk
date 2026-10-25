#ifndef ENTITYINDEX_H
#define ENTITYINDEX_H

#if _WIN32
#pragma once
#endif

class CEntityIndex
{
public:
	CEntityIndex( int index )
	{
		_index = index;
	}

	int Get() const
	{
		return _index;
	}

	bool operator==( const CEntityIndex &other ) const { return other._index == _index; }
	bool operator!=( const CEntityIndex &other ) const { return other._index != _index; }
	
private:
	int _index;
};

#endif