#include "public/engine2/isource2engine.h"

SpawnGroupDesc_t::SpawnGroupDesc_t()
:
	m_pWorldOffsetCallback(nullptr),
	m_sWorldGroupName("default"),
	m_hOwner(0),
	m_iPriorityLoader(-2),
	m_nManifestLoadPriority(RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT),
	m_flTimeoutInterval(0.0f),
	m_bCreateClientEntitiesOnLaterConnectingClients(false),
	m_bDontSpawnEntities(false),
	m_bBlockUntilLoaded(false),
	m_bLoadStreamingData(true),
	m_bCreateNewSceneWorld(false),
	m_bManualCompletion(false),
	m_bSetActivePostLoad(false),
	m_unk101(true),
	m_bLevelTransition(false),
	m_unk201(false),
	m_unk202(false)
{
	SetIdentityMatrix(m_vecWorldOffset);
}
