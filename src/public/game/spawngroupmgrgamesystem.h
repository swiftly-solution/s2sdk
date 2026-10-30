#ifndef SPAWNGROUPMGRGAMESYSTEM_H
#define SPAWNGROUPMGRGAMESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "public/engine2/isource2engine.h"

class IPVS;

class CMapSpawnGroup
{
public:
	const char *GetWorldName() const;

	const char *GetEntityLumpName() const;

	const char *GetEntityFilterName() const;

	SpawnGroupHandle_t GetSpawnGroupHandle() const;

	const matrix3x4a_t &GetWorldOffset() const;

	const char *GetParentNameFixup() const;

	const char *GetLocalNameFixup() const;

	SpawnGroupHandle_t GetOwnerSpawnGroup() const;

	ILoadingSpawnGroup *GetLoadingSpawnGroup() const;

	void SetLoadingSpawnGroup(ILoadingSpawnGroup *pLoading);

	int GetCreationTick() const;

	bool DontSpawnEntities() const;

	void GetSpawnGroupDesc(SpawnGroupDesc_t *pDesc) const;

public:
	ISpawnGroup *GetSpawnGroup() const;

private:
	ISpawnGroup *m_pSpawnGroup;
	bool m_bSpawnGroupPrecacheDispatched;
	bool m_bSpawnGroupLoadDispatched;
	IPVS *m_pPVS;
	// CUtlVector<SpawnGroupConnectionInfo_t> m_Connections;
};

// AMNOTE: Short stubs representing game class hierarchy
class CLoadingSpawnGroup : public ILoadingSpawnGroup
{
};

class CSpawnGroupMgrGameSystem : public IGameSpawnGroupMgr, public IGameSystem
{
};

#endif // SPAWNGROUPMGRGAMESYSTEM_H
