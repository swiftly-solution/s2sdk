#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "entity2/entitycomponent.h"
#include "entityhandle.h"
#include "networksystem/iflattenedserializers.h"
#include "public/vscript/ivscript.h"

#include <memory>


#define FENTCLASS_NON_NETWORKABLE		(1 << 0) // If the EntityClass is non-networkable
#define FENTCLASS_ALIAS					(1 << 1) // If the EntityClass is an alias
#define FENTCLASS_NO_SPAWNGROUP			(1 << 2) // Don't use spawngroups when creating entity
#define FENTCLASS_FORCE_EHANDLE			(1 << 3) // Forces m_requiredEHandle on created entities
#define FENTCLASS_UNK004				(1 << 4)
#define FENTCLASS_SUSPEND_OUTSIDE_PVS	(1 << 5) // Suspend entities outside of PVS
#define FENTCLASS_ANONYMOUS				(1 << 6) // If the EntityClass is anonymous
#define FENTCLASS_UNK007				(1 << 7)
#define FENTCLASS_UNK008				(1 << 8)
#define FENTCLASS_UNK009				(1 << 9)
#define FENTCLASS_FORCE_WORLDGROUPID	(1 << 10) // Forces worldgroupid to be 1 on created entities

class CSchemaClassInfo;
class CEntityClass;
class CEntityIdentity;
class CEntitySharedPulseSignature;
class CPulseAPIExtensionRegistrationContext;
class ServerClass;
struct EntInput_t;
struct EntOutput_t;
struct datamap_t;
class CChoreoComponent;
class CNetworkSerializerClassInfo;
class CNetworkSerializerCodeGenDatabase;

typedef void(*THINKFUNC)(CEntityInstance* pEntity);

struct NetworkRecipientsFilter_t
{
	using FilterCb = void (*)(CEntityInstance *ent, CCheckTransmitInfo *pInfo, CPlayerBitVec &player_mask);

	void *m_unk001;
	FilterCb m_FilterFn;
	CUtlString m_FilterName;
	int8 m_unk101;
};

struct NetworkChangePointerCallback_t
{
	using ChangeCb = void (*)(CChoreoComponent *component, CEntityInstance *ent, bool);

	CUtlString m_CallbackName;
	CUtlString m_ClassName;
	void *m_unk001;
	ChangeCb m_CallbackFn;
	int8 m_unk101;
};

struct NetworkOverride_t
{
	const char *m_ParentClass;
	const char *m_FieldName;
	const char *m_FieldPriority;
	int m_unk001;
};

struct VarTypeOverride_t
{
	CUtlString m_FieldName;
	CUtlString m_OverrideType;
};

struct SerializedFieldTypeMapping_t
{
	CUtlString m_FieldName;
	CUtlString m_FieldType;
};

struct UserGroupProxy_t
{
	const char *m_ClassName;
	const char *m_UserGroup;
	void *m_ProxyFn1;
	void *m_ProxyFn2;

	int8 m_unk001;
};

struct ReplayCompatField_t
{
	CUtlString m_FieldPath;
	CUtlString m_FieldType;
	CUtlString m_unk001;
	int8 m_unk002;
};

// alliedmodders/hl2sdk/tree/cs2
class CNetworkSerializerFieldInfo
{
public:
	CUtlStringToken m_FieldNameHash;
	CUtlString m_pszFieldName;
	CUtlString m_pszTypeName;
	CUtlString m_pszRawType;
	CUtlString m_pszEncodedType;
	CUtlStringToken m_ClassNameHash;
	CUtlString m_pszClassName;
	int32 m_nFieldSize;
	int32 m_nFieldOffset;
	CUtlStringToken m_NetworkAliasHash;
	CUtlString m_NetworkAlias;
	CUtlStringToken m_NetworkTypeAliasHash;
	CUtlString m_NetworkTypeAlias;

	void *m_unk001;

	CUtlString m_NetworkSerializer;
	CUtlString m_NetworkEncoder;

	std::shared_ptr<NetworkRecipientsFilter_t> m_NetworkSendProxyRecipientsFilter;
	std::shared_ptr<NetworkChangePointerCallback_t> m_NetworkChangePointerCallback;

	int8 m_NetworkPriority;

	SchemaCollectionManipulatorFn_t m_CollectionManipulatorFn;
	CUtlVector<CUtlString> m_NetworkIncludeByUserGroup;
	CUtlVector<CUtlString> m_NetworkChangeCb;

	int m_unk101;
	void *m_unk102;
	void *m_unk103;

	int m_NetworkBitCount;
	int m_NetworkEncodeFlags;
	int m_NetworkVarEmbeddedFieldOffsetDelta;
	float m_NetworkMin;
	float m_NetworkMax;

	int m_unk201;
	int8 m_unk202;
	int8 m_unk203;

	bool m_NetworkPolymorphic;
	CUtlString m_pszCodeGenType;
	CNetworkSerializerClassInfo *m_TypeClassInfo;

	int m_CodeGenValueTypeSize;

	int m_unk401;

	CUtlString m_TypeOverride;
	CUtlString m_BuiltinUnderlyingType;

	char m_ResourceTypeForInfoType[8];
	int m_FixedArraySize;
	char m_IsAtomic;
	char m_IsBuiltIn;
	char m_IsEnum;
	char m_IsCHandle;
	char m_IsPointer;
	char m_IsUtlVector;
	char m_IsFixedArray;
	char m_IsTypeSafeInt;
	char m_IsTypeSafeFloat;
	char m_IsStrongHandle;
	char m_IsWeakHandle;
	char m_IsModifierHandle;
	char m_IsSigned;
	char m_IsNetArray;
};

struct SerializerFieldLookup_t
{
	SerializerFieldLookup_t( const char *fieldname ) : m_FieldName( fieldname ), m_FieldIndex( 0 ) {}

	CUtlString m_FieldName;
	int m_FieldIndex;
};

template<> struct DefaultEqualFunctor<SerializerFieldLookup_t> { bool operator()( SerializerFieldLookup_t a, SerializerFieldLookup_t b ) const { return a.m_FieldName == b.m_FieldName; } };
template<> struct DefaultHashFunctor<SerializerFieldLookup_t> { unsigned int operator()( SerializerFieldLookup_t a ) const { return HashStringCaseless( a.m_FieldName.String() ); } };

class CNetworkSerializerClassInfo
{
public:
	CNetworkSerializerFieldInfo *FindField( const char *field_name ) const
	{
		auto handle = m_FieldLookupTable.Find( field_name );

		if(handle != m_FieldLookupTable.InvalidHandle())
		{
			auto elem = m_FieldLookupTable.Element( handle );
			Assert( elem.m_FieldIndex >= 0 && elem.m_FieldIndex < m_nTotalFieldEntries );
			return m_Fields[elem.m_FieldIndex];
		}

		return nullptr;
	}

public:
	struct ExcludeIncludeFilter_t
	{
		CUtlVector<CUtlString> m_ExcludeList;
		CUtlVector<CUtlString> m_IncludeList;
	};

	CUtlStringToken m_nHash;
	CUtlString m_pszClassName;
	CUtlVector<CNetworkSerializerFieldInfo *> m_Fields;
	ExcludeIncludeFilter_t m_NetworkFilterByUserGroup;
	ExcludeIncludeFilter_t m_NetworkFilterByName;

	CUtlHash<SerializerFieldLookup_t, DefaultEqualFunctor<SerializerFieldLookup_t>, DefaultHashFunctor<SerializerFieldLookup_t>> m_FieldLookupTable;
	int m_nTotalFieldEntries;

	CUtlVector<CNetworkSerializerClassInfo *> m_ParentClassInfo;
	CNetworkSerializerClassInfo *m_ParentClassInfoBuffer;
	CUtlVector<int> m_ParentClassOffset;

	struct
	{
		void *m_unk001;
		CUtlLinkedList<void *, int> m_unk002;
	} m_unk001;

	CUtlVector<NetworkOverride_t *> m_NetworkOverrides;
	// Includes this class and parent overreides
	CUtlVector<NetworkOverride_t *> m_NetworkFlattenedOverrides;

	CUtlVector<VarTypeOverride_t *> m_NetworkVarTypeOverrides;
	CUtlVector<SerializedFieldTypeMapping_t *> m_FieldTypeMappings;
	CUtlVector<UserGroupProxy_t *> m_UserGroupProxies;
	CUtlVector<ReplayCompatField_t *> m_NetworkReplayCompatFields;
	CNetworkSerializerCodeGenDatabase *m_pDatabase;

	int32_t m_nClassSize;
	int m_NetworkOutOfPVSUpdates;
	int m_unk101;

	SchemaClassManipulatorFn_t m_pfnManipulator;

	bool m_Initialized;
	bool m_NetworkVarsAtomic;
	bool m_unk201;
	bool m_unk202;
	bool m_NetworkStructNotInNetworkUtlVectorEmbedded;

	CThreadSpinRWLock m_Mutex;
};

class CNetworkSerializerCodeGenDatabase
{
public:
	struct EnumInfo_t
	{
		int32_t m_nValue;
		int8_t m_nFlags;
	};

	CUtlString m_ModuleName;
	CUtlDict<CNetworkSerializerClassInfo *> m_ClassInfos;
	CUtlDict<CNetworkSerializerCodeGenDatabase::EnumInfo_t> m_EnumInfos;
	CUtlDict<CUtlString> m_AtomicTypeMapping;

	bool m_bDebugSpew;

	CUtlString *m_unk001;
	CUtlString *m_unk002;
	CUtlString *m_unk003;
	CUtlString *m_unk004;

	int32_t m_nDuplicateCount;
};

struct EntClassComponentOverride_t
{
    const char* pszBaseComponent;
    const char* pszOverrideComponent;
};

class CEntityClassInfo
{
public:
    const char* m_pszClassname;
    const char* m_pszCPPClassname;
    const char* m_pszDescription;
    CEntityClass* m_pClass;
    CEntityClassInfo* m_pBaseClassInfo;
    CSchemaClassInfo* m_pSchemaBinding;
    datamap_t* m_pDataDescMap;
    datamap_t* m_pPredDescMap;
};

// https://github.com/Wend4r/sourcesdk/commit/cb805ab5d462f79c7d64c06e21b9bc7a7db0bf24
// Size: 0x160
class CEntityClass
{
    struct ComponentOffsets_t
    {
        uint16 m_nOffset;
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
    inline CSchemaClassInfo* GetSchemaBinding() const
    {
        return m_pClassInfo->m_pSchemaBinding;
    }

    inline datamap_t* GetDataDescMap() const
    {
        return m_pClassInfo->m_pDataDescMap;
    }

public:
    using FuncToNameCb = const char *(*)(THINKFUNC think_fn);
	using NameToFuncCb = THINKFUNC (*)(const char *fn_name);
	using RegisterPulseBindingsCb = void (*)(CPulseAPIExtensionRegistrationContext *pContext);
	using EnumerateComponentsCb = void (*)(void *pOut);

    ScriptClassDesc_t* m_pScriptDesc;
	CNetworkSerializerClassInfo* m_pNetworkSerializerInfo;

    EntInput_t* m_pInputs;
    EntOutput_t* m_pOutputs;
    int m_nInputCount;
    int m_nOutputCount;

public:

    CEntitySharedPulseSignature* m_pSharedPulseSignature;

    RegisterPulseBindingsCb m_pfnRegisterPulseBindings;

    NameToFuncCb m_NameToThinkFunc;
	FuncToNameCb m_ThinkFuncToName;

	EnumerateComponentsCb m_pfnEnumerateComponents;

	EntClassComponentOverride_t* m_pComponentOverrides;

    CEntityClassInfo* m_pClassInfo; // 0x50
    CEntityClassInfo* m_pBaseClassInfo;
    CUtlSymbolLarge m_designerName;

    // Uses FENTCLASS_* flags
    uint m_flags;
    int m_nSpawnOrder;

    uint m_nAllHelpersFlags;

    CUtlVector<ComponentOffsets_t> m_ComponentOffsets;
    CUtlVector<ComponentHelper_t> m_AllHelpers;

    ComponentUnserializerClassInfo_t m_componentUnserializerClassInfo;

    FlattenedSerializerDesc_t m_flattenedSerializer;

    CUtlVector<ClassInputInfo_t> m_classInputInfos;
    CUtlVector<ClassOutputInfo_t> m_classOutputInfos;

    CEntityHandle m_requiredEHandle;

    CEntityClass* m_pNext;
    CEntityIdentity* m_pFirstEntity;
    ServerClass* m_pServerClass;
};

#endif // ENTITYCLASS_H
