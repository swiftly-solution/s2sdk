#ifndef COMPONENTUNSERIALIZER_H
#define COMPONENTUNSERIALIZER_H

#if _WIN32
#pragma once
#endif

#include "public/schemalib/schemaenuminfo.h"
#include "public/tier0/utlstringtoken.h"
#include "datamap.h"

struct ComponentUnserializerClassInfo_t;

struct ComponentUnserializerKeyNamesChunk_t
{
	CUtlStringToken m_keyNames[4];
};

struct ComponentUnserializerFieldInfo_t
{
	CSchemaEnumInfo* m_pSchemaEnum;
	const char* m_pKeyName;
	
	uint16_t m_nOffset;
	uint16_t m_nArraySize;
	
	SpawnKeyType_t m_Type;
	
	bool m_bUnserializeAsMatrix : 1;
	bool m_bArrayElement : 1;
	bool m_bRemovedKeyField : 1;
};

struct ComponentUnserializerPtrToClassInfo_t
{
	uint32_t m_nOffset;
	ComponentUnserializerClassInfo_t* m_pClassInfo;
};

struct ComponentUnserializerClassInfo_t
{
	ComponentUnserializerClassInfo_t* m_pBaseClassInfo;

	ComponentUnserializerKeyNamesChunk_t* m_pKeyNamesChunks;
	ComponentUnserializerFieldInfo_t* m_pFieldInfos;
	ComponentUnserializerPtrToClassInfo_t* m_pClassInfoPtrs;

	uint16_t m_nFieldInfoCount;
	uint16_t m_nKeyNamesChunkCount;
	uint16_t m_nClassInfoPtrCount;
};

#endif
