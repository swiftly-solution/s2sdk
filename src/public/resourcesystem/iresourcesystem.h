#ifndef IRESOURCESYSTEM_H
#define IRESOURCESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/tier0/utlstring.h"
#include "public/appframework/iappsystem.h"
#include "public/resourcefile/resourcetype.h"
#include "public/tier0/utlvector.h"
#include "resourcesystemtypes.h"
#include "public/filesystem/iasyncfilesystem.h"
#include "public/filesystem/filesystem.h"

class IResourceSystemUtils;
class ResourceFileHeader_t;
class IRD_RegisterResourceDataUtils;
class IResourceDeallocatorUtils;
class IResourceUpdater;
class IToolsResourceListener;
class ICodeResourceManifestManager;
class IResourceSystemProfiler;
class IResourceSystemLeakTracker;
class IVDataTypeManager;
class IToolsResourcePreReloadListener;

enum ResourceManifestLoadBehavior_t : int8_t
{
	RESOURCE_MANIFEST_LOAD_DEFAULT = -1,
	RESOURCE_MANIFEST_LOAD_STREAMING_DATA = 0,
	RESOURCE_MANIFEST_INITIALLY_USE_FALLBACKS,
};

enum ResourceManifestLoadPriority_t : int8_t
{
	RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT = -1,
	RESOURCE_MANIFEST_LOAD_PRIORITY_LOW = 0,
	RESOURCE_MANIFEST_LOAD_PRIORITY_MEDIUM,
	RESOURCE_MANIFEST_LOAD_PRIORITY_HIGH,
	RESOURCE_MANIFEST_LOAD_PRIORITY_IMMEDIATE,

	RESOURCE_MANIFEST_LOAD_PRIORITY_COUNT,
};

enum ResourceLoadType_t
{
	RESOURCE_LOAD_INVALID = -1,
	RESOURCE_LOAD_SUCCESS = 0,
	RESOURCE_LOAD_FAIL,
	RESOURCE_LOAD_RELOAD,
	RESOURCE_LOAD_PRE_RELOAD,
};

typedef void* HResourceManifest;

enum ResourceStreamingType_t : uint8_t
{
	RESOURCE_STREAMING_TYPE_NO_DATA = 0,
	RESOURCE_STREAMING_TYPE_HAS_DATA,
	RESOURCE_STREAMING_TYPE_HAS_DATA_ALWAYS_LOAD,
	RESOURCE_STREAMING_TYPE_STREAM_DATA_FROM_ALLOCATE,
};

class IAsyncProcessedDataResult
{
public:
	virtual ~IAsyncProcessedDataResult();
};

class IResourceTypeManager
{
public:
	virtual bool Init( IResourceSystemUtils *pUtils ) = 0;
	virtual void Shutdown() = 0;
	// AMNOTE: Called after Shutdown when the type manager is removed, once the type's pending loads have finished
	virtual void unk001() = 0;
	virtual ResourceHandle_t GetErrorResource() = 0;
	virtual bool NeedsFrameUpdate() const = 0;
	virtual void FrameUpdate( int ) = 0;
	virtual bool RequiresFinalizeResourceCall() const = 0;
	virtual void FinalizeResource( ResourceHandle_t hResource, ResourceLoadType_t eLoadType ) = 0;
	// AMNOTE: Cached when the type manager is installed, true makes the resource system call unk102 on a worker thread before FinalizeResource
	virtual bool unk101() const = 0;
	virtual void unk102( ResourceHandle_t hResource, ResourceLoadType_t eLoadType ) = 0;
	virtual ResourceStreamingType_t GetStreamingType() const = 0;
	virtual int GetNonStreamingDataLoadSize( ResourceHandle_t hResource, bool ) const = 0;
	virtual void LoadStreamingData( ResourceHandle_t hResource, IResourceSystemUtils *pUtils ) = 0;
	virtual void AllocateResource( ResourceHandle_t hResource, ResourceId_t id, const ResourceFileHeader_t *pHeader, IRD_RegisterResourceDataUtils *pUtils ) = 0;
	virtual void DeallocateResource( void *pData, IResourceDeallocatorUtils *pUtils ) = 0;
	virtual void NotifyResourceWithWorkPendingLoadCancelled( ResourceHandle_t hResource, void *pData ) = 0;
	virtual bool UsesData() const = 0;
	virtual bool IsStreamingDataAlreadyLoaded( ResourceHandle_t hResource ) = 0;
	virtual bool WantsAsyncProcessData() const = 0;
	virtual std::unique_ptr<IAsyncProcessedDataResult> AsyncProcessData( ResourceHandle_t hResource, const ResourceFileHeader_t *pHeader ) = 0;
	// AMNOTE: Cached when the type manager is installed, true makes the resource system call unk202 during Update, only for the vdata type
	virtual bool unk201() const = 0;
	virtual void unk202() = 0;
	// AMNOTE: Called on every type manager before and after Update processes the resources that finished loading
	virtual void unk203() = 0;
	virtual void unk204() = 0;
};

enum ResourceManifestPriority_t
{
	RESOURCE_TYPE_MANIFEST_PRIORITY_LOAD_FIRST = -1,
	RESOURCE_TYPE_MANIFEST_PRIORITY_DEFAULT = 0,
	RESOURCE_TYPE_MANIFEST_PRIORITY_LOAD_LAST = 1,
};

enum ResourceSystemDebugMode_t
{
	RESOURCE_SYSTEM_DEBUG_MODE_DEFER_MANIFEST_FINALIZATION = 0,
	RESOURCE_SYSTEM_DEBUG_MODE_SKIP_EXTREF_DEPTH_ASSERTIONS,
	RESOURCE_SYSTEM_DEBUG_MODE_IGNORE_DUPLICATE_TYPE_MANAGERS,

	RESOURCE_SYSTEM_DEBUG_MODE_COUNT,
};

enum ResourceManifestType_t : int8_t
{
	RESOURCE_MANIFEST_TYPE_DEFAULT = 0,
	RESOURCE_MANIFEST_TYPE_BLOCKING_LOAD,
	RESOURCE_MANIFEST_TYPE_BLOCKING_RELOAD,
};

typedef void (*ResourceManifestLoadCompletionCallback_t)( HResourceManifest hManifest, void *pContext );

struct ResourceManifestCreationInfo_t
{
	ResourceManifestCreationInfo_t();

	ResourceManifestCreationInfo_t( int nCount, const char *const *ppResourceFiles, ResourceManifestType_t nType, ResourceManifestLoadBehavior_t nLoadBehavior,
		const char *pDebugName, ResourceManifestLoadPriority_t nPriority );

	ResourceManifestCreationInfo_t( int nCount, const char *const *ppResourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior = RESOURCE_MANIFEST_LOAD_DEFAULT,
		const char *pDebugName = nullptr, ResourceManifestLoadPriority_t nPriority = RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT );

	// A single resource; pszResourceFile is referenced, so it has to outlive the use of the info
	ResourceManifestCreationInfo_t( const char *const &pszResourceFile, ResourceManifestLoadBehavior_t nLoadBehavior = RESOURCE_MANIFEST_LOAD_DEFAULT,
		const char *pDebugName = nullptr, ResourceManifestLoadPriority_t nPriority = RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT );

	ResourceManifestCreationInfo_t( std::initializer_list<const char *> resourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior = RESOURCE_MANIFEST_LOAD_DEFAULT,
		const char *pDebugName = nullptr, ResourceManifestLoadPriority_t nPriority = RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT );

	int m_nCount;
	const char *const *m_ppResourceFiles;
	ResourceManifestType_t m_nType;
	ResourceManifestLoadBehavior_t m_nLoadBehavior;
	const char *m_pDebugName;
	ResourceManifestLoadPriority_t m_nPriority;
	ResourceManifestLoadCompletionCallback_t m_pfnCallback;
	void *m_pContext;
};

typedef void (*ResourceSystemForcedSynchronizationCallback_t)( void *pContext );
typedef void (*ResourceSystemQueuedCallbackFn_t)( void *pContext );

struct ResourceSystemQueuedCallback_t
{
	void *m_pContext;
	ResourceSystemQueuedCallbackFn_t m_pfnCallback;
};

enum ResourceEvent_t
{
	// Its file data was read, nDataSize is its size in bytes
	RESOURCE_EVENT_DATA_LOADED = 0,
	// Its streaming data was requested
	RESOURCE_EVENT_STREAMING_DATA_REQUESTED,
	// Finished loading, also sent for a resource with no file
	RESOURCE_EVENT_LOADED,
	// Just before it is deallocated
	RESOURCE_EVENT_DEALLOCATING,
	// The manifest starts loading its resources
	RESOURCE_EVENT_MANIFEST_LOAD_STARTED,
	// The manifest has loaded, after its completion callback ran
	RESOURCE_EVENT_MANIFEST_LOADED,
	// The manifest's load priority was raised, as when something blocks until it has loaded
	RESOURCE_EVENT_MANIFEST_PRIORITY_RAISED,
};

class IResourceEventListener
{
public:
	// nDataSize is 0 for every event but RESOURCE_EVENT_DATA_LOADED
	virtual void OnResourceEvent( const char *pszName, ResourceEvent_t nEvent, int nDataSize ) = 0;
};

enum ProceduralResourceType_t
{
	PROCEDURAL_RESOURCE_ANONYMOUS = 0,
	PROCEDURAL_RESOURCE_NAMED,
};

enum ResourceSystemUpdateMode_t
{
	RESOURCE_SYSTEM_UPDATE_RETURN_QUICKLY = 0,
	RESOURCE_SYSTEM_UPDATE_USE_ALL_TIME,
	RESOURCE_SYSTEM_UPDATE_USE_ALL_TIME_EXACT,
	RESOURCE_SYSTEM_UPDATE_SYNCHRONOUSLY_LOADING,
};

enum ResourceSystemGetResourcesFlags_t
{
	RESOURCE_SYSTEM_INCLUDE_PENDING_RESOURCES = 0x1,
	RESOURCE_SYSTEM_INCLUDE_NAMED_RESOURCES = 0x2,
	RESOURCE_SYSTEM_INCLUDE_ANONYMOUS_RESOURCES = 0x4,
	RESOURCE_SYSTEM_INCLUDE_NAMED_AND_ANONYMOUS_RESOURCES = RESOURCE_SYSTEM_INCLUDE_NAMED_RESOURCES | RESOURCE_SYSTEM_INCLUDE_ANONYMOUS_RESOURCES,
	RESOURCE_SYSTEM_INCLUDE_ALL_RESOURCES = RESOURCE_SYSTEM_INCLUDE_PENDING_RESOURCES | RESOURCE_SYSTEM_INCLUDE_NAMED_AND_ANONYMOUS_RESOURCES,
};

class IResourceSystem : public IAppSystem
{
public:
	virtual void InstallTypeManager( ResourceType_t nType, IResourceTypeManager *pTypeManager, const char *pszTypeName, const char *pszManifestName ) = 0;
	virtual void InstallNullTypeManager( ResourceType_t nType, const char *pszTypeName ) = 0;
	virtual void Update( int nTimeBudgetNs, ResourceSystemUpdateMode_t eMode ) = 0;
	virtual void UpdateSimple() = 0;
	virtual bool HasPendingWork() = 0;
	virtual void RemoveNullTypeManager( ResourceType_t nType ) = 0;
	virtual void RemoveResourceTypeManager( IResourceTypeManager *pTypeManager ) = 0;
	virtual IResourceTypeManager *GetTypeManager( ResourceType_t nType ) = 0;

	virtual HResourceManifest LoadResourceManifestFile( const char *pszFileName, ResourceManifestLoadBehavior_t eBehavior, const char *pszDebugName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HResourceManifest LoadResourceManifestGroup( const char *pszGroupName, ResourceManifestLoadBehavior_t eBehavior, const char *pszDebugName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HResourceManifest LoadResourceManifestNamed( const char *pszManifestName, ResourceManifestLoadBehavior_t eBehavior, const char *pszDebugName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HResourceManifest CreateResourceManifest( const ResourceManifestCreationInfo_t &info ) = 0;
	HResourceManifest CreateResourceManifest( int nCount, const char *const *ppResourceFiles, ResourceManifestType_t nType, ResourceManifestLoadBehavior_t nLoadBehavior, const char *pDebugName, ResourceManifestLoadPriority_t nPriority );
	HResourceManifest CreateResourceManifest( int nCount, const char *const *ppResourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior, const char *pDebugName, ResourceManifestLoadPriority_t nPriority );
	HResourceManifest CreateResourceManifest( std::initializer_list<const char *> resourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior = RESOURCE_MANIFEST_LOAD_DEFAULT,
		const char *pDebugName = nullptr, ResourceManifestLoadPriority_t nPriority = RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT );

	virtual void SetManifestCompletionCallback( HResourceManifest hManifest, ResourceManifestLoadCompletionCallback_t pfnCallback, void *pContext ) = 0;
	virtual bool IsManifestLoaded( HResourceManifest hManifest ) = 0;
	virtual void DestroyResourceManifest( HResourceManifest hManifest ) = 0;
	virtual const char *GetResourceManifestDebugName( HResourceManifest hManifest ) = 0;
	virtual void ForceSynchronizationAndBlockUntilManifestLoaded( HResourceManifest hManifest ) = 0;

	virtual ResourceHandle_t FindOrCreateProceduralResource( const CResourceName &name, void *pData, ProceduralResourceType_t eType ) = 0;
	virtual ResourceHandle_t FindResourceById( ResourceId_t id, ResourceType_t nType ) = 0;

	virtual void GetAllNamedResourcesByType( ResourceType_t nType, CUtlVector<ResourceHandle_t> &resources, ResourceSystemGetResourcesFlags_t nFlags ) = 0;
	virtual void GetAllResources( CUtlVector<ResourceHandle_t> &resources, ResourceSystemGetResourcesFlags_t nFlags ) = 0;
	virtual void GetAllResourcesByType( ResourceType_t nType, CUtlVector<ResourceHandle_t> &resources, ResourceSystemGetResourcesFlags_t nFlags ) = 0;

	virtual ResourceId_t ResourceHandleToResourceId( ResourceHandle_t hResource ) const = 0;

	// bAppend keeps what pName holds instead of clearing it first
	virtual void GetResourceName( ResourceHandle_t hResource, CBufferString *pName, bool bAppend ) = 0;
	virtual void GetResourceName( ResourceHandle_t hResource, CResourceName *pName ) = 0;

	virtual ResourceType_t GetResourceType( ResourceHandle_t hResource ) = 0;
	virtual ResourceHandle_t GetErrorResource( ResourceType_t nType ) = 0;
	virtual const char *GetResourceTypeName( ResourceType_t nType ) = 0;

	virtual void MarkErrorResourcesReloaded() = 0;

	virtual ResourceHandle_t BlockingLoadResourceByName( const CResourceName &name, const char *pszDebugName ) = 0;
	virtual ResourceHandle_t BlockingLoadResourceByNameIntoJustInTimeManifest( const CResourceName &name, const char *pszDebugName ) = 0;

	virtual void FreeJustInTimeManifests() = 0;
	virtual int GetJustInTimeManifestCount() = 0;
	virtual void GetJustInTimeManifestResources( CUtlVector<ResourceHandle_t> &resources ) = 0;

	virtual bool IsInFrameUpdate() = 0;

	virtual void InstallResourceUpdater( IResourceUpdater *pUpdater ) = 0;
	virtual void UninstallResourceUpdater( IResourceUpdater *pUpdater ) = 0;

	virtual void GetActualFileName( ResourceHandle_t hResource, CBufferString *pFileName, bool bAppend ) = 0;

	virtual ResourceStatus_t GetResourceStatus( const CResourceName &name ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceId_t id ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceHandle_t hResource ) = 0;

	virtual void RegisterForcedSynchronizationCallback( ResourceSystemForcedSynchronizationCallback_t pfnCallback, void *pContext ) = 0;
	virtual void UnregisterForcedSynchronizationCallback( ResourceSystemForcedSynchronizationCallback_t pfnCallback, void *pContext ) = 0;

	virtual void GetResourcesNamesInManifest( HResourceManifest hManifest, CUtlVector<CUtlString> &resourceNames ) const = 0;
	// Appends the handles of the manifest's resources; a manifest made of a single .vrman file lists that file's resources instead
	virtual void GetResourcesInManifest( HResourceManifest hManifest, CUtlVector<ResourceHandle_t> &resources ) = 0;

	// AMNOTE: Ignored outside tools mode
	virtual void RegisterToolsResourceListener( IToolsResourceListener *pListener ) = 0;
	virtual void UnregisterToolsResourceListener( IToolsResourceListener *pListener ) = 0;

	virtual void RegisterResourceEventListener( IResourceEventListener *pListener ) = 0;
	virtual void UnregisterResourceEventListener( IResourceEventListener *pListener ) = 0;

	virtual ICodeResourceManifestManager *GetCodeResourceManifestManager() = 0;
	virtual IResourceSystemProfiler *GetProfiler() = 0;
	virtual IResourceSystemLeakTracker *GetLeakTracker() = 0;
	virtual void SetResourceTypeManifestPriority( ResourceType_t nType, ResourceManifestPriority_t ePriority ) = 0;
	virtual void BlockingFinishAllCurrentlyLoadingManifests() = 0;
	virtual bool HasCompletedIORequests() = 0;
	virtual void EnableDebugMode( ResourceSystemDebugMode_t eMode, bool bEnable ) = 0;
	virtual AsyncRequestPriority_t GetAsyncFileRequestPriority( ResourceManifestLoadPriority_t ePriority ) const = 0;
	virtual IVDataTypeManager *GetGenericDataTypeManager() = 0;

	virtual bool IsShuttingDown() = 0;
	virtual bool IsDebugModeEnabled( ResourceSystemDebugMode_t eMode ) = 0;

	// AMNOTE: Ignored outside tools mode
	virtual void RegisterToolsResourcePreReloadListener( IToolsResourcePreReloadListener *pListener ) = 0;
	virtual void UnregisterToolsResourcePreReloadListener( IToolsResourcePreReloadListener *pListener ) = 0;

	virtual void InstallTestFilesystem( IFileSystem *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( IFileSystem *pTestFS ) = 0;

	// A recursive mutex for callers, the resource system doesn't take it itself
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual bool IsLockedByCurrentThread() = 0;

	virtual void QueueFrameUpdateCallback( const ResourceSystemQueuedCallback_t &callback ) = 0;

	// bWarnIfNotLoaded logs resources that are missing from every manifest, still pending or not loaded
	virtual ResourceHandle_t FindOrRegisterResourceByName( const CResourceName &name, bool bWarnIfNotLoaded ) = 0;
};

#endif