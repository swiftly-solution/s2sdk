#ifndef ENTITYINSTANCE_H
#define ENTITYINSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include "entityidentity.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlvector.h"
#include "public/networksystem/networksystemtypes.h"
#include "public/networksystem/inetworkserializer.h"
#include "variant.h"
#include "entityscriptframework.h"

#include <initializer_list>

class CKV3TransferSaveContext;
class CKV3TransferLoadContext;
struct CEntityPrecacheContext;
struct CPulseArgumentPack;
struct CPulseInputParamMap;
class CEntityKeyValues;

class ISave;
class IRestore;

class IEntityVisitor
{
public:
	// AMNOTE: pField points to the visited member function pointer, a BASEPTR, ENTITYFUNCPTR or USEPTR
	virtual void Visit( CEntityInstance* pEntity, void* pField ) = 0;
};

enum AcceptInputResult_t
{
	ACCEPT_INPUT_NONE = 0, // Not handled, the script's Input<name> function is tried next
	ACCEPT_INPUT_UNHANDLED = 1, // Not handled, and nothing else is tried
	ACCEPT_INPUT_HANDLED = 2,
};

struct NetworkStateChangedData
{
	NetworkStateChangedData();
	explicit NetworkStateChangedData( bool bFullChanged );

	// nLocalOffset is the flattened field offset
	//		calculated taking into account embedded structures
	//		if PathIndex is specified, then the offset must start from the last object in the chain
	// nArrayIndex is the index of the array element 
	//		if the field is a CNetworkUtlVectorBase, otherwise pass -1
	// nPathIndex is the value to specify 
	//		if the path to the field goes through one or more pointers, otherwise pass -1
	// 		this value is usually a member of the CNetworkVarChainer and belongs to the last object in the chain
	NetworkStateChangedData( uint32_t nLocalOffset, int32_t nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() );

	NetworkStateChangedData(const std::initializer_list< uint32_t > nLocalOffsets, int32_t nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t());

	// AMNOTE: 1 when only the fields at m_LocalOffsets changed, 0 marks the whole entity changed (FL_FULL_EDICT_CHANGED)
	uint32_t m_bPartialChange;
	CUtlVector<uint32_t> m_LocalOffsets;
	
	// AMNOTE: Where a whole entity change came from, for its debug log
	CUtlString m_FieldName;
	CUtlString m_FileName;
	int32_t m_nLine;
	int32_t m_nArrayIndex;
	// AMNOTE: -2 for fields culled from the network, whose changes are dropped
	ChangeAccessorFieldPathIndex_t m_nPathIndex;

	// AMNOTE: Set for the field offsets a networked vector sends for one of its elements.
	// CNetworkVarChainer drops such a change when no path index is known
	bool m_bRequiresPathIndex;
	bool m_unk101;
};

struct NetworkStateChangedRemove_t
{
	NetworkStateChangedRemove_t();

	// The changes under these paths, used when m_nFieldOffset is 0
	CUtlVector<ChangeAccessorFieldPathIndex_t> m_PathIndices;
	// Otherwise these elements of the networked vector at m_nFieldOffset
	CUtlVector<int32_t> m_ElementIndices;
	uint32_t m_nFieldOffset;
};

class CEntityInstance
{
public:
	virtual CNetworkSerializerClassInfo* GetNetworkSerializerInfo() = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext* pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext* pContext ) = 0;

	virtual ScriptClassDesc_t* GetScriptDesc() = 0;
	
	virtual ~CEntityInstance() = 0;
	
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void Precache( const CEntityPrecacheContext* pContext ) = 0;
	virtual void AddedToEntityDatabase() = 0;
	virtual void Spawn( const CEntityKeyValues* pKeyValues ) = 0;

	virtual void DispatchPostDataUpdate( /*DataUpdateType_t*/int updateType ) = 0;

	virtual void PostDataUpdate( /*DataUpdateType_t*/int updateType ) = 0;
	virtual void OnDataUnchangedInPVS() = 0;
	virtual void Activate( /*ActivateType_t*/int activateType ) = 0;
	virtual void UpdateOnRemove() = 0;
	virtual void OnSetDormant( /*EntityDormancyType_t*/int prevDormancyType, /*EntityDormancyType_t*/int newDormancyType ) = 0;

	virtual void* ScriptEntityIO() = 0;
	virtual AcceptInputResult_t ScriptAcceptInput( const CUtlSymbolLarge &sInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, const variant_t &value, const CPulseArgumentPack* pArgs, const CPulseInputParamMap* pParamMap ) = 0;
	
	virtual void PreDataUpdate( /*DataUpdateType_t*/int updateType ) = 0;
	
	virtual void DrawEntityDebugOverlays( uint64 debug_bits ) = 0;
	virtual void DrawDebugTextOverlays( void* unk, uint64 debug_bits, int flags ) = 0;
	
	virtual int Save( ISave &save ) = 0;
	virtual int Restore( IRestore &restore ) = 0;
	virtual bool OnSave() = 0;
	virtual void OnRestore() = 0;
	
	// AMNOTE: Visits the entity's stored member function pointers
	virtual void EnumerateVisitor( IEntityVisitor* pVisitor ) = 0;

	virtual int ObjectCaps() = 0;
	virtual CEntityIndex RequiredEdictIndex() = 0;

	// marks a field for transmission over the network
	virtual void NetworkStateChanged( const NetworkStateChangedData& data ) = 0;

	virtual void NetworkStateChangedBranch( const CFieldPath& path ) = 0;
	// AMNOTE: Removes pending changes
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t& data ) = 0;

	// Network state changes are ignored while set to true
	virtual void NetworkUpdateState( bool state ) = 0;
	virtual void NetworkStateChangedLog( const char* pszFieldName, const char* pszInfo ) = 0;

	virtual bool FullEdictChanged() = 0;

	// AMNOTE: Marks the polymorphic metadata helper of the transmit component to be rebuilt
	virtual void InvalidatePolymorphicMetadataHelper() = 0;
	// AMNOTE: Empty in every entity class
	virtual void unk101() = 0;

	virtual ChangeAccessorFieldPathIndex_t AddChangeAccessorPath( const CFieldPath& path ) = 0;
	virtual void AssignChangeAccessorPathIds() = 0;
	virtual ChangeAccessorFieldPathIndexInfo_t* GetChangeAccessorPathInfo() = 0;
	virtual const ChangeAccessorFieldPathIndexInfo_t* GetChangeAccessorPathInfo() const = 0;
	
	virtual bool GetFieldPathChildIndices( const CFieldPath& path, CUtlVector<int>* pChildIndices ) = 0;
	// AMNOTE: Returns false in every entity class
	virtual bool GetAlternateBaselineName( const char **ppszName ) = 0;

	virtual void Pulse_OnDynamicAttributeChanged() = 0;

	virtual void ReloadPrivateScripts() = 0;
	virtual datamap_t* GetDataDescMap() = 0;

	// Returns the member whose class is named sClassName, which isn't necessarily a CEntityComponent
	virtual void* FindComponent( const CUtlSymbolLarge& sClassName ) = 0;
	// AMNOTE: Copies the entity's world group id into its nested members
	virtual void UpdateNestedWorldGroupIds() = 0;

	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;

public:
	CEntityHandle GetRefEHandle() const;
	
	const char *GetClassname() const;

	CEntityIndex GetEntityIndex() const;

public:
	CUtlSymbolLarge m_iszPrivateVScripts;
	CEntityIdentity* m_pEntity;
private:
	void* m_hPrivateScope; // CEntityPrivateScriptScope
public:
	CEntityKeyValues* m_pKeyValues;
	CScriptComponent* m_CScriptComponent;
};

#endif