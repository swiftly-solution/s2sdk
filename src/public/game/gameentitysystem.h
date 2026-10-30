#ifndef GAMEENTITYSYSTEM_H
#define GAMEENTITYSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "public/entity2/entitysystem.h"

class CGameEntitySystem;

// Null before ISource2Server::Init and after its Shutdown
extern CGameEntitySystem* GameEntitySystem();

enum SpawnGroupEntityFilterType_t
{
	SPAWN_GROUP_ENTITY_FILTER_FALLBACK = 0x0,
	SPAWN_GROUP_ENTITY_FILTER_MOD_SPECIFIC = 0x1,
};

class IEntityListener
{
public:
	virtual void OnEntityCreated(CEntityInstance* pEntity);
	virtual void OnEntitySpawned(CEntityInstance* pEntity);
	virtual void OnEntityDeleted(CEntityInstance* pEntity);
	virtual void OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent);
};

class CGameEntitySystem : public CEntitySystem
{
	struct SpawnGroupEntityFilterInfo_t
	{
		ISpawnGroupEntityFilter* m_pFilter;
		SpawnGroupEntityFilterType_t m_nType;
	};

public:
	virtual ~CGameEntitySystem() = 0;

	void AddListenerEntity(IEntityListener* pListener);
	void RemoveListenerEntity(IEntityListener* pListener);

public:
	int m_iNetworkedEntCount;
	int m_iNonNetworkedSavedEntCount;
	CUtlDict<SpawnGroupEntityFilterInfo_t> m_spawnGroupEntityFilters;
	CUtlVector<IEntityListener*> m_entityListeners;
	IEntity2SaveRestore* m_pEntity2SaveRestore;
	IEntity2Networkables* m_pEntity2Networkables;
};

#endif // GAMEENTITYSYSTEM_H
