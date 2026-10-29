#ifndef NETWORKSYSTEMTYPES_H
#define NETWORKSYSTEMTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/utlhashtable.h"

enum NetworkFieldChangeCallbackPerformType_t
{
	NETWORK_FIELD_CHANGE_CALLBACK_PERFORM_ON_CHANGE,
	NETWORK_FIELD_CHANGE_CALLBACK_PERFORM_ON_CHANGE_OR_CREATE
};

struct OffsetIgnore_t
{
	typedef uint16_t OffsetIgnoreValueType_t;

	CUtlHashtable<uint16_t, empty_t> m_OffsetHashtable;
	OffsetIgnoreValueType_t m_unMaxOffset;
};

struct ChangeAccessorFieldPathIndex_t
{
	ChangeAccessorFieldPathIndex_t();
	ChangeAccessorFieldPathIndex_t( int32_t value );

	int32_t m_Value;
};

struct ChangeAccessorFieldPathIndexInfo_t
{
	struct IgnoreCache_t
	{
		uint16_t m_Offsets[16];
		ChangeAccessorFieldPathIndex_t m_FieldPaths[16];
		int m_nCount;
		int m_nFirstElement;
	};

	typedef CUtlLeanVectorFixedGrowable<uint32> PackedFieldPathVec_t;

	PackedFieldPathVec_t m_ChangeAccessorFieldPathIdList;
	const OffsetIgnore_t *m_pBaseOffsetToIgnore;
	CUtlVectorFixedGrowable<const OffsetIgnore_t *, 4> m_OffsetsToIgnoreForPaths;
	IgnoreCache_t m_ShouldIgnore;
	IgnoreCache_t m_ShouldNotIgnore;
};

struct VarChangeInfo_t
{
	uint32_t m_nFieldOffset;
	ChangeAccessorFieldPathIndex_t m_nRootPathIndex;

	// All bits are set when only the whole field changed, and then these hold no element list.
	// Otherwise the changed elements are in VarChangeList_t::m_ChangedElements[m_nElementListIndex]
	uint16_t m_nElementCount : 7;
	uint16_t m_bElementsSorted : 1;
	uint16_t m_nElementListIndex : 7;
	uint16_t m_bFieldChanged : 1; // The whole field changed too
};

enum NetworkSerializationMode_t
{
	NET_SERIALIZATION_MODE_0 = 0x0,
	NET_SERIALIZATION_MODE_1 = 0x1,
	NET_SERIALIZATION_MODE_COUNT = 0x2,
	NET_SERIALIZATION_MODE_DEFAULT = 0x0,
	NET_SERIALIZATION_MODE_SERVER = 0x0,
	NET_SERIALIZATION_MODE_CLIENT = 0x1,
};

enum NetworkOverrideType_t
{
	MNetworkOverride_Invalid = -1,
	MNetworkSerializer = 0,
	MNetworkEncoder,
	MNetworkChangeCallback,
	MNetworkChangeTag,
	MNetworkBitCount,
	MNetworkUserGroup,
	MNetworkPriority,
	MNetworkOutOfPVSUpdates,
	MNetworkRemoveAll,
};

struct NetworkOverride_t
{
	const char *m_ParentClass;
	const char *m_FieldName;
	const char *m_Value;
	NetworkOverrideType_t m_Type;
};

enum NetChannelBufType_t : int8_t
{
	BUF_DEFAULT = -1,
	BUF_UNRELIABLE = 0,
	BUF_RELIABLE,
	BUF_VOICE,
};

typedef void* FlattenedSerializerHandle_t;

typedef int NetworkMessageId;
typedef uint8_t NetworkGroupId;
typedef uint32_t NetworkCategoryId;

#endif