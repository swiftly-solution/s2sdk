#include "public/tier0/uniqueid.h"

bool operator ==( const UniqueId_t& lhs, const UniqueId_t& rhs )
{
	return !Q_memcmp( (void *)&lhs.m_Value[ 0 ], (void *)&rhs.m_Value[ 0 ], sizeof( lhs.m_Value ) );
}
