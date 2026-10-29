#ifndef ENTITYIDENTITY_H
#define ENTITYIDENTITY_H

#if _WIN32
#pragma once
#endif

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64 // 0x3F
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS // 0x8000

#include "entityhandle.h"
#include "entityindex.h"
#include "entityclass.h"
#include "public/tier0/utlsymbollarge.h"
#include "public/worldrenderer/worldgroupid.h"
#include "public/networksystem/networksystemtypes.h"

class CEntityAttributeTable;
class CEntityClass;

enum EntityFlags_t : uint32_t
{
	EF_IS_INVALID_EHANDLE = 0x1,
	EF_SPAWN_IN_PROGRESS = 0x2,
	EF_IN_STAGING_LIST = 0x4,
	EF_IN_POST_DATA_UPDATE = 0x8,
	EF_DELETE_IN_PROGRESS = 0x10,
	EF_IN_STASIS = 0x20,
	EF_IS_ISOLATED_ALLOCATION_NETWORKABLE = 0x40,
	EF_IS_DORMANT = 0x80,
	EF_IS_PRE_SPAWN = 0x100,
	EF_MARKED_FOR_DELETE = 0x200,
	EF_IS_CONSTRUCTION_IN_PROGRESS = 0x400,
	EF_IS_ISOLATED_ALLOCATION = 0x800,
	EF_HAS_BEEN_UNSERIALIZED = 0x1000,
	EF_IS_SUSPENDED = 0x2000,
	EF_IS_ANONYMOUS_ALLOCATION = 0x4000,
	EF_SUSPEND_OUTSIDE_PVS = 0x8000,
};

class CEntityIdentity
{
public:
	CEntityHandle GetRefEHandle() const;
	
	const char *GetClassname() const;

	CEntityIndex GetEntityIndex() const;

	SpawnGroupHandle_t GetSpawnGroup() const;

public:
	CEntityInstance* m_pInstance;
	CEntityClass* m_pClass;
	CEntityHandle m_EHandle;
	int32_t m_nameStringTableIndex;
	CUtlSymbolLarge m_name;
	CUtlSymbolLarge m_designerName;
	uint64_t m_hPublicScope; // CEntityPublicScriptScope
	EntityFlags_t m_flags;
	SpawnGroupHandle_t m_hSpawnGroup;
	WorldGroupId_t m_worldGroupId;
	uint32_t m_fDataObjectTypes;
	ChangeAccessorFieldPathIndex_t m_PathIndex;
	CEntityAttributeTable* m_pAttributes;
	CEntityIdentity* m_pPrev;
	CEntityIdentity* m_pNext;
	CEntityIdentity* m_pPrevByClass;
	CEntityIdentity* m_pNextByClass;
};

#endif