#include "public/entity2/entityindex.h"

CEntityIndex::CEntityIndex( int index )
{
	_index = index;
}

int CEntityIndex::Get() const
{
	return _index;
}

bool CEntityIndex::operator==( const CEntityIndex &other ) const
{ return other._index == _index; }

bool CEntityIndex::operator!=( const CEntityIndex &other ) const
{ return other._index != _index; }
