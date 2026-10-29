#ifndef ENTITYNETWORKABLE_H
#define ENTITYNETWORKABLE_H

#if _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/tier0/utlmap.h"
#include "public/tier0/utlvector.h"
#include "public/const.h"
#include "entityindex.h"
#include "entityidentity.h"

class ServerClass;
class CNetworkTransmitComponent;
class CEntityInstancePolymorphicMetadataHelper;

class CChangeInfoAccessor
{
public:
	uint16_t m_iChangeInfo;
	uint16_t m_iChangeInfoSerialNumber;
};

struct Entity2Networkable_t
{
	ServerClass* m_pServerClass;
	CEntityIdentity* m_pEntity;
	CEntityInstance* m_pEntityInstance;
	CEntityInstance* m_pParentEntity;
	const char* m_pszDesignerName;
	const char* m_pszCPPClassname;
	CNetworkTransmitComponent* m_pTransmitComponent;
	CChangeInfoAccessor* m_pChangeAccessor;
	uint32_t* m_pnChangeFlags;
	CEntityInstancePolymorphicMetadataHelper* m_pMetadataHelper;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	int m_nSerialNumber;
	int m_nEntryIndex;
};

class IEntity2Networkables
{
public:
	virtual ~IEntity2Networkables() = 0;

	virtual bool GetEntity2Networkable( CEntityIndex nEntityIndex, Entity2Networkable_t* pNetworkable ) = 0;
	virtual CUtlOrderedMap<int, Entity2Networkable_t, CDefLess<int>, uint16_t>* GetEntity2Networkables() = 0;

	virtual void unk101() = 0;
	virtual void unk102() = 0;

	virtual CUtlVector<ServerClass*>* GetServerClasses() = 0;
};

#endif