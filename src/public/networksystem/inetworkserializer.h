#ifndef NETWORKSERIALIZER_H
#define NETWORKSERIALIZER_H

#ifdef _WIN32
#pragma once
#endif

#include "inetworkmessages.h"
#include "public/schemasystem/manipulator.h"
#include "public/tier0/bitbuf.h"
#include "public/tier0/platform.h"
#include "public/tier0/utldict.h"
#include "public/tier0/utlhash.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlstringtoken.h"
#include "public/tier0/utlvector.h"
#include "networksystemtypes.h"

#include <memory>

class NetMessageInfo_t;
class CCheckTransmitInfo;
class CEntityInstance;
class CNetworkSerializerClassInfo;
class CNetworkSerializerCodeGenDatabase;
class CNetMessage;

enum NetworkValidationMode_t
{

};

class INetworkMessageInternal
{
public:
	virtual ~INetworkMessageInternal() = 0;

	virtual const char *GetUnscopedName() = 0;
	virtual NetMessageInfo_t *GetNetMessageInfo() = 0;

	virtual void SetMessageId( NetworkMessageId nMessageId ) = 0;

	virtual void AddCategoryMask( int nMask, bool bRequireHandler ) = 0;

	virtual void SwitchMode( NetworkValidationMode_t nMode ) = 0;

	virtual CNetMessage *AllocateMessage() = 0;

	// Calls to INetworkMessages::SerializeMessageInternal
	virtual bool Serialize( bf_write &pBuf, const CNetMessage *pData ) = 0;
	// Calls to INetworkMessages::UnserializeMessageInternal
	virtual bool Unserialize( bf_read &pBuf, CNetMessage *pData ) = 0;
};

struct NetworkRecipientsFilter_t
{
	class CFilterOwner {};
	using FilterCb = void (CFilterOwner::*)(CCheckTransmitInfo *pInfo, CPlayerBitVec &player_mask);

	enum FilterOwnerType : int8_t
	{
		Filter_Other = 1,
		Filter_Entity = 2,
		Filter_FieldOwner = 3
	};

	CFilterOwner *m_FilterOwner;
	FilterCb m_FilterFn;
	const char *m_FilterName;
	FilterOwnerType m_FitlerOwnerType;
};

struct NetworkChangePointerCallback_t
{
	class CCallbackOwner {};
	// AMNOTE: nAction is 1 when the pointer is set, 2 when it's cleared, 3 when the pointee changes in place
	using ChangeCb = void (CCallbackOwner::*)( CEntityInstance *pEntity, uint8_t nAction );

	// AMNOTE: Selects the object m_CallbackFn is called on
	enum CallbackOwnerType : int8_t
	{
		// AMNOTE: The pointer's new value, or the stored owner when it's null
		Callback_Pointee = 1,
		// AMNOTE: The object that contains the pointer field
		Callback_FieldOwner = 2,
		// AMNOTE: The stored owner
		Callback_Other = 3
	};

	const char *m_CallbackName;
	const char *m_ClassName;
	CCallbackOwner *m_CallbackOwner;
	ChangeCb m_CallbackFn;
	CallbackOwnerType m_CallbackOwnerType;
};

enum OOPVSUpdates_t
{
	OOPVSUpdates_OptOut = 0,
	OOPVSUpdates_OptIn,
	OOPVSUpdates_Default,
};

// AMNOTE: Only struct and polymorphic classes can be allocated through m_pfnManipulator
enum NetworkClassKind_t
{
	ClassKind_Struct = 1,
	ClassKind_Polymorphic,
	ClassKind_PolymorphicWithStaticClassInfo,
	ClassKind_Abstract,
};

struct VarTypeOverride_t
{
	const char *m_FieldName;
	const char *m_OverrideType;
};

struct SerializedFieldTypeMapping_t
{
	const char *m_FieldName;
	const char *m_FieldType;
};

struct UserGroupProxy_t
{
	const char *m_ClassName;
	const char *m_UserGroup;
	NetworkRecipientsFilter_t::CFilterOwner *m_ProxyOwner;
	NetworkRecipientsFilter_t::FilterCb m_ProxyFn;
	NetworkRecipientsFilter_t::FilterOwnerType m_ProxyOwnerType;
};

enum NetworkReplayCompatFieldFlags_t : uint8_t
{
	NETWORK_REPLAY_COMPAT_FIELD_REGEX = (1 << 0), // m_FieldPath is a regular expression
	NETWORK_REPLAY_COMPAT_FIELD_AGGREGATED = (1 << 1),
};

struct NetworkReplayCompatField_t
{
	const char *m_FieldPath;
	const char *m_CallbackName;
	const char *m_pszClassName;
	uint8_t m_nFlags; // NetworkReplayCompatFieldFlags_t
};

class CNetworkSerializerFieldInfo
{
public:
	CUtlStringToken m_FieldNameHash;
	const char *m_pszFieldName;
	CUtlString m_pszTypeName;
	CUtlString m_pszRawType;
	CUtlString m_pszEncodedType;
	CUtlStringToken m_ClassNameHash;
	const char *m_pszClassName;
	int32_t m_nFieldSize;
	int32_t m_nFieldOffset;
	CUtlStringToken m_NetworkAliasHash;
	const char *m_NetworkAlias;
	CUtlStringToken m_NetworkTypeAliasHash;
	const char *m_NetworkTypeAlias;

	// AMNOTE: The constructor zeroes it and nothing else writes it
	void *m_unk001;

	const char *m_NetworkSerializer;
	const char *m_NetworkEncoder;

	std::shared_ptr<NetworkRecipientsFilter_t> m_NetworkSendProxyRecipientsFilter;
	std::shared_ptr<NetworkChangePointerCallback_t> m_NetworkChangePointerCallback;

	int8_t m_NetworkPriority;

	SchemaCollectionManipulatorFn_t m_CollectionManipulatorFn;
	CUtlVector<CUtlString> m_NetworkIncludeByUserGroup;
	CUtlVector<CUtlString> m_NetworkChangeCb;
	CUtlVector<CUtlString> m_NetworkChangeTags;

	int m_NetworkBitCount;
	int m_NetworkEncodeFlags;
	int m_NetworkVarEmbeddedFieldOffsetDelta;
	float m_NetworkMin;
	float m_NetworkMax;

	OOPVSUpdates_t m_NetworkOutOfPVSUpdates;
	bool m_NetworkBitCountSet;
	bool m_NetworkVarEmbeddedNotFlattened;

	bool m_NetworkPolymorphic;
	CUtlString m_pszCodeGenType;
	CNetworkSerializerClassInfo *m_TypeClassInfo;

	int m_CodeGenValueTypeSize;

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
	SerializerFieldLookup_t( const char *fieldname );

	CUtlString m_FieldName;
	int m_FieldIndex;
};

template<> struct DefaultEqualFunctor<SerializerFieldLookup_t> { bool operator()( SerializerFieldLookup_t a, SerializerFieldLookup_t b ) const { return a.m_FieldName == b.m_FieldName; } };
template<> struct DefaultHashFunctor<SerializerFieldLookup_t> { unsigned int operator()( SerializerFieldLookup_t a ) const { return HashStringCaseless( a.m_FieldName.String() ); } };

class CNetworkSerializerClassInfo
{
public:
	CNetworkSerializerFieldInfo *FindField( const char *field_name ) const;

public:
	struct ExcludeIncludeFilter_t
	{
		CUtlVector<CUtlString> m_ExcludeList;
		CUtlVector<CUtlString> m_IncludeList;
	};

	// AMNOTE: nAction 0 allocates and constructs a new object, 1 destructs and frees pObject,
	// 2 returns the CNetworkSerializerClassInfo of pObject's most derived class
	typedef void *(*ManipulatorFn_t)( int nAction, void *pObject );

	CUtlStringToken m_nHash;
	const char *m_pszClassName;
	CUtlVector<CNetworkSerializerFieldInfo *> m_Fields;
	ExcludeIncludeFilter_t m_NetworkFilterByUserGroup;
	ExcludeIncludeFilter_t m_NetworkFilterByName;

	CUtlHash<SerializerFieldLookup_t> m_FieldLookupTable;
	int m_nTotalFieldEntries;

	CUtlVectorFixedGrowable<CNetworkSerializerClassInfo *, 1> m_ParentClassInfo;
	CUtlVector<int> m_ParentClassOffset;

	// AMNOTE: Maps member names like __m_pChainEntity and m_PathIndex to their offsets in the class
	CUtlDict<uint16_t> m_MemberOffsets;

	CUtlVector<NetworkOverride_t *> m_NetworkOverrides;
	// Includes this class and parent overreides
	CUtlVector<NetworkOverride_t *> m_NetworkFlattenedOverrides;

	CUtlVector<VarTypeOverride_t *> m_NetworkVarTypeOverrides;
	CUtlVector<SerializedFieldTypeMapping_t *> m_FieldTypeMappings;
	CUtlVector<UserGroupProxy_t *> m_UserGroupProxies;
	CUtlVector<NetworkReplayCompatField_t *> m_NetworkReplayCompatFields;
	CNetworkSerializerCodeGenDatabase *m_pDatabase;

	int32_t m_nClassSize;
	OOPVSUpdates_t m_NetworkOutOfPVSUpdates;
	NetworkClassKind_t m_nClassKind;

	ManipulatorFn_t m_pfnManipulator;

	bool m_Initialized;
	bool m_NetworkVarsAtomic;
	// AMNOTE: Never set, would skip the MNetworkEnable without CNetworkVar warning
	bool m_unk301;
	bool m_NetworkNoBase;
	bool m_NetworkStructNotInNetworkUtlVectorEmbedded;
	bool m_NetworkVarEmbeddedNotFlattened;
	// AMNOTE: Only the constructor writes it
	bool m_unk401;
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
};

#endif