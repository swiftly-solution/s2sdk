#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H

#if _WIN32
#pragma once
#endif

#include "componentunserializer.h"
#include "entitycomponent.h"
#include "public/vscript/ivscript.h"
#include "public/networksystem/iflattenedserializers.h"
#include "public/engine2/serverclass.h"

class CEntityClass;
class CPulseAPIExtensionRegistrationContext;
class EntInput_t;
class CEntitySharedPulseSignature;
class CEntityComponentHelper;
class EntOutput_t;

enum EntityClassFlags_t
{
	ECF_NOT_NETWORKED						= (1 << 0), // If the EntityClass is non-networkable
	ECF_ALIAS								= (1 << 1), // If the EntityClass is an alias
	ECF_SPAWN_GROUP_HANDLE_INVALID			= (1 << 2), // Don't use spawngroups when creating entity
	ECF_HAS_REQUIRED_ENTITY_HANDLE			= (1 << 3), // Forces m_requiredEHandle on created entities
	ECF_ALWAYS_SPAWN_ON_CLIENT				= (1 << 4),
	ECF_BECOME_SUSPENDED_INSTEAD_OF_DORMANT = (1 << 5), // Suspend entities outside of PVS
	ECF_ANONYMOUS_ENTITY					= (1 << 6), // If the EntityClass is anonymous
	ECF_PRECACHE_NETWORKED_ENTITY_ON_CLIENT	= (1 << 7),
	ECF_HAS_ALTERNATE_BASELINES				= (1 << 8),
	ECF_ALTERNATE_BASELINES_USE_RECIPIENTS_FILTERS = (1 << 9),
	ECF_FORCE_WORLDGROUPID					= (1 << 10) // Forces worldgroupid to be 1 on created entities
};

typedef enum
{
	USE_OFF = 0,
	USE_ON = 1,
	USE_SET = 2,
	USE_TOGGLE = 3
} USE_TYPE;

struct EntityUseInput_t
{
	CEntityInstance *m_pActivator;
	CEntityInstance *m_pCaller;
	USE_TYPE m_nType;
	float m_flValue;
};

// Keeps the member function pointers below as small as the engine's
class SINGLE_INHERITANCE CEntityInstance;

typedef void (CEntityInstance::*BASEPTR)();
typedef void (CEntityInstance::*ENTITYFUNCPTR)(CEntityInstance *pOther);
typedef void (CEntityInstance::*USEPTR)(const EntityUseInput_t *pInput);

struct EntClassComponentOverride_t
{
	const char* pszBaseComponent;
	const char* pszOverrideComponent;
};

enum EntityClassInfoFlags_t
{
	ECIF_NOT_SPAWNABLE = (1 << 0), // CreateEntityByName refuses to spawn the entity if set
};

struct EntComponentNameEntry_t
{
	const char* pszComponentClassName;
	size_t nOffsetInEntity;
};

class CEntityClassInfo
{
public:
	const char* m_pszClassname;
	const char* m_pszCPPClassname;
	uint32_t m_nFlags; // EntityClassInfoFlags_t

	CEntityClass *m_pClass;
	CEntityClassInfo *m_pBaseClassInfo;
	CSchemaClassInfo* m_pSchemaBinding;
	datamap_t* m_pDataDescMap;
};

class CEntityClass
{
	struct ComponentOffsets_t
	{
		uint16_t m_nOffset;
	};

	struct ComponentHelper_t
	{
		size_t m_nOffset;
		CEntityComponentHelper* m_pComponentHelper;
	};
	
	struct ClassInputInfo_t
	{
		CUtlSymbolLarge m_sName;
		EntInput_t* m_pInput;
	};
	
	struct ClassOutputInfo_t
	{
		CUtlSymbolLarge m_sName;
		EntOutput_t* m_pOutput;
	};
	
public:
	CSchemaClassInfo *GetSchemaBinding() const;

	datamap_t *GetDataDescMap() const;
	
public:
	using FuncToNameCb = const char *(*)(BASEPTR think_fn);
	using NameToFuncCb = BASEPTR (*)(const char *fn_name);

	// Registers this class' pulse bindings, called base classes first
	using RegisterPulseBindingsCb = void (*)(CPulseAPIExtensionRegistrationContext *pContext);
	// Appends the components this class owns, overrides are applied by the caller
	using EnumerateComponentsCb = void (*)(CUtlVector<EntComponentNameEntry_t> *pOut);

	ScriptClassDesc_t* m_pScriptDesc;
	CNetworkSerializerClassInfo *m_NetworkSerializerInfo;

	EntInput_t* m_pInputs;
	EntOutput_t* m_pOutputs;
	int m_nInputCount;
	int m_nOutputCount;

	CEntitySharedPulseSignature *m_pSharedPulseSignature;

	RegisterPulseBindingsCb m_pfnRegisterPulseBindings;
	// Convert this class' think and touch functions to and from their names,
	// searching the parent classes as well
	NameToFuncCb m_NameToThinkFunc;
	FuncToNameCb m_ThinkFuncToName;
	EnumerateComponentsCb m_pfnEnumerateComponents;

	EntClassComponentOverride_t* m_pComponentOverrides;
	
	CEntityClassInfo* m_pClassInfo;
	CEntityClassInfo* m_pBaseClassInfo;
	CUtlSymbolLarge m_designerName;

	// Uses EntityClassFlags_t flags
	uint m_flags;

	int m_SpawnOrder;
	
	uint m_nAllHelpersFlags;

	CUtlVector<ComponentOffsets_t> m_ComponentOffsets;
	CUtlVector<ComponentHelper_t> m_AllHelpers;
	
	ComponentUnserializerClassInfo_t m_componentUnserializerClassInfo;
	
	FlattenedSerializerDesc_t m_flattenedSerializer;

	CUtlVector<ClassOutputInfo_t> m_classOutputInfos;
	
	CEntityHandle m_requiredEHandle;

	CEntityClass* m_pNext;
	CEntityIdentity* m_pFirstEntity;
	ServerClass* m_pServerClass;

	// Assigned from the global class counter on registration, used as a bit index by CEntitySystem
	int m_nClassIndex;
};

#endif