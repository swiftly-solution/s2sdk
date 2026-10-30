#ifndef NETWORKMESSAGES_H
#define NETWORKMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/bitbuf.h"
#include "public/tier0/bitvec.h"
#include "public/tier0/color.h"
#include "public/tier0/loggingtypes.h"
#include "public/tier0/platform.h"
#include "public/tier0/utldelegate.h"
#include "inetworkserializer.h"
#include "networksystemtypes.h"
#include "inetchannel.h"

template<typename PROTO_TYPE>
class CNetMessagePB;

class CSchemaClassBindingBase;
class NetworkSerializerInfo_t;
class NetworkUnserializerInfo_t;
class NetworkableData_t;
class FieldMetaInfo_t;
class NetworkFieldInfo_t;
struct NetworkFieldResult_t;
struct NetworkableDataType_t;
class INetworkSerializerBindingBuildFilter;

class CNetMessage
{
public:
	// Disabled as per CNetMessagePB note
	CNetMessage() = delete;
	CNetMessage( const CNetMessage & ) = delete;

	virtual ~CNetMessage();

	// Returns the underlying proto object
	virtual void *AsProto() const = 0;
	virtual void *AsProto2() const = 0;

	virtual INetworkMessageInternal *GetNetMessage() const = 0;
	virtual CNetMessage *CopyConstruct() const = 0;
	virtual NetworkMessageId GetMessageId() const = 0;
	virtual const char *GetName() const = 0;

	// Helper function to cast up the abstract message to a concrete CNetMessagePB<T> type.
	// Doesn't do any validity checks itself!
	template<typename T>
	const CNetMessagePB<T> *ToPB() const
	{
		return static_cast<const CNetMessagePB<T> *>(this);
	}

	template<typename T>
	CNetMessagePB<T> *ToPB()
	{
		return static_cast<CNetMessagePB<T> *>(this);
	}

	float GetMargin() const;

private:
	double m_flRecvTime;
	int m_nTick;
	NetChannelBufType_t m_nBufType;
	int m_nBits;
	int m_nInSequenceNr;
	float m_flMargin;
	// NetPacket_t::m_nStreamPosition of the packet it was parsed from
	int64 m_nStreamPosition;
};

template<typename PROTO_TYPE>
class CNetMessagePB : public CNetMessage, public PROTO_TYPE
{
public:
	// Prevent manual construction of this object as per the comment above
	CNetMessagePB() = delete;
	CNetMessagePB( const CNetMessagePB & ) = delete;
};

class IProtobufBinding
{
public:
	virtual const char *GetName() = 0;
	virtual int GetSize() = 0;

	virtual const char *ToString(const void *pData, CUtlString &sResult) = 0;

	virtual const char *GetGroup() = 0;
	virtual Color GetGroupColor() = 0;
	virtual NetChannelBufType_t GetBufType() = 0;

	virtual CNetMessage *AllocateMessage() = 0;

	virtual bool OkToRedispatch() = 0;
	virtual bool unk001() = 0;
};

struct NetMessageInfo_t
{
	int m_nCategories;
	IProtobufBinding *m_pBinding;
	CUtlString m_szGroup;
	NetworkMessageId m_MessageId;
	NetworkGroupId m_GroupId;

	// AMNOTE: From IProtobufBinding::GetBufType()
	NetChannelBufType_t m_nBufType : 6;
	// AMNOTE: Set by FindOrCreateNetMessage's bCannotBeNetworked
	bool m_bCannotBeNetworked : 1;
	uint8_t m_unk001 : 1;

	// AMNOTE: AddCategoryMask ORs nMask into it too when bRequireHandler is true. ConfirmAllMessageHandlersInstalled
	// reports channels in these categories that have no handler registered for the message
	int m_nRequiredHandlerCategories;
	bool m_bOkayToRedispatch;
};

enum NetworkFieldChangedDelegateType_t
{
	NETWORK_FIELD_CHANGED_TYPE_NONE,
	NETWORK_FIELD_CHANGED_NO_VALUE,
	NETWORK_FIELD_CHANGED_WITH_VALUE,
	NETWORK_FIELD_CHANGED_NO_VALUE_NO_CONTEXT,
	NETWORK_FIELD_CHANGED_ARRAY_WITH_VALUE,
	NETWORK_FIELD_CHANGED_ARRAY_NO_VALUE,
	NETWORK_FIELD_CHANGED_REPLAY_COMPAT,
	NETWORK_FIELD_CHANGED_TAG,
	NETWORK_FIELD_CHANGED_COUNT_TYPE
};

#define NETWORK_FIELD_CHANGE_DEFAULT_PRIORITY INT_MIN

// Called on the object whose field changed instead of the bound one, so bind a member function of its class
typedef CUtlDelegate<void ( const CUtlStringToken &tag, const char *pszTag )> NetworkChangeTagDelegate_t;

typedef NetworkFieldResult_t ( *NetworkFieldSerializeCB )(NetworkSerializerInfo_t const&, int, NetworkableData_t *);
typedef NetworkFieldResult_t ( *NetworkFieldUnserializeCB )(NetworkUnserializerInfo_t const&, int, NetworkableData_t *);

typedef NetworkFieldResult_t ( *NetworkFieldSerializeBufferCB )(NetworkSerializerInfo_t const&, bf_write &, int *, void const **, bool);
typedef NetworkFieldResult_t ( *NetworkFieldUnserializeBufferCB )(NetworkUnserializerInfo_t const&, bf_read &, int *, void **, bool, CSchemaClassBindingBase const*);

typedef uint32_t ( *NetworkFieldInfoCB )(NetworkFieldInfo_t const&);
typedef bool ( *NetworkFieldMetaInfoCB )(FieldMetaInfo_t const&, CSchemaClassBindingBase const*, NetworkFieldInfo_t const&, void *);
typedef bool ( *NetworkableDataCB )(NetworkableData_t const*, CUtlString &);
typedef char const* ( *NetworkFieldSerializerNameCB )(void);

// The bit sizes of the network coordinate, normal and angle encodings, so the precision networked positions,
// directions and angles have on clients, and the values derived from them. Clients get the six sizes in
// CSVCMsg_FlattenedSerializer's coord_size_params (ProtoCoordSizeParams_t)
// AMNOTE: Changing them on a running server likely breaks decoding on clients that already got the serializers
struct CoordSizeParams_t
{
	// Coordinates: values within +-(1 << m_nCoordIntegerBits), in steps of m_flCoordResolution
	int m_nCoordIntegerBits;
	int m_nCoordFractionalBits;
	int m_nCoordDenominator; // 1 << m_nCoordFractionalBits
	float m_flCoordResolution; // 1 / m_nCoordDenominator

	// Multiplayer coordinates: values within +-(1 << m_nCoordIntegerBitsMP) take that many integer bits (others
	// m_nCoordIntegerBits), and low precision ones m_nCoordFractionalBitsMPLowPrecision fractional bits
	int m_nCoordIntegerBitsMP;
	int m_nCoordFractionalBitsMPLowPrecision;
	int m_nCoordDenominatorLowPrecision; // 1 << m_nCoordFractionalBitsMPLowPrecision
	float m_flCoordResolutionLowPrecision; // 1 / m_nCoordDenominatorLowPrecision

	// Normals: each component in [-1, 1], in steps of m_flNormalResolution
	int m_nNormalFractionalBits;
	int m_nNormalDenominator; // (1 << m_nNormalFractionalBits) - 1
	float m_flNormalResolution; // 1 / m_nNormalDenominator

	// Angles: m_nAngleBits per component; components smaller than m_flAngleResolution are sent as 0
	int m_nAngleBits;
	int m_nAngleDenominator; // 1 << m_nAngleBits
	float m_flAngleResolution; // 1 / m_nAngleDenominator
	double m_flAngleDegreesToBits; // m_nAngleDenominator / 360
	double m_flAngleBitsToDegrees; // 360 / m_nAngleDenominator

	// bf_read::ReadBitCoordMP's numbits_table and mul_table: the bit counts and multipliers multiplayer coordinates
	// decode with
	uint8_t m_CoordMPNumBits[8];
	float m_CoordMPMultipliers[4];
};

class CSendProxyRecipients
{
public:
	CPlayerBitVec m_Bits;
};

#define NETWORKMESSAGES_INTERFACE_VERSION "NetworkMessagesVersion001"

class INetworkMessages
{
public:
	virtual void RegisterNetworkCategory(NetworkCategoryId nCategoryId, char const *szDebugName) = 0;

	virtual void AssociateNetMessageWithChannelCategoryAbstract(INetworkMessageInternal *pNetMessage, NetworkCategoryId nCategoryId, bool bRequireHandler) = 0;

	// Passing nMessasgeId as -1 would auto-assign the id based on the message name hash.
	// AMNOTE: bCannotBeNetworked marks the message as never sent over the network, Serialize rejects it,
	// and its id is auto-assigned too
	virtual INetworkMessageInternal *FindOrCreateNetMessage(int nMessageId, IProtobufBinding const *pProtoBinding, uint32_t nMessageSize, INetworkSerializerBindingBuildFilter *pUnused, bool bCreateIfNotFound = true, bool bCannotBeNetworked = false) = 0;

	virtual bool Serialize(bf_write &pBuf, const CNetMessage *pData) = 0;

	virtual bool UnserializeMessageInternal( bf_read &pBuf, CNetMessage *pData ) = 0;
	virtual bool SerializeMessageInternal( bf_write &pBuf, const CNetMessage *pData ) = 0;

	// Returns nullptr if failed to unserialize, reason is written to err_reason
	virtual CNetMessage *UnserializeFromStream(bf_read &pBuf, CUtlString &err_reason) = 0;
	virtual bool SerializeAbstract(bf_write &pBuf, INetworkMessageInternal *pNetMessage, const CNetMessage *pData) = 0;

	virtual CNetMessage *AllocateAndCopyConstructNetMessageAbstract(INetworkMessageInternal *pNetMessage, const CNetMessage *pFrom) = 0;

	virtual void DeallocateNetMessageAbstract(INetworkMessageInternal *pNetMessage, CNetMessage *pData) = 0;

	// For both, the last two callbacks are used instead of the first serialize and unserialize callbacks while
	// net_validatemessages is on; null ones fall back to them
	virtual void *RegisterNetworkFieldSerializer(char const *szName, NetworkSerializationMode_t mode, NetworkableDataType_t dataType, int nComponentsCount, NetworkFieldSerializeCB, NetworkFieldUnserializeCB, NetworkFieldInfoCB,
		NetworkFieldMetaInfoCB, NetworkableDataCB, NetworkFieldSerializerNameCB, NetworkFieldSerializeCB, NetworkFieldUnserializeCB) = 0;
	virtual void *RegisterNetworkArrayFieldSerializer(char const *szName, NetworkSerializationMode_t mode, NetworkFieldSerializeBufferCB, NetworkFieldUnserializeBufferCB, NetworkFieldInfoCB,
		NetworkFieldMetaInfoCB, NetworkFieldSerializeBufferCB, NetworkFieldUnserializeBufferCB) = 0;

	virtual NetMessageInfo_t *GetNetMessageInfo(INetworkMessageInternal *pNetMessage) = 0;

	virtual INetworkMessageInternal* FindNetworkMessage(char const *szName) = 0;
	virtual INetworkMessageInternal* FindNetworkMessagePartial(char const *szPartialName) = 0;

	virtual NetworkGroupId FindNetworkGroup(char const *szGroup, bool bCreateIfNotFound = false) = 0;
	virtual int GetNetworkGroupCount() = 0;
	virtual const char *GetNetworkGroupName(NetworkGroupId nGroupId) = 0;
	virtual Color GetNetworkGroupColor(NetworkGroupId nGroupId) = 0;

	virtual void AssociateNetMessageGroupIdWithChannelCategory(NetworkCategoryId nCategoryId, char const *szGroup) = 0;

	virtual void SetNetworkSerializationContextData(char const *szContext, NetworkSerializationMode_t, void *) = 0;
	// Returns 0xFFFF for a context SetNetworkSerializationContextData never set
	virtual uint16_t LookupNetworkSerializationContextDataId(char const *szContext) = 0;
	// Returns the data SetNetworkSerializationContextData set for the context and mode, nullptr for an id of 0xFFFF
	virtual void *GetNetworkSerializationContextData(uint16_t hContext, NetworkSerializationMode_t mode) = 0;

	// AMNOTE: The callback runs for fields with MNetworkChangeTag metadata
	virtual void RegisterNetworkChangeTagCallback(const NetworkChangeTagDelegate_t &callback) = 0;

	// szCallbackName is the name fields refer to in their MNetworkChangeCallback metadata; registering it twice is a fatal error.
	// AMNOTE: szClassName is stored with the callback but not used
	virtual void RegisterNetworkFieldChangeCallbackInternal(char const *szCallbackName, char const *szClassName, NetworkFieldChangedDelegateType_t fieldType, CUtlAbstractDelegate pCallback, NetworkFieldChangeCallbackPerformType_t cbPerformType, int nPriority ) = 0;

	virtual void AllowAdditionalMessageRegistration(bool bAllow) = 0;
	virtual bool IsAdditionalMessageRegistrationAllowed() = 0;

	virtual int GetFieldChangeCallbackOrderCount() = 0;
	virtual void GetFieldChangeCallbackPriorities(int nCount, int *nPriorities) = 0;

	virtual void RegisterFieldChangeCallbackPriority(int nPriority) = 0;

	virtual INetworkMessageInternal *FindNetworkMessageById(int nMessageId) = 0;

	virtual void SetIsForServer(bool bIsForServer) = 0;
	virtual bool GetIsForServer() = 0;

	virtual void RegisterSchemaTypeOverride(uint32_t, char const *) = 0;

	virtual int ComputeOrderForPriority(int nPriority) = 0;

	virtual LoggingChannelID_t GetLoggingChannel() = 0;

	virtual void SetCoordSizeParams(const CoordSizeParams_t *pParams) = 0;
	virtual const CoordSizeParams_t *GetCoordSizeParams() = 0;

	virtual ~INetworkMessages() = 0;
};

#endif