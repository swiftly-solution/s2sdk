#ifndef SCHEMAENUMINFO_DATA_H
#define SCHEMAENUMINFO_DATA_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform_base.h"
#include "schemametadata_data.h"

class CSchemaEnumInfo;
class CSchemaSystemTypeScope;

enum SchemaEnumFlags1_t : uint8_t
{
	SCHEMA_EF1_INFO_IS_INITIALIZED = (1 << 0),
	SCHEMA_EF1_MODULE_LOCAL_TYPE_SCOPE = (1 << 1),
	SCHEMA_EF1_GLOBAL_TYPE_SCOPE = (1 << 2),
	SCHEMA_EF1_UNSIGNED_STORAGE_TYPE = (1 << 3),
	SCHEMA_EF1_IS_FLAGS_ENUM = (1 << 4),
	SCHEMA_EF1_IS_SCOPED_ENUM = (1 << 5)
};

struct SchemaEnumeratorInfoData_t
{
	const char* m_pName;
	
	int64_t m_nValue;
	
	uint16_t m_nNumMetadata;
	SchemaMetadataEntryData_t* m_pMetadata;
};

struct SchemaEnumInfoData_t
{
	CSchemaEnumInfo* m_pPrimaryBinding;
	
	const char* m_pName;
	const char* m_pProjectName;
	
	uint8_t m_nSizeOf;
	uint8_t m_nAlignOf;
	
	uint16_t m_nEnumFlags1;

	uint16_t m_nNumEnumerators;
	uint16_t m_nNumMetadata;
	
	SchemaEnumeratorInfoData_t* m_pEnumerators;
	SchemaMetadataEntryData_t* m_pMetadata;
	
	CSchemaSystemTypeScope* m_pOuterTypeScope;
	
	int64_t m_nMinValue;
	int64_t m_nMaxValue;
};

#endif