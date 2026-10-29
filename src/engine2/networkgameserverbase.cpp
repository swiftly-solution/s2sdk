#include "public/engine2/networkgameserverbase.h"

WorldGroupId_t CNetworkGameServerBase::WorldGroupList_t:: GetWorldGroupId( int nIndex ) const
{ return WorldGroupId_t( (uint32)nIndex | ( m_bClient ? 0x80000000 : 0 ) ); }

bool CNetworkGameServerBase::WorldGroupList_t:: IsWorldGroupValid( WorldGroupId_t hWorldGroup ) const
{
	const WorldGroup_t *pWorldGroup = GetWorldGroup( hWorldGroup );
	return pWorldGroup && pWorldGroup->m_nRefCount > 0;
}

const CNetworkGameServerBase::WorldGroup_t * CNetworkGameServerBase::WorldGroupList_t::GetWorldGroup( WorldGroupId_t hWorldGroup ) const
{
	if ( ( ( hWorldGroup.GetId() & 0x80000000 ) != 0 ) != m_bClient )
		return nullptr;

	int nIndex = (int)( hWorldGroup.GetId() & 0x7FFFFFFF );
	if ( nIndex >= m_Groups.Count() )
		return nullptr;

	return &m_Groups[nIndex];
}

WorldGroupId_t CNetworkGameServerBase::WorldGroupList_t::FindWorldGroupByName( const char *pszName ) const
{
	if ( pszName && *pszName )
	{
		CUtlStringToken token( pszName );
		FOR_EACH_VEC( m_Groups, i )
		{
			if ( m_Groups[i].m_Token == token )
				return GetWorldGroupId( i );
		}
	}

	return INVALID_WORLD_GROUP_ID;
}
