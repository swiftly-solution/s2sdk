//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ENTITYDATAINSTANTIATOR_H
#define ENTITYDATAINSTANTIATOR_H
#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/utlhashtable.h"

#include "public/tier0/memdbgon.h"

class CEntityInstance;

//-----------------------------------------------------------------------------
// Purpose: Data objects kept per entity outside of it, created on demand
//-----------------------------------------------------------------------------
abstract_class IEntityDataInstantiator
{
public:
	virtual ~IEntityDataInstantiator();

	virtual void *GetDataObject( const CEntityInstance *pInstance ) = 0;
	virtual void *CreateDataObject( const CEntityInstance *pInstance ) = 0;
	virtual void DestroyDataObject( const CEntityInstance *pInstance ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: An IEntityDataInstantiator of T objects. Destroying it doesn't
// destroy the objects it still holds
//-----------------------------------------------------------------------------
template <class T>
class CEntityDataInstantiator : public IEntityDataInstantiator
{
public:
	virtual void *GetDataObject( const CEntityInstance *pInstance )
	{
		UtlHashHandle_t handle = m_HashTable.Find( pInstance );
		return m_HashTable.IsValidHandle( handle ) ? m_HashTable[ handle ] : NULL;
	}

	virtual void *CreateDataObject( const CEntityInstance *pInstance )
	{
		UtlHashHandle_t handle = m_HashTable.Find( pInstance );
		if ( !m_HashTable.IsValidHandle( handle ) )
			handle = m_HashTable.Insert( pInstance, new T );

		return m_HashTable[ handle ];
	}

	virtual void DestroyDataObject( const CEntityInstance *pInstance )
	{
		UtlHashHandle_t handle = m_HashTable.Find( pInstance );
		if ( m_HashTable.IsValidHandle( handle ) )
		{
			delete m_HashTable[ handle ];
			m_HashTable.Remove( pInstance );
		}
	}

private:
	CUtlHashtable< const CEntityInstance *, T * > m_HashTable;
};

#include "public/tier0/memdbgoff.h"

#endif // ENTITYDATAINSTANTIATOR_H
