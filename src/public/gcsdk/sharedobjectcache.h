//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for objects that are kept in synch between client and server
//
//=============================================================================

#ifndef SHAREDOBJECTCACHE_H
#define SHAREDOBJECTCACHE_H

#ifdef _WIN32
#pragma once
#endif

#include "sharedobject.h"

namespace GCSDK
{

//----------------------------------------------------------------------------
// Purpose: The part of a shared object cache that handles all objects of a
//			single type.
//----------------------------------------------------------------------------
class CSharedObjectTypeCache
{
public:
	virtual ~CSharedObjectTypeCache();

	int GetTypeID() const;
	uint32 GetCount() const;
	CSharedObject *GetObject( uint32 nObj );
	const CSharedObject *GetObject( uint32 nObj ) const;

	virtual bool AddObject( CSharedObject *pObject );
	virtual bool AddObjectClean( CSharedObject *pObject );
	virtual CSharedObject *RemoveObject( const CSharedObject &soIndex );
	virtual void RemoveAllObjectsWithoutDeleting();
	virtual void EnsureCapacity( uint32 nItems );
	virtual void Dump() const;

private:
	CSharedObjectVec m_vecObjects;
	// AMNOTE: Hash map of the key lookup tables CSharedObject::FindKeyIndex searches
	uint8 m_KeyIndices[80];
	int m_nTypeID;
};

//----------------------------------------------------------------------------
// Purpose: A cache of a bunch of shared objects of different types. This class
//			is shared between clients, gameservers, and the GC and is
//			responsible for sending messages from the GC to cause object
//			creation/destruction/updating on the clients/gameservers.
//----------------------------------------------------------------------------
abstract_class CSharedObjectCache
{
public:
	virtual ~CSharedObjectCache();

	virtual SOID_t GetOwner() const = 0;

	virtual bool AddObject( CSharedObject *pSharedObject );
	virtual bool AddObjectClean( CSharedObject *pSharedObject );
	virtual CSharedObject *RemoveObject( const CSharedObject &soIndex );
	virtual bool RemoveAllObjectsWithoutDeleting();

	//called to find the type cache for the specified class ID. This will return NULL if one does not exist
	CSharedObjectTypeCache *FindBaseTypeCache( int nClassID ) const;

	void SetVersion( uint64 ulVersion );
	uint64 GetVersion() const;
	virtual void MarkDirty();

	virtual void Dump() const;

protected:
	virtual CSharedObjectTypeCache *AllocateTypeCache( int nClassID ) const = 0;
	CSharedObjectTypeCache *GetTypeCacheByIndex( int nIndex );
	int GetTypeCacheCount() const;

	uint64 m_ulVersion;

private:
	// Sorted by type ID
	CUtlVector<CSharedObjectTypeCache *> m_vecTypeCaches;
};

} // namespace GCSDK

#endif // SHAREDOBJECTCACHE_H
