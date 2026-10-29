#ifndef ENTITYCOMPONENT_H
#define ENTITYCOMPONENT_H

#if _WIN32
#pragma once
#endif

#include "public/schemalib/schemaenuminfo.h"
#include "public/tier0/smartptr.h"
#include "public/tier0/utlstringtoken.h"
#include "public/schemalib/schemaclassinfo.h"
#include "public/networksystem/inetworkserializer.h"
#include "datamap.h"
#include "componentunserializer.h"

class CEntityClass;
class CEntityComponentHelper;
class CEntityIdentity;
class CEntityInstance;
class CKV3TransferSaveContext;
class CKV3TransferLoadContext;

enum ActivateType_t
{
	ACTIVATE_TYPE_INITIAL_CREATION = 0x0,
	ACTIVATE_TYPE_DATAUPDATE_CREATION = 0x1,
	ACTIVATE_TYPE_ONRESTORE = 0x2,
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0x0,
	DATA_UPDATE_DATATABLE_CHANGED = 0x1,
};

enum EntityDormancyType_t
{
	ENTITY_NOT_DORMANT = 0x0,
	ENTITY_DORMANT = 0x1,
	ENTITY_SUSPENDED = 0x2,
};

struct EntComponentInfo_t
{
	const char* m_pName;
	const char* m_pCPPClassname;
	const char* m_pNetworkDataReferencedDescription;
	const char* m_pNetworkDataReferencedPtrPropDescription;
	int m_nRuntimeIndex;
	uint32_t m_nFlags;
	ComponentUnserializerClassInfo_t m_componentUnserializerClassInfo;
	void* m_pScriptDesc;
	CEntityComponentHelper* m_pBaseClassComponentHelper;
};

struct EntOutput_t
{
	typedef void* (*GetOutputOwnerFn_t)( CEntityInstance* pEntity );

	const char* m_pName;
	uint32_t m_nFlags;
	GetOutputOwnerFn_t m_pfnGetOutputOwner;
	uint32_t m_nOutputOffset;
};

class CEntityComponentHelper
{
public:
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;
	virtual void Finalize() = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> GetSchemaBinding() = 0;
	virtual datamap_t* GetDataDescMap() = 0;
	virtual bool Allocate( CEntityIdentity* pEntity, void* pComponent ) = 0;
	virtual void Free( CEntityIdentity* pEntity, void* pComponent ) = 0;

public:
	uint m_flags;
	EntComponentInfo_t* m_pInfo;
	int m_nPriority;
	CEntityComponentHelper* m_pNext;
};

class CEntityComponent
{
public:
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;
	virtual datamap_t* GetDataDescMap() = 0;
	virtual CNetworkSerializerClassInfo* GetNetworkSerializerInfo() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext* pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext* pContext ) = 0;
	virtual CEntityComponentHelper* GetComponentHelper() = 0;
};

class CEntityOwnerPtr : public CSmartPtr<CEntityInstance, CNullRefCountAccessor>
{
public:
	operator CEntityInstance*();
	operator const CEntityInstance*() const;

	void Set( CEntityInstance* pEntity );
};

#endif