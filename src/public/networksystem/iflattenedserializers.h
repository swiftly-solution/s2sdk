#ifndef FLATTENEDSERIALIZERS_H
#define FLATTENEDSERIALIZERS_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/playerslot.h"
#include "public/tier0/utldelegate.h"
#include "public/tier0/utldict.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlsymbollarge.h"
#include "public/tier0/utlvector.h"
#include "public/tier2/fieldpath.h"
#include "inetworkserializer.h"
#include "networksystemtypes.h"

class INetworkSerializerBindingBuildFilter;

struct FlattenedSerializerDesc_t
{
	CUtlSymbolLarge m_name;
	FlattenedSerializerHandle_t m_handle;
};

class CSchemaClassInfo;

// AMNOTE: One class to build a flattened serializer for, FindOrCreateCreateFlattenedSerializers fills
// *m_pSerializer and m_bSerializerAlreadyExisted
struct BuildFlattenedSerializerInfo_t
{
	FlattenedSerializerDesc_t *m_pSerializer;
	CSchemaClassInfo *m_pClassIntrospection;
	INetworkSerializerBindingBuildFilter *m_pFilter;

	// Used for fields whose own and class settings are OOPVSUpdates_Default
	OOPVSUpdates_t m_nDefaultOutOfPVSUpdates;
	NetworkSerializationMode_t m_nMode;

	// Shared by every entry of one call; the check for missing CNetworkVar declarations adds the fields of each struct
	// that has MNetworkEnable but no CNetworkVar, by struct name
	CUtlDict<CUtlString> *m_pMissingNetworkVars;

	bool m_bUseNetworkVars;
	bool m_bForceCannotBeNetworked;
	bool m_bSerializerAlreadyExisted;
};

struct SerializerMatchInfo_t
{
	CUtlString m_networkName;
	FlattenedSerializerDesc_t m_flattenedSerializer;
};

// AMNOTE: One send proxy recipients filter's result for the fields from m_StartPath to m_EndPath
struct SendProxyData_t
{
	const char *m_pszRecipientsFilter; // Its name, "" when unset
	CFieldPath m_StartPath;
	CFieldPath m_EndPath;
	CSendProxyRecipients m_Recipients;
	bool m_bIsRange; // m_StartPath and m_EndPath differ
};

// AMNOTE: The send proxy results of one entity, sorted by m_StartPath
class CSendProxyDataList
{
public:
	CUtlVector<SendProxyData_t> m_Data;

	// AMNOTE: A unique number given once packing is done, kept while the data stays the same and cleared when data is
	// inserted; two lists with the same non-zero number are equal
	uint64 m_nSerialNumber;

	// AMNOTE: Makes the gather give every range no recipients without calling the filters; the game never sets it
	bool m_bNoRecipients;
	// AMNOTE: Set when a range gets no recipients
	bool m_bHasRangeWithoutRecipients;
	// AMNOTE: Set once the whole entity is gathered, after which gathering into it again is skipped
	bool m_bFullyGathered;
};

// AMNOTE: The fields a player starts receiving from one send proxy result to the next, for CalcDelta
struct SendProxyRecipientsDelta_t
{
	SendProxyRecipientsDelta_t();

	CPlayerSlot m_nPlayerSlot;
	const CSendProxyDataList *m_pFrom;
	const CSendProxyDataList *m_pTo;

	// The ranges of m_pTo that m_nPlayerSlot receives and didn't in m_pFrom
	CSendProxyDataList m_NewlyReceived;
	bool m_bHasNewlyReceived;
};

struct PolymorphicMetadataEntry_t
{
	uint32_t m_nFieldPath;
	int32_t m_nVariant; // 1-based index of the variant the field holds

	// AMNOTE: Always set when recorded, never read
	bool m_unk101;
};

struct PolymorphicMetadata_t
{
	int m_nCount;
	PolymorphicMetadataEntry_t *m_pEntries;
	bool m_bNeedsSort; // Set once there are two entries; GatherPolymorphicMetadata sorts them and clears it
};

class bf_read;
class bf_write;
class CSerializedEntity;
class CSVCMsg_FlattenedSerializer;
class IFieldChangeLimitSpew;
class IFlattenedSerializerSpewFunc;
class IFlattenedSerializerSpewListener;
class INetworkFieldChangedEventQueue;
class INetworkFieldScratchData;
class IPolymorphicMetadataHelper;
struct VarChangeList_t;

// AMNOTE: Names array elements in field path spew instead of "[%d]" when it returns a non-empty string
typedef CUtlDelegate<CUtlString ( int, const char *pszName, int nArrayIndex )> ArrayElementNameFormatterDelegate_t;

class IFlattenedSerializers
{
public:
	virtual void FindOrCreateCreateFlattenedSerializers( BuildFlattenedSerializerInfo_t *pInfos, int nCount ) = 0;

	virtual CUtlString GetFullFieldName( const FlattenedSerializerDesc_t &serializer, const CFieldPath &path, void *pObject, int nEntityIndex, NetworkSerializationMode_t nMode, bool bIncludeClassNames ) = 0;
	virtual const char *GetFieldName( const FlattenedSerializerDesc_t &serializer, const CFieldPath &path, IPolymorphicMetadataHelper *pMetadataHelper, NetworkSerializationMode_t nMode, CUtlString *pOut ) = 0;
	virtual const char *GetFieldName( const FlattenedSerializerDesc_t &serializer, int nFieldPath, IPolymorphicMetadataHelper *pMetadataHelper, NetworkSerializationMode_t nMode, CUtlString *pOut ) = 0;

	virtual bool ReadFields( const FlattenedSerializerDesc_t &serializer, bf_read *pBuf, CSerializedEntity *pOut, int nEntityIndex, NetworkSerializationMode_t nMode, int ) = 0;

	// Encode, GatherSendProxyResults and BuildDeltaProperties get the entity being packed (its CEntityInstance) as pObject
	virtual bool Encode( const FlattenedSerializerDesc_t &serializer, CSerializedEntity *pOut, void *pObject, int nEntityIndex, NetworkSerializationMode_t nMode, CSendProxyDataList *pSendProxyData ) = 0;
	virtual bool GatherSendProxyResults( const FlattenedSerializerDesc_t &serializer, void *pObject, int nEntityIndex, NetworkSerializationMode_t nMode, CSendProxyDataList *pSendProxyData ) = 0;
	virtual bool Decode( const FlattenedSerializerDesc_t &serializer, const CSerializedEntity *pFrom, void *pObject, int nEntityIndex, NetworkSerializationMode_t nMode, void *, int nFlags ) = 0;
	virtual int CalcDelta( const FlattenedSerializerDesc_t &serializer, const CSerializedEntity *pFrom, const CSerializedEntity *pTo, CUtlLeanVectorFixedGrowable<uint32> *pOutFieldPaths, int nEntityIndex, NetworkSerializationMode_t nMode, SendProxyRecipientsDelta_t *pRecipientsDelta ) = 0;
	virtual CSerializedEntity *BuildDeltaProperties( const FlattenedSerializerDesc_t &serializer, void *pObject, int nEntityIndex, NetworkSerializationMode_t nMode, const CSerializedEntity *pFrom, const CUtlLeanVectorFixedGrowable<uint32> *pFieldPaths, CUtlLeanVectorFixedGrowable<uint32> *pOutFieldPaths, CSendProxyDataList *pSendProxyData, bool *pbNoFieldsChanged, bool *pbFieldSizeChanged ) = 0;
	virtual bool WriteFieldList( const FlattenedSerializerDesc_t &serializer, bf_write *pBuf, const CSerializedEntity *pEntity, int nEntityIndex, NetworkSerializationMode_t nMode, const CUtlLeanVectorFixedGrowable<uint32> *pFieldPaths, CPlayerSlot nSlot ) = 0;
	virtual bool MergeDeltas( const FlattenedSerializerDesc_t &serializer, const CSerializedEntity *pOld, const CSerializedEntity *pDelta, CSerializedEntity *pOut, int nEntityIndex, NetworkSerializationMode_t nMode, CUtlLeanVectorFixedGrowable<uint32> *pOutFieldPaths ) = 0;
	virtual CSerializedEntity *BuildMergedSerializedEntity( const FlattenedSerializerDesc_t &serializer, void *pPackedDelta, CSerializedEntity *pBase, CUtlLeanVectorFixedGrowable<uint32> *pOutFieldPaths, bool bFreePackedDelta, int nEntityIndex ) = 0;
	virtual int CullFields( const FlattenedSerializerDesc_t &serializer, CPlayerSlot nSlot, int nEntityIndex, const CUtlLeanVectorFixedGrowable<uint32> *pFieldPaths, const CSendProxyDataList *pSendProxyData, CUtlLeanVectorFixedGrowable<uint32> *pOutFieldPaths ) = 0;
	virtual int RemoveArrayElementsOutsideOfArrayMetadataBounds( const FlattenedSerializerDesc_t &serializer, const CSerializedEntity *pEntity, CUtlLeanVectorFixedGrowable<uint32> *pFieldPaths, int nEntityIndex, NetworkSerializationMode_t nMode ) = 0;
	virtual void SpewSerializer( const FlattenedSerializerDesc_t &serializer, NetworkSerializationMode_t nMode, IFlattenedSerializerSpewFunc *pSpew ) = 0;
	virtual bool MakeSerializersMatchByMeta( void *pMeta, const char *pszSerializerName, const FlattenedSerializerDesc_t &serializer, void *, void *, bool ) = 0;
	virtual bool CreateReplayCompatSerializerFromMeta( void *pMeta, const char *pszSerializerName, FlattenedSerializerDesc_t *pOut, void *, void * ) = 0;
	virtual void AssignChangeAccessorPathIds( const FlattenedSerializerDesc_t &serializer, IPolymorphicMetadataHelper *pMetadataHelper, NetworkSerializationMode_t nMode, ChangeAccessorFieldPathIndexInfo_t *pInfo, void *pObject ) = 0;

	// AMNOTE: The change offsets and m_ChangedFieldPaths of pChanges (CEdictChangeInfo::m_Changes on the server)
	// are resolved into pOutFieldPaths, and m_ChangedFieldPaths entries that were already there are removed from it.
	// pUnresolvedChanges, if set, gets the indices of the change offsets that resolve to no field
	virtual void ResolveChanges( const FlattenedSerializerDesc_t &serializer, IPolymorphicMetadataHelper *pMetadataHelper, NetworkSerializationMode_t nMode, const CUtlLeanVectorFixedGrowable<uint32> *pFieldPaths, VarChangeList_t *pChanges, CUtlLeanVectorFixedGrowable<uint32> *pOutFieldPaths, CUtlVector<int> *pUnresolvedChanges ) = 0;
	virtual CFieldPath FindFieldPathByAddress( const FlattenedSerializerDesc_t &serializer, const void *pObject, const void *pField, bool *pbFound ) = 0;
	virtual bool GetFieldPathChildIndices( const FlattenedSerializerDesc_t &serializer, IPolymorphicMetadataHelper *pMetadataHelper, NetworkSerializationMode_t nMode, const CFieldPath &path, CUtlVector<int> *pChildIndices ) = 0;
	virtual bool GatherSendProxyResultsForPath( const FlattenedSerializerDesc_t &serializer, void *pObject, IPolymorphicMetadataHelper *pMetadataHelper, int nEntityIndex, NetworkSerializationMode_t nMode, const CFieldPath &path, CUtlVector<CFieldPath> *pOutFieldPaths ) = 0;
	virtual const OffsetIgnore_t *LookupOffsetsToIgnoreForPath( const FlattenedSerializerDesc_t &serializer, const CFieldPath &path, IPolymorphicMetadataHelper *pMetadataHelper, NetworkSerializationMode_t nMode ) = 0;
	virtual bool IsUnflattenedField( const FlattenedSerializerDesc_t &serializer, const char *pszFieldName ) = 0;
	virtual bool IsFieldNetworked( const FlattenedSerializerDesc_t &serializer, const char *pszFieldName ) = 0;
	virtual bool ValidateSerializedEntity( const FlattenedSerializerDesc_t &serializer, const CSerializedEntity *pEntity, int nEntityIndex, NetworkSerializationMode_t nMode ) = 0;
	virtual void DumpSerializedEntityToConsole( const FlattenedSerializerDesc_t &serializer, const char *pszLabel, const CSerializedEntity *pEntity, int nEntityIndex, NetworkSerializationMode_t nMode ) = 0;
	virtual void DumpSerializedEntityToLines( const FlattenedSerializerDesc_t &serializer, const char *pszLabel, const CSerializedEntity *pEntity, int nEntityIndex, NetworkSerializationMode_t nMode, CUtlVector<CUtlString> *pOutLines ) = 0;
	virtual void AddSpewListener( IFlattenedSerializerSpewListener *pListener ) = 0;
	virtual void RemoveSpewListener( IFlattenedSerializerSpewListener *pListener ) = 0;
	virtual bool WriteSerializerInfos( const CUtlVector<SerializerMatchInfo_t> &serializers, CSVCMsg_FlattenedSerializer *pMsg ) = 0;
	virtual void Report( NetworkSerializationMode_t nMode, bool bVerbose ) = 0;
	virtual void AddArrayElementNameFormatter( const ArrayElementNameFormatterDelegate_t &formatter ) = 0;
	virtual void RemoveArrayElementNameFormatter( const ArrayElementNameFormatterDelegate_t &formatter ) = 0;
	virtual INetworkFieldChangedEventQueue *CreateNetworkFieldChangedEventQueue( INetworkFieldScratchData *pScratchData, IFieldChangeLimitSpew *pChangeLimitSpew ) = 0;

	// AMNOTE: Begin resets every serializer's matched meta variant, End reports the matches
	virtual void BeginSerializerMetaMatching( bool bTrack ) = 0;
	virtual void EndSerializerMetaMatching() = 0;

	// AMNOTE: Disabled fields are skipped when gathering send proxy results, building deltas and gathering polymorphic metadata
	virtual void SetFieldEnabled( const FlattenedSerializerDesc_t &serializer, const char *pszFieldName, bool bEnabled ) = 0;
	virtual void GatherPolymorphicMetadata( const FlattenedSerializerDesc_t &serializer, void *pObject, int nEntityIndex, NetworkSerializationMode_t nMode, PolymorphicMetadata_t *pMetadata ) = 0;

	// AMNOTE: Writes the variant class names of metadata gathered by GatherPolymorphicMetadata, the third argument is unused
	virtual void GetPolymorphicMetadataDescription( const FlattenedSerializerDesc_t &serializer, const PolymorphicMetadata_t *pMetadata, void *, CUtlVector<CUtlString> *pOutLines ) = 0;
	virtual bool HasPolymorphicFields( const FlattenedSerializerDesc_t &serializer ) = 0;
	virtual void PurgeTemporaryData() = 0;
	virtual bool UserGroupHasFields( const FlattenedSerializerDesc_t &serializer, const char *pszUserGroup ) = 0;

	// AMNOTE: The fields MNetworkOutOfPVSUpdates opts in and out
	virtual bool GetOutOfPVSUpdateFields( const FlattenedSerializerDesc_t &serializer, CSendProxyDataList **ppOptIn, CSendProxyDataList **ppOptOut ) = 0;
};

#endif