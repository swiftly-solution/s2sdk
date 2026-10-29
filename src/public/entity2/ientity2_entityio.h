#ifndef IENTITY2_ENTITYIO_H
#define IENTITY2_ENTITYIO_H

#if _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/tier0/keyvalues3.h"
#include "public/tier0/utlsymbollarge.h"
#include "entityhandle.h"
#include "public/schemasystem/typescope.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
struct EntOutput_t;

enum EntityIOTargetType_t
{
	ENTITY_IO_TARGET_INVALID = -1,
	ENTITY_IO_TARGET_CLASSNAME = 0,
	ENTITY_IO_TARGET_CLASSNAME_DERIVES_FROM = 1,
	ENTITY_IO_TARGET_ENTITYNAME = 2,
	ENTITY_IO_TARGET_CONTAINS_COMPONENT = 3,
	ENTITY_IO_TARGET_SPECIAL_ACTIVATOR = 4,
	ENTITY_IO_TARGET_SPECIAL_CALLER = 5,
	ENTITY_IO_TARGET_EHANDLE = 6,
	ENTITY_IO_TARGET_ENTITYNAME_OR_CLASSNAME = 7,
};

struct CPulseInputParamMap
{
	KeyValues3 m_KV3;
	bool m_bForwardAllArgs;
};

struct EntityIOConnectionDesc_t
{
	CUtlSymbolLarge m_targetDesc;
	CUtlSymbolLarge m_targetInput;
	CUtlSymbolLarge m_valueOverride;
	CEntityHandle m_hTarget; // Used when m_nTargetType is ENTITY_IO_TARGET_EHANDLE
	EntityIOTargetType_t m_nTargetType;
	int32 m_nTimesToFire; // -1 fires unlimited times
	float m_flDelay;
};

struct EntityIOConnection_t : EntityIOConnectionDesc_t
{
	CPulseInputParamMap m_paramMap;
	bool m_bMarkedForRemoval;
	EntityIOConnection_t* m_pNext;
};

class CEntityIOOutput
{
public:
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext* pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext* pContext ) = 0;

public:
	EntityIOConnection_t* m_pConnections;
	EntOutput_t* m_pDesc;
};

template <class T>
class CEntityOutputTemplate : public CEntityIOOutput
{
public:
	T m_Value;
};

#endif // IENTITY2_ENTITYIO_H
