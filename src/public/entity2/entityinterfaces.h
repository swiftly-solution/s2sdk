#ifndef ENTITYINTERFACES_H
#define ENTITYINTERFACES_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/resourcefile/resourcetype.h"
#include "entitystructs.h"
#include "public/mathlib/mathlib.h"
#include "public/const.h"
#include "public/tier0/utlvector.h"

class IEntityResourceManifest;
class CEntityIdentity;
class IEntityResourceManifestBuilder;
class CKV3Arena;

typedef void (*EntityResourceManifestCreationCallback_t)(IEntityResourceManifest *, void *);

struct EntityNotification_t
{
	CEntityIdentity* m_pEntity;
};

struct EntitySpawnInfo_t : EntityNotification_t
{
	const CEntityKeyValues* m_pKeyValues;
	// Index of the entity in the save file it is restored from, -1 for entities that don't come from a save
	int m_nSaveFileEntityIndex;

	// CEntitySystem::SortEntities orders by m_Depth, then m_ClassSpawnOrder from high to low, then m_CreationOrder

	// The order the entities were queued for creation in, 0xFFFF in a loading spawn group
	uint16_t m_CreationOrder;
	// Set by SortEntities: 1, or the depth of the entity its parentname names plus 1
	uint8_t m_Depth;
	// The low byte of its CEntityClass::m_SpawnOrder
	uint8_t m_ClassSpawnOrder;
};

class SINGLE_INHERITANCE CEntityInstance;

struct SecondaryPrecacheMemberCallback_t
{
	void (CEntityInstance::*pfnPrecache)(ResourceHandle_t hResource, const CEntityPrecacheContext* pContext);
};

struct CEntityResourceManifestLock
{
	IEntityResourceManifestBuilder* m_pBuilder;
	matrix3x4a_t m_vSpawnGroupOffset;
	SpawnGroupHandle_t m_hSpawnGroup;
	IEntityPrecacheConfiguration* m_pConfig;
	IEntityResourceManifest* m_pManifest;
	bool m_bIsLocked;
	bool m_bPrecacheEnable;
};

class IEntityResourceManifestBuilder
{
public:
	virtual void		BuildResourceManifest(SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t *pEntities, const matrix3x4a_t *vWorldOffset, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest) = 0;
	virtual void		BuildResourceManifest(SpawnGroupHandle_t hSpawnGroup, int iCount, const CEntityKeyValues **ppKeyValues, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest) = 0;
	virtual void		BuildResourceManifest(SpawnGroupHandle_t hSpawnGroup, const CUtlVector<const CEntityKeyValues*>* pEntityKeyValues, const char* pFilterName /* Usually, "mapload" and "cs_respawn"/"respawn". See CSpawnGroupEntityFilterRegistrar ctors */, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest) = 0;
	virtual void		BuildResourceManifest(const char* pManifestNameOrGroupName, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest) = 0;
	virtual void		BuildResourceManifest(EntityResourceManifestCreationCallback_t callback, void* pContext, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest) = 0;
	virtual void		BuildResourceManifestForEntity(const char* pEntityDesignerName, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest, CKV3Arena* pEntityAllocator) = 0;
	virtual void		InvokePrecacheCallback(ResourceHandle_t hResource, const EntitySpawnInfo_t* info, IEntityPrecacheConfiguration* pConfig, IEntityResourceManifest* pResourceManifest, SecondaryPrecacheMemberCallback_t callback) = 0;
	virtual void		AddRefKeyValues(const CEntityKeyValues* pKeyValues) = 0;
	virtual void		ReleaseKeyValues(const CEntityKeyValues* pKeyValues) = 0;
	virtual void		LockResourceManifest(bool bLock, CEntityResourceManifestLock* const context) = 0;
};

#endif