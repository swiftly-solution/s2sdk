#include "public/networksystem/networkvar.h"

CNetworkVarChainer::CNetworkVarChainer()
: m_bForwardStateChanges( false )
{}

void CNetworkVarChainer::RegisterChainUpdatePropagationTarget( ChainUpdatePropagation_t& target )
{ m_PropagationList.AddToTail( target ); }

void CNetworkVarChainer::Set( CEntityInstance* pInstance, bool bAssignChangeAccessorPathIds, void* pOwner )
{
	CEntityOwnerPtr::Set( pInstance );

	for ( ChainUpdatePropagation_t& target : m_PropagationList )
	{
		CUtlAbstractDelegate delegate = target.updateDelegate.GetAbstractDelegate();
		delegate.UnsafeThisPointerSlam( (uint8*)pOwner + target.m_nFieldOffset );
		target.updateDelegate.SetAbstractDelegate( delegate );
		target.updateDelegate( *this );
	}

	if ( pInstance && bAssignChangeAccessorPathIds )
		pInstance->AssignChangeAccessorPathIds();
}

void CNetworkVarChainer::NetworkStateChanged( NetworkStateChangedData& data )
{
	CEntityInstance* pEntity = *this;
	if ( !pEntity || ( pEntity->m_pEntity->m_flags & EF_IS_CONSTRUCTION_IN_PROGRESS ) != 0 )
		return;

	ChangeAccessorFieldPathIndex_t nPathIndex = data.m_nPathIndex.m_Value >= 0 ? data.m_nPathIndex : m_PathIndex;
	if ( data.m_bRequiresPathIndex && nPathIndex.m_Value < 0 )
		return;

	data.m_nPathIndex = nPathIndex;
	pEntity->NetworkStateChanged( data );
}

void CNetworkVarChainer::NetworkStateChangedRemove( const NetworkStateChangedRemove_t& data )
{
	CEntityInstance* pEntity = *this;
	if ( pEntity && ( pEntity->m_pEntity->m_flags & EF_IS_CONSTRUCTION_IN_PROGRESS ) == 0 )
		pEntity->NetworkStateChangedRemove( data );
}

void CNetworkVarChainer::NetworkStateChangedLog( const char* pszFieldName, const char* pszInfo )
{
	CEntityInstance* pEntity = *this;
	if ( pEntity && ( pEntity->m_pEntity->m_flags & EF_IS_CONSTRUCTION_IN_PROGRESS ) == 0 )
		pEntity->NetworkStateChangedLog( pszFieldName, pszInfo );
}
