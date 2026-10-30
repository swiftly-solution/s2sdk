#include "public/game/spawngroupmgrgamesystem.h"

const char * CMapSpawnGroup::GetWorldName() const
{
	return m_pSpawnGroup->GetWorldName();
}

const char * CMapSpawnGroup::GetEntityLumpName() const
{
	return m_pSpawnGroup->GetEntityLumpName();
}

const char * CMapSpawnGroup::GetEntityFilterName() const
{
	return m_pSpawnGroup->GetEntityFilterName();
}

SpawnGroupHandle_t CMapSpawnGroup::GetSpawnGroupHandle() const
{
	return m_pSpawnGroup->GetHandle();
}

const matrix3x4a_t & CMapSpawnGroup::GetWorldOffset() const
{
	return m_pSpawnGroup->GetWorldOffset();
}

const char * CMapSpawnGroup::GetParentNameFixup() const
{
	return m_pSpawnGroup->GetParentNameFixup();
}

const char * CMapSpawnGroup::GetLocalNameFixup() const
{
	return m_pSpawnGroup->GetLocalNameFixup();
}

SpawnGroupHandle_t CMapSpawnGroup::GetOwnerSpawnGroup() const
{
	return m_pSpawnGroup->GetOwnerSpawnGroup();
}

ILoadingSpawnGroup * CMapSpawnGroup::GetLoadingSpawnGroup() const
{
	return m_pSpawnGroup->GetLoadingSpawnGroup();
}

void CMapSpawnGroup::SetLoadingSpawnGroup(ILoadingSpawnGroup *pLoading)
{
	m_pSpawnGroup->SetLoadingSpawnGroup(pLoading);
}

int CMapSpawnGroup::GetCreationTick() const
{
	return m_pSpawnGroup->GetCreationTick();
}

bool CMapSpawnGroup::DontSpawnEntities() const
{
	return m_pSpawnGroup->DontSpawnEntities();
}

void CMapSpawnGroup::GetSpawnGroupDesc(SpawnGroupDesc_t *pDesc) const
{
	m_pSpawnGroup->GetSpawnGroupDesc(pDesc);
}

ISpawnGroup * CMapSpawnGroup::GetSpawnGroup() const
{
	return m_pSpawnGroup;
}
