#ifndef NETWORKVAR_H
#define NETWORKVAR_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/utldelegate.h"
#include "public/tier0/utlvector.h"
#include "public/entity2/entityinstance.h"
#include "networksystemtypes.h"

class CNetworkVarChainer : public CEntityOwnerPtr
{
public:
	CNetworkVarChainer();

	struct ChainUpdatePropagation_t
	{
		CUtlDelegate<void ( const CNetworkVarChainer & )> updateDelegate;
		// The networked member updateDelegate is bound to: its name, and its offset in the chainer's owner
		const char* m_pszFieldName;
		uint32 m_nFieldOffset;
	};

	void RegisterChainUpdatePropagationTarget( ChainUpdatePropagation_t& target );

	// Points each target at its member of pOwner, then calls it.
	// AMNOTE: bAssignChangeAccessorPathIds has the entity fill in m_PathIndex
	void Set( CEntityInstance* pInstance, bool bAssignChangeAccessorPathIds, void* pOwner );

	// These pass the call on to the entity, unless there is none or it is being constructed.
	// NetworkStateChanged fills in data's path index when it has none
	void NetworkStateChanged( NetworkStateChangedData& data );
	void NetworkStateChangedRemove( const NetworkStateChangedRemove_t& data );
	void NetworkStateChangedLog( const char* pszFieldName, const char* pszInfo );

public:
	CUtlVector<ChainUpdatePropagation_t> m_PropagationList;
	ChangeAccessorFieldPathIndex_t m_PathIndex;
	// AMNOTE: Checked by the owner, not by the methods above. A struct with its own chainer passes its state changes to it
	// only while this is set, which a networked vector does for its elements
	bool m_bForwardStateChanges;
};





#endif // NETWORKVAR_H
