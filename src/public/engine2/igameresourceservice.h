#ifndef IGAMERESOURCESERVICE_H
#define IGAMERESOURCESERVICE_H

#ifdef _WIN32
#pragma once
#endif

#include "iengineservicemgr.h"
#include "public/entity2/entitysystem.h"
#include "public/resourcefile/resourcetype.h"
#include "public/resourcesystem/iresourcesystem.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlvector.h"

class CCompressedResourceManifest;
class CGameResourceManifestLock;

abstract_class IGameResourceManifestLoadCompletionCallback
{
public:
	// AMNOTE: Game callbacks delete themselves here, spawn groups do nothing
	virtual void Release() = 0;
	virtual void OnGameResourceManifestLoaded( HGameResourceManifest hManifest, int nResourceCount, ResourceHandle_t * pResourceHandles ) = 0;
};

abstract_class IGameResourceService : public IEngineService
{
public:
	virtual ~IGameResourceService();

	virtual HGameResourceManifest LoadGameResourceManifestNamed( const char *pszResourceName, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HGameResourceManifest LoadGameResourceManifestGroup( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HGameResourceManifest LoadGameResourceManifest( int nResourceCount, const char *const *ppszResourceNames, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HGameResourceManifest LoadGameResourceManifest( EntityResourceManifestCreationCallback_t pfnCreationCallback, void *pContext, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual void SetManifestCompletionCallback( HGameResourceManifest hManifest, IGameResourceManifestLoadCompletionCallback *pCallback, int nResourceCount, const char *const *ppszResourceNames, bool ) = 0;
	virtual bool IsManifestLoaded( HGameResourceManifest hManifest ) = 0;
	virtual void BlockUntilManifestLoaded( HGameResourceManifest hManifest ) = 0;
	virtual void DestroyResourceManifest( HGameResourceManifest hManifest ) = 0;
	virtual const char *GetResourceManifestDebugName( HGameResourceManifest hManifest ) = 0;
	virtual bool DoesManifestHaveFutureDependentResources( HGameResourceManifest hManifest ) = 0;
	virtual void SetEntityResourceManifestHandler( IEntityResourceManifestBuilder *pBuilder ) = 0;
	virtual void PrecacheEntitiesAndConfirmResourcesAreLoaded( SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t *pEntities, const matrix3x4a_t *pWorldOffset ) = 0;
	virtual void DescribeContents( LoggingChannelID_t nChannel, HGameResourceManifest hManifest ) = 0;
	virtual void GetManifestResourceNames( HGameResourceManifest hManifest, CUtlVector<CUtlString> &resourceNames ) = 0;
	virtual HGameResourceManifest AllocGameResourceManifest( ResourceManifestLoadBehavior_t eBehavior, const char *pszAllocatorName, ResourceManifestLoadPriority_t ePriority ) = 0;

	// Overloads LoadGameResourceManifest: GCC keeps this position, MSVC groups it with the overloads above
	virtual bool LoadGameResourceManifest( HGameResourceManifest hManifest ) = 0;

	virtual HGameResourceManifest CreateGameResourceManifest( const CCompressedResourceManifest *pCompressedManifest, bool, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual void AppendToGameResourceManifest( HGameResourceManifest hManifest, const CCompressedResourceManifest *pCompressedManifest, bool, ResourceManifestLoadBehavior_t eBehavior, bool ) = 0;
	virtual void BuildCompressedManifest( HGameResourceManifest hManifest, CCompressedResourceManifest *pCompressedManifest, bool bOnlyUnloadedResources ) = 0;
	virtual void LockGameResourceManifest( bool bLock, CGameResourceManifestLock &manifestLock ) = 0;
	virtual bool AppendToAndCreateGameResourceManifest( HGameResourceManifest hManifest, SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t *pEntities, const matrix3x4a_t *pWorldOffset ) = 0;
};

class CGameResourceService : public CBaseEngineService<IGameResourceService>
{
public:
	// AMNOTE: The engine never reads or writes it
	bool m_unk001;
	bool m_bIsServer;
	CUtlVector<void *> m_unk101;
	IEntityResourceManifestBuilder *m_pEntityResourceManifestHandler;
};

#endif // IGAMERESOURCESERVICE_H
