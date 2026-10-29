#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include "public/tier0/platform.h"
#include "public/tier0/threadtools.h"
#include "public/tier0/generichash.h"
#include "public/tier0/keyvalues3.h"
#include "public/tier0/utlsymboltablelarge.h"
#include "public/tier0/utlvector.h"
#include "public/tier0/utldict.h"
#include "public/tier0/utlmap.h"
#include "public/tier0/utlhashtable.h"
#include "public/tier0/utldelegate.h"
#include "public/tier0/mempool.h"
#include "public/tier0/utlstring.h"
#include "public/networksystem/inetworkserializer.h"
#include "public/vscript/ivscript.h"
#include "public/resourcefile/resourcetype.h"
#include "entityhandle.h"
#include "concreteentitylist.h"
#include "entitydatainstantiator.h"
#include "ientity2_entityio.h"
#include "entitystructs.h"
#include "entityinterfaces.h"
#include "gametime.h"

class CKV3Arena;
class CPulseCallContextInfo;
class CEntityClass;
class CEntityComponentHelper;
class CEntityKeyValues;
class IEntityIONotify;
class INetworkFieldChangedEventQueue;
class INetworkFieldScratchData;
class IFieldChangeLimitSpew;
class IEntity2SaveRestore;
class IEntity2Networkables;
class IEntityResourceManifest;
class IEntityPrecacheConfiguration;
class IEntityResourceManifestBuilder;
class ISpawnGroupEntityFilter;
class IHandleEntity;
struct ComponentUnserializerFieldInfo_t;
struct EntOutput_t;

enum EntityIterType_t
{
	ENTITY_ITER_OVER_ACTIVE = 0x0,
	ENTITY_ITER_OVER_DORMANT = 0x1,
};

enum ClearEntityDatabaseMode_t
{
	CED_NORMAL = 0x0,
	CED_NETWORKEDONLY_AND_DONTCLEARSTRINGPOOL = 0x1,
};

// Event queue //

struct CPulseArgumentPack
{
	// The data of each argument's value
	CUtlVectorFixed<void*, 16> m_Values;

	// AMNOTE: Holds the argument names, types and values
	CPulseCallContextInfo* m_pInfo;
};

struct EntityIOQueuePrioritizedEvent_t
{
	WorldGroupId_t m_WorldGroupId;
	GameTime_t m_flFireTime;
	EntityIOTargetType_t m_targetType;
	CUtlSymbolLarge m_pTarget;
	CUtlSymbolLarge m_pTargetInput;
	CEntityHandle m_hActivator;
	CEntityHandle m_hCaller;
	CEntityHandle m_hEntTarget; // a pointer to the entity to target; overrides m_pTarget

	CVariant m_variantValue; // variable-type parameter

	CPulseArgumentPack m_PulseArguments;
	CPulseInputParamMap m_paramMap;

	EntityIOQueuePrioritizedEvent_t *m_pNext;
	EntityIOQueuePrioritizedEvent_t *m_pPrev;
};

class CEventQueue
{
public:
	CAtomicMutex m_Mutex;
	EntityIOQueuePrioritizedEvent_t m_Events;
};

// Entity notifications //

struct EntityDormancyChange_t : EntityNotification_t
{
	EntityDormancyType_t m_nPrevDormancyType;
	EntityDormancyType_t m_nNewDormancyType;
};

struct EntityActivation_t : EntityNotification_t
{
};

struct EntityDeletion_t : EntityNotification_t
{
};

struct PostDataUpdateInfo_t : EntityNotification_t
{
	DataUpdateType_t m_updateType;
};

// Called with each added resource once it's loaded, to add the resources it uses
typedef void (*SecondaryPrecacheCallback_t)(ResourceHandle_t hResource, const CEntityPrecacheContext* pContext);

abstract_class IEntityResourceManifest
{
public:
	virtual void AddResource(const char* pszResourceName) = 0;
	virtual void AddResource(const char* pszResourceName, SecondaryPrecacheCallback_t pfnCallback) = 0;
	virtual void AddResource(const char* pszResourceName, CEntityInstance* pEntity, SecondaryPrecacheMemberCallback_t callback) = 0;

	// Adds the compiled resource of a "file://{resources}/" panorama path
	virtual void AddPanoramaResource(const char* pszPath) = 0;

	virtual void AddPrecachedSubclass(const CUtlStringToken& subclassId) = 0;
	virtual bool IsSubclassPrecached(const CUtlStringToken& subclassId) = 0;

	// AMNOTE: Writes "<p2>/<p1>/<p3>/<pszFilter or __nofilter__>.ventl" to pOut
	virtual void ConstructEntityLumpResourceName(const char* p1, const char* p2, const char* p3, const char* pszFilter, CBufferString* pOut) = 0;

	// Logs each added resource when enabled
	virtual void SetVerbose(bool bVerbose) = 0;

	// AMNOTE: True when the game resource manifest that owns this one was allocated with RESOURCE_MANIFEST_LOAD_DEFAULT load behavior
	virtual bool unk101() = 0;

	virtual int GetResourceCount() = 0;
	virtual const char* const* GetResourceNames() = 0;

	// Sets the entity whose resources are being added, returns the previous one
	virtual EntitySpawnInfo_t SetCurrentPrecacheEntity(const EntitySpawnInfo_t& info) = 0;
};

struct EntityNameHandles_t
{
	CUtlVector<CEntityHandle> m_Handles;

	// Held by the entry in the name map and by iterators over the handles, freed when it drops to 0
	int m_nRefCount;
};

class CEntitySystem : public IEntityResourceManifestBuilder
{
private:
	enum
	{
		MAX_ACCESSORS = 32,
	};

	struct CreationInfo_t
	{
		CEntityHandle m_hEnt;
		CEntityInstance* m_pEnt;
		const CEntityKeyValues* m_pKeyValues;

		// AMNOTE: Seems to be checked along side m_pKeyValues before its usage
		bool m_bEntityKeyValuesInitialized;
	};

	struct DestructionInfo_t
	{
		CEntityIdentity* m_pEnt;
	};

	struct DormancyChangeInfo_t
	{
		CEntityHandle m_hEnt;
		bool m_bInPVS;
	};

	struct MurmurHash2HashFunctor
	{ 
		unsigned int operator()( uint32 n ) const; 
	};

	typedef CUtlDelegate<bool( IErrorListener *, CEntityInstance *, void *, const ComponentUnserializerFieldInfo_t *, const KeyValues3 * )> KeyUnserializerDelegate;

public:
	virtual						~CEntitySystem() = 0;

	// This function is called in CGameEntitySystem::ProcessEventQueue()
	virtual GameTime_t			GetCurTime( WorldGroupId_t ) = 0;

	virtual void				ClearEntityDatabase(ClearEntityDatabaseMode_t eMode) = 0;
	virtual CEntityInstance*	FindEntityProcedural(const char* szName, CEntityInstance* pSearchingEntity = nullptr, CEntityInstance* pActivator = nullptr, CEntityInstance* pCaller = nullptr) = 0;
	virtual void				OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent) = 0;
	virtual void				OnAddEntity(CEntityInstance* pEnt, CEntityHandle handle) = 0;
	virtual void				OnRemoveEntity(CEntityInstance* pEnt, CEntityHandle handle) = 0;
	virtual WorldGroupId_t		GetSpawnGroupWorldId(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual void				SortEntities(int nCount, EntitySpawnInfo_t* pInfo) = 0;
	virtual void				Spawn(int nCount, const EntitySpawnInfo_t* pInfo) = 0;
	virtual void				Activate(int nCount, const EntityActivation_t* pActivates, ActivateType_t activateType) = 0;
	virtual void				PostDataUpdate(int nCount, const PostDataUpdateInfo_t *pInfo) = 0;
	virtual void				OnSetDormant(int nCount, const EntityDormancyChange_t* pInfo, bool bNotifyAddedToPVS) = 0;
	virtual void				UpdateOnRemove(int nCount, const EntityDeletion_t *pDeletion) = 0;

public:
	CEntityIdentity *GetEntityIdentity( CEntityIndex entnum );
	CEntityIdentity *GetEntityIdentity( const CEntityHandle &hEnt );

	CEntityInstance *GetEntityInstance( CEntityIdentity *ident );
	CEntityInstance *GetEntityInstance( CEntityIndex entnum );
	CEntityInstance *GetEntityInstance( const CEntityHandle &hEnt );

	// AMNOTE: The controller of player slot N is the entity at index N + 1, only slots below CGlobalVars::maxClients have one
	CEntityInstance *GetPlayerController( CPlayerSlot slot );

	void AddEntityKeyValuesAllocatorRef();
	void ReleaseEntityKeyValuesAllocatorRef();

	CKV3Arena* GetEntityKeyValuesAllocator();

	// Search for an entity class by its C++ name, case-insensitive
	CEntityClass* FindClassByName(const char* szClassName);
	
	// Search for an entity class by its designer name, case-insensitive
	CEntityClass* FindClassByDesignName(const char* szClassName);

	CEntityHandle FindFirstEntityHandleByName(const char* szName, WorldGroupId_t hWorldGroupId = WorldGroupId_t());

	CUtlSymbolLarge AllocPooledString(const char* pString);
	CUtlSymbolLarge FindPooledString(const char* pString);

private:
	IEntityResourceManifest *m_pCurrentManifest;

public:
	CConcreteEntityList m_EntityList;
	CUtlString m_sEntSystemName;

	CUtlOrderedMap<const char*, CEntityClass*, CDefFastCaselessStringLess, uint16> m_entClassesByCPPClassname;
	CUtlOrderedMap<const char*, CEntityClass*, CDefFastCaselessStringLess, uint16> m_entClassesByClassname;
	CUtlOrderedMap<const char*, CEntityComponentHelper*, CDefFastCaselessStringLess, uint16> m_entityComponentHelpers;
	CUtlOrderedMap<CUtlSymbolLarge, EntityNameHandles_t*, CDefLess<CUtlSymbolLarge>, uint16> m_entityNames;

	CEventQueue m_EventQueue;
	CUtlVectorFixedGrowable<IEntityIONotify*, 2> m_entityIONotifiers;
	int m_nSuppressDormancyChangeCount;
	NetworkSerializationMode_t m_eNetworkSerializationMode;
	int m_nExecuteQueuedCreationDepth;
	int m_nExecuteQueuedDeletionDepth;
	int m_nSuppressDestroyImmediateCount;
	int m_nSuppressAutoDeletionExecutionCount;
	int m_nEntityKeyValuesAllocatorRefCount;
	float m_flChangeCallbackSpewThreshold;

	bool m_bPerformDeletionPostCreation;
	bool m_bPerformDeallocationPostCreation;
	bool m_bInvokePostDataUpdates;
	bool m_bAutoExecuteQueuedDeletion;
	bool m_bProfilingResourceSystem;
	bool m_bInRestoreActivation;

	// AMNOTE: Write-only: set with m_bInRestoreActivation while restored entities are activated and restored afterwards, true when they come from an adjacent level
	bool m_unk101;

	bool m_bProcessingQueuedActivatesAndPostDataUpdates;

	CUtlVector<CreationInfo_t> m_queuedCreations;
	CUtlVector<PostDataUpdateInfo_t> m_queuedPostDataUpdates;
	CUtlVector<DestructionInfo_t> m_queuedDeletions;
	CUtlVector<DestructionInfo_t> m_queuedDeferredDeletions;
	CUtlVector<DestructionInfo_t> m_queuedDeallocations;
	CUtlVector<DormancyChangeInfo_t> m_queuedDormancyChanges;

	CUtlDelegate<void ( int, const EntitySpawnInfo_t* )> m_EntityPostSpawnCallback;
	INetworkFieldChangedEventQueue* m_pNetworkFieldChangedEventQueue;
	INetworkFieldScratchData* m_pNetworkFieldScratchData;
	IFieldChangeLimitSpew* m_pFieldChangeLimitSpew;
	CUtlHashtable<SpawnKeyType_t, KeyUnserializerDelegate, MurmurHash2HashFunctor> m_DataDescKeyUnserializers;
	CUtlScratchMemoryPool m_ComponentUnserializerInfoAllocator;
	CKV3Arena m_EntityKeyValuesAllocator;
	CUtlSymbolTableLargeMT_CI m_Symbols;
	SpawnGroupHandle_t m_hActiveSpawnGroup;
	matrix3x4a_t m_vSpawnOriginOffset;
	IEntityDataInstantiator* m_Accessors[ MAX_ACCESSORS ];
	CUtlHashtable<CUtlString, void*> m_EntityMaterialAttributes;
};

abstract_class IEntityIONotify
{
public:
	virtual void OnEntityIOOutput( CEntityInstance* pActivator, CEntityInstance* pCaller, const EntityIOConnection_t* pConnection, const CPulseArgumentPack* pArgs ) = 0;
	// Called after the target handled the input
	virtual void OnEntityIOInput( CEntityInstance* pTarget, const char* pszInput, CEntityInstance* pActivator, CEntityInstance* pCaller, const CVariant* pValue, const CPulseArgumentPack* pArgs ) = 0;
	virtual void OnEntityIOConnectionRemoved( CEntityInstance* pActivator, CEntityInstance* pCaller, const EntityIOConnection_t* pConnection ) = 0;
	virtual void OnOutputFired( CEntityInstance* pActivator, CEntityInstance* pCaller, float flDelay, const EntOutput_t* pOutput, const CPulseArgumentPack* pArgs ) = 0;
};

abstract_class IEntityFindFilter
{
public:
	virtual bool ShouldFindEntity(CEntityInstance* pEnt) = 0;
};

class EntityInstanceIter_t
{
public:
	EntityInstanceIter_t(IEntityFindFilter* pFilter = nullptr, EntityIterType_t eIterType = ENTITY_ITER_OVER_ACTIVE);

	CEntityInstance* First();
	CEntityInstance* Next();

	void SetWorldGroupId(WorldGroupId_t hWorldGroupId);

private:
	CEntityIdentity*	m_pCurrentEnt;
	IEntityFindFilter*	m_pFilter;
	EntityIterType_t	m_eIterType;
	WorldGroupId_t		m_hWorldGroupId;
};

class EntityInstanceByNameIter_t
{
public:
	EntityInstanceByNameIter_t(const char* szName, CEntityInstance* pSearchingEntity = nullptr, CEntityInstance* pActivator = nullptr, CEntityInstance* pCaller = nullptr, IEntityFindFilter* pFilter = nullptr, EntityIterType_t eIterType = ENTITY_ITER_OVER_ACTIVE);

	CEntityInstance* First();
	CEntityInstance* Next();

	void SetWorldGroupId(WorldGroupId_t hWorldGroupId);

private:
	CEntityIdentity*			m_pCurrentEnt;
	IEntityFindFilter*			m_pFilter;
	EntityIterType_t			m_eIterType;
	WorldGroupId_t				m_hWorldGroupId;
	const char*					m_pszEntityName;
	CUtlVector<CEntityHandle>*	m_pEntityHandles;
	int							m_nCurEntHandle;
	int							m_nNumEntHandles;
	CEntityInstance*			m_pProceduralEnt;
};

class EntityInstanceByClassIter_t
{
public:
	EntityInstanceByClassIter_t(const char* szClassName, IEntityFindFilter* pFilter = nullptr, EntityIterType_t eIterType = ENTITY_ITER_OVER_ACTIVE);
	EntityInstanceByClassIter_t(CEntityInstance* pStart, const char* szClassName, IEntityFindFilter* pFilter = nullptr, EntityIterType_t eIterType = ENTITY_ITER_OVER_ACTIVE);

	CEntityInstance* First();
	CEntityInstance* Next();

	void SetWorldGroupId(WorldGroupId_t hWorldGroupId);

private:
	CEntityIdentity*	m_pCurrentEnt;
	IEntityFindFilter*	m_pFilter;
	EntityIterType_t	m_eIterType;
	WorldGroupId_t		m_hWorldGroupId;
	const char*			m_pszClassName;
	CEntityClass*		m_pEntityClass;
};

#endif // ENTITYSYSTEM_H