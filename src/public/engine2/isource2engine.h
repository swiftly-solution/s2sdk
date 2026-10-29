#ifndef ISOURCE2ENGINE_H
#define ISOURCE2ENGINE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "public/tier0/platform.h"
#include "public/resourcesystem/iresourcesystem.h"
#include "public/tier0/utlstring.h"
#include "public/const.h"
#include "public/worldrenderer/iworldrenderer.h"
#include "public/worldrenderer/worldgroupid.h"
#include "iscreenshotcallback.h"
#include "public/entity2/entityinterfaces.h"
#include "public/tier0/loggingtypes.h"

class CEntityLump;
class CCompressedResourceManifest;
class IWorldReference;
class ISpawnGroupPrerequisiteRegistry;
class ILoadingSpawnGroup;

class ISource2Engine : public IAppSystem
{
public:
	// Is the game paused?
	virtual bool		IsPaused() = 0;

	// What is the game timescale multiplied with the host_timescale?
	virtual float		GetTimescale( void ) const = 0;

	virtual void		*FindOrCreateWorldSession( const char *pszWorldName, const char *, void * ) = 0;

	virtual CEntityLump	*GetEntityLumpForTemplate( const char *, bool, const char *, const char *, bool ) = 0;

	virtual uint32_t	GetStatsAppID() const = 0;

	virtual void		WriteJpegScreenshot( const char *pszFilename, int nQuality, int nWidth, int nHeight, bool bAsyncWrite ) = 0;
	virtual void		RequestScreenshot( IScreenshotCallback *pCallback, void *pContext ) = 0;
};

#define MAX_SPAWN_GROUP_WORLD_NAME_LENGTH 4096

typedef int *SaveRestoreDataHandle_t;

enum SpawnGroupState_t
{
	SPAWN_GROUP_ALLOCATED = 0,
	SPAWN_GROUP_WORLD_LOADED = 1,
	SPAWN_GROUP_ENTITIES_ALLOCATED = 2,
	SPAWN_GROUP_GAMESYSTEMS_PRECACHE_START = 3,
	SPAWN_GROUP_GAMESYSTEMS_PRECACHE_END = 4,
	SPAWN_GROUP_ENTITY_PRECACHE_START = 5,
	SPAWN_GROUP_ENTITY_PRECACHE_END = 6,
	SPAWN_GROUP_READY_TO_SPAWN_ENTITIES = 7,
	SPAWN_GROUP_ENTITIES_SPAWNED = 8
};

struct SpawnGroupDesc_t
{
public:
	SpawnGroupDesc_t();

	CUtlString m_sWorldName; // map path eg. 'maps/prefabs/de_dust2/de_dust2_skybox' or 'de_mirage'
	CUtlString m_sWorldMountName;
	CUtlString m_sEntityLumpName;
	CUtlString m_sEntityFilterName;
	CUtlString m_sDescriptiveName;
	CUtlString m_sParentNameFixup;
	CUtlString m_sLocalNameFixup;
	IComputeWorldOriginCallback *m_pWorldOffsetCallback;
	matrix3x4a_t m_vecWorldOffset;
	CUtlString m_sWorldGroupName;
	CUtlString m_sSaveFileName;
	SpawnGroupHandle_t m_hOwner;
	int m_iPriorityLoader;
	ResourceManifestLoadPriority_t m_nManifestLoadPriority; // ResourceManifestLoadPriority_t
	float m_flTimeoutInterval;

	bool m_bCreateClientEntitiesOnLaterConnectingClients;
	bool m_bDontSpawnEntities;
	bool m_bBlockUntilLoaded;
	bool m_bLoadStreamingData;
	bool m_bCreateNewSceneWorld;
	bool m_bManualCompletion;
	bool m_bSetActivePostLoad;
	bool m_unk101; // AMNOTE: Spawn groups created by point_prefab clear it
	bool m_bLevelTransition;
	bool m_unk201;
	bool m_unk202;
};

struct SpawnGroupDescReceive_t
{
public:
	bool m_bIsInitialSpawnGroup;
	bool m_bCreateClientOnlyEntities;
	bool m_bIsSynchronousSpawn;

public:
	CCompressedResourceManifest *m_pCompressedResource;

public:
	int32 m_nTickCount;
	bool m_bManifestInComplete;

public:
	SpawnGroupHandle_t m_hSpawnGroupParentHandle;
	WorldGroupId_t m_hWorldGroupId;
};

enum CreateSpawnGroupType_t
{
	CREATE_SPAWN_GROUP_IMMEDIATELY = 0,
	CREATE_SPAWN_GROUP_ASYNCHRONOUSLY,
	CREATE_SPAWN_GROUP_ASYNCHRONOUSLY_CONFIRM_RESOURCES_LOADED,
};

class ISpawnGroup
{
public:
	virtual const char *GetWorldName() const = 0;
	virtual const char *GetEntityLumpName() const = 0;
	virtual const char *GetEntityFilterName() const = 0;
	virtual SpawnGroupHandle_t GetHandle() const = 0;
	virtual const matrix3x4a_t &GetWorldOffset() const = 0;
	virtual bool ShouldLoadEntitiesFromSave() const = 0;
	virtual const char *GetParentNameFixup() const = 0;
	virtual const char *GetLocalNameFixup() const = 0;
	virtual SpawnGroupHandle_t GetOwnerSpawnGroup() const = 0;
	virtual ILoadingSpawnGroup *GetLoadingSpawnGroup() const = 0;
	virtual void SetLoadingSpawnGroup(ILoadingSpawnGroup *pLoading) = 0;
	virtual IWorldReference *GetWorldReference() const = 0;
	virtual CUtlString Describe() const = 0;

public:
	virtual uint32 GetCreationTick() const = 0;
	virtual void RequestDeferredUnload(bool bAsync = true) = 0;
	virtual uint32 GetDestructionTick() const = 0;

public:
	virtual bool DontSpawnEntities() const = 0;
	virtual uint8 GetUnk2AndLastFlagBits() const = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup() const = 0;
	virtual uint32 GetChildSpawnGroupCount() const = 0;
	virtual void GetSpawnGroupDesc(SpawnGroupDesc_t *pDesc) const = 0;

public:
	virtual void FlagManualCreation() = 0;
	virtual bool HasManualCreation() = 0;
	virtual void LoadAllGameResourceManifests(void *) = 0;
	virtual void AddSpawnGroupChild(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual bool HasSetActivePostLoad() const = 0;
	virtual bool HasUnk2() const = 0;
	virtual bool HasUnk() const = 0;
	virtual void UnkSetter(uint32 n) = 0;
	virtual bool HasLevelTransition() const = 0;
	virtual WorldGroupId_t GetWorldGroupId() const = 0;
};

class ILoadingSpawnGroup
{
public:
	virtual int EntityCount() const = 0;
	virtual const EntitySpawnInfo_t *GetEntities() const = 0;
	virtual bool ShouldLoadEntitiesFromSave() const = 0;
	virtual const CUtlBuffer *GetSaveRestoreFileData() const = 0;
	virtual void SetLevelTransitionPreviousMap(const char *pLevelTransitionMap, const char *pLandmarkName) = 0;
	virtual bool IsLevelTransition() const = 0;
	virtual const char *GetLevelTransitionPreviousMap() const = 0;
	virtual const char *GetLevelTransitionLandmarkName() const = 0;
	virtual CKV3Arena *GetEntityKeyValuesAllocator() = 0;
	virtual CEntityInstance *CreateEntityToSpawn(SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset, CreateSpawnGroupType_t createType, const CEntityKeyValues *pEntityKeyValues, uint32) = 0;
	virtual void SpawnEntities() = 0;
	// AMNOTE: Destroys and clears the entities after passing them to the entity save/restore system
	virtual void RestoreEntities(uint32, const char *, int, void *, int *) = 0;
	// AMNOTE: Asks the entity save/restore system about the save file named pszSaveName
	virtual bool unk101(void *, const char *pszSaveName) = 0;
	virtual void Release() = 0;
};

class IGameSpawnGroupMgr
{
public:
	virtual void AllocateSpawnGroup(SpawnGroupHandle_t h, ISpawnGroup *pSpawnGroup) = 0;
	virtual void ReleaseSpawnGroup(SpawnGroupHandle_t h) = 0;
	virtual void SpawnGroupInit(SpawnGroupHandle_t handle, IEntityResourceManifest *pManifest, IEntityPrecacheConfiguration *pConfig, ISpawnGroupPrerequisiteRegistry *pRegistry) = 0;
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroup(SpawnGroupHandle_t handle, bool bSynchronouslySpawnEntities, bool bConfirmResourcesLoaded, const CUtlVector<const CEntityKeyValues *> *pKeyValues) = 0;
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroupForSaveFile(SpawnGroupHandle_t handle, const void *pSaveData, size_t nSaveDataSize) = 0;
	virtual void SpawnGroupSpawnEntities(SpawnGroupHandle_t handle) = 0;
	virtual void SpawnGroupShutdown(SpawnGroupHandle_t handle) = 0;
	virtual void SetActiveSpawnGroup(SpawnGroupHandle_t handle) = 0;
	virtual void UnloadSpawnGroup(SpawnGroupHandle_t hSpawnGroup, bool bSaveEntities) = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual int GetChildSpawnGroupCount(SpawnGroupHandle_t handle) = 0;
	virtual SaveRestoreDataHandle_t SaveGame_Start(const char *pSaveName, const char *pOldLevel, const char *pszLandmarkName) = 0;
	virtual bool SaveGame_Finalize(SaveRestoreDataHandle_t hSaveRestore) = 0;
	virtual bool StreamEntitiesToFile(SaveRestoreDataHandle_t hSaveRestore) = 0;
	virtual const char *SaveGame_GetLastSaveFile() = 0;
	virtual bool IsGameReadyToSave() = 0;
	virtual void DumpSpawnGroups() = 0;
	virtual LoggingChannelID_t GetSaveRestoreLogChannel() = 0;
	virtual CUtlString GetSaveRestoreContextString() const = 0;
	// AMNOTE: Dispatch the IGameSystem WorldGroupCreated and WorldGroupDestroyed events
	virtual void OnWorldGroupCreated(WorldGroupId_t hWorldGroup) = 0;
	virtual void OnWorldGroupDestroyed(WorldGroupId_t hWorldGroup) = 0;
	virtual CUtlString GetName() const = 0;
};

#endif