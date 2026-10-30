#ifndef NETWORKTRANSMITCOMPONENT_H
#define NETWORKTRANSMITCOMPONENT_H

#if _WIN32
#pragma once
#endif

#include "public/schemasystem/typescope.h"
#include "public/networksystem/networksystemtypes.h"
#include "public/tier0/platform.h"
#include "public/tier0/threadtools.h"
#include "entityindex.h"
#include "public/const.h"
#include "entitynetworkable.h"
#include "public/game/timedeventmgr.h"

class CKV3TransferSaveContext;
class CKV3TransferLoadContext;
class CEntityInstancePolymorphicMetadataHelper;
class CEventRegister;

class CNetworkTransmitComponent : public IEventRegisterCallback
{
public:
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext* pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext* pContext ) = 0;
	virtual const char* GetClassname() = 0;
	virtual ~CNetworkTransmitComponent() = 0;
public:
	// AMNOTE: One 32-bit word of the PVS cluster bit vector
	struct PVSClusterWord_t
	{
		uint16_t m_nCount;
		uint16_t m_nIndex;
		uint32_t m_nBits;
	};

public:
	// Allocated on first use
	ChangeAccessorFieldPathIndexInfo_t* m_pChangeAccessorFieldPathIndexInfo;

	// While it is registered, state changes are only marked pending until it fires
	CEventRegister* m_pTimerEvent;

	// The PVS clusters the entity's bounds touch, recomputed when m_bPVSClustersDirty is set
	Vector m_vCenter; // Center of those bounds
	int m_nPVSClusterWordCount;
	// A single { 1, 0, 1 } when the bounds touch more than 32 words
	PVSClusterWord_t m_PVSClusterWords[32];

	CThreadRWLock_FastRead m_unk101;

	// AMNOTE: Bounds that the transmit check can cull the entity by distance to
	Vector m_unk102;
	Vector m_unk103;

	// FL_EDICT_CHANGED, FL_FULL_EDICT_CHANGED and the FL_EDICT_ALWAYS, FL_EDICT_DONTSEND or FL_EDICT_PVSCHECK transmit state
	uint32 m_nStateFlags;
	uint8 m_nTransmitStateOwnedCounter;
	bool m_bPendingStateChange;

	// Set together to recompute m_PhysicalSpawnGroupHandle and the PVS clusters when needed
	bool m_bPhysicalSpawnGroupDirty;
	bool m_bPVSClustersDirty;

	bool m_bNetworkUpdatesDisabled;

	// Changed offsets one shared change info holds, MAX_CHANGE_OFFSETS when negative
	int m_nMaxChangedOffsets;

	CEntityInstance* m_pOuter;
	CEntityIndex m_EntityIndex; // Of m_pOuter, -1 until it is set

	CChangeInfoAccessor m_Accessor;

	// The spawn group whose bounds contain the entity, preferring the entity's own spawn group
	SpawnGroupHandle_t m_PhysicalSpawnGroupHandle;

	// AMNOTE: Held while the transmit state is queried from m_pOuter, when no m_nTransmitStateOwnedCounter owner sets it
	CAtomicMutex m_TransmitStateMutex;
	// AMNOTE: Held while the PVS clusters are recomputed
	CAtomicMutex m_PVSClustersMutex;

	CEntityInstancePolymorphicMetadataHelper* m_pPolymorphicMetadataHelper;
	bool m_bPolymorphicMetadataHelperDirty;
};

#endif