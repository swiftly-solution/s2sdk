#ifndef ENTITYINDEX_H
#define ENTITYINDEX_H

#if _WIN32
#pragma once
#endif

class CEntityIndex
{
public:
	CEntityIndex( int index );

	int Get() const;

	bool operator==( const CEntityIndex &other ) const;
	bool operator!=( const CEntityIndex &other ) const;
	
private:
	int _index;
};

#endif