#ifndef SCHEMACLASSINFO_DATA_H
#define SCHEMACLASSINFO_DATA_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "schemametadata_data.h"

class CSchemaClassInfo;
class CSchemaSystemTypeScope;
class CSchemaType;
class CSchemaType_DeclaredClass;
struct datamap_t;

enum SchemaClassFlags1_t : uint32_t
{
	SCHEMA_CF1_HAS_VIRTUAL_MEMBERS = (1 << 0),
	SCHEMA_CF1_IS_ABSTRACT = (1 << 1),
	SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR = (1 << 2),
	SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR = (1 << 3),
	SCHEMA_CF1_LIMITED_METADATA = (1 << 4),
	SCHEMA_CF1_INFO_IS_INITIALIZED = (1 << 5),
	SCHEMA_CF1_MODULE_LOCAL_TYPE_SCOPE = (1 << 6),
	SCHEMA_CF1_GLOBAL_TYPE_SCOPE = (1 << 7),
	SCHEMA_CF1_CONSTRUCT_ALLOWED = (1 << 8),
	SCHEMA_CF1_CONSTRUCT_DISALLOWED = (1 << 9),
	SCHEMA_CF1_INFO_TAG_MNetworkAssumeNotNetworkable = (1 << 10),
	SCHEMA_CF1_INFO_TAG_MNetworkNoBase = (1 << 11),
	SCHEMA_CF1_INFO_TAG_MIgnoreTypeScopeMetaChecks = (1 << 12),
	SCHEMA_CF1_INFO_TAG_MDisableDataDescValidation = (1 << 13),
	SCHEMA_CF1_INFO_TAG_MClassHasEntityLimitedDataDesc = (1 << 14),
	SCHEMA_CF1_INFO_TAG_MClassHasCustomAlignedNewDelete = (1 << 15),
	SCHEMA_CF1_INFO_TAG_MNonConstructibleClassBase = (1 << 16),
	SCHEMA_CF1_INFO_TAG_MConstructibleClassBase = (1 << 17),
	SCHEMA_CF1_INFO_TAG_MHasKV3TransferPolymorphicClassname = (1 << 18),
};

enum SchemaClassFlags2_t : uint32_t {};

// Works in combination with SchemaGenClassOpsFn_t
enum SchemaClassManipulatorAction_t
{
	// Registers pObject in a schemasystem
	SCHEMA_CLASS_MANIPULATOR_ACTION_REGISTER = 0,
	SCHEMA_CLASS_MANIPULATOR_ACTION_REGISTER_PRE,

	// Allocates object on the heap and constructs it in place, pObject is unused
	SCHEMA_CLASS_MANIPULATOR_ACTION_ALLOCATE,
	// Deallocates pObject
	SCHEMA_CLASS_MANIPULATOR_ACTION_DEALLOCATE,

	// Constructs pObject in place
	SCHEMA_CLASS_MANIPULATOR_ACTION_CONSTRUCT_IN_PLACE,
	// Destructs pObject in place
	SCHEMA_CLASS_MANIPULATOR_ACTION_DESCTRUCT_IN_PLACE,

	// Returns schema binding of pObject
	SCHEMA_CLASS_MANIPULATOR_ACTION_GET_SCHEMA_BINDING,
};

typedef void *(*SchemaGenClassOpsFn_t)(SchemaClassManipulatorAction_t eAction, void *pObject);

struct SchemaClassFieldData_t
{
	const char* m_pName;
	
	CSchemaType* m_pType;
	
	int m_nSingleInheritanceOffset;
	
	uint16_t m_nNumMetadata;
	SchemaMetadataEntryData_t* m_pMetadata;
};

struct SchemaStaticFieldData_t
{
	const char* m_pName;
	
	CSchemaType* m_pType;
	
	void* m_pInstance;
	
	uint16_t m_nNumMetadata;
	SchemaMetadataEntryData_t* m_pMetadata;
};

struct SchemaBaseClassInfoData_t
{
	uint32_t m_nOffset;
	CSchemaClassInfo* m_pClass;
};

struct SchemaClassInfoData_t
{
	CSchemaClassInfo* m_pPrimaryBinding;
	
	const char* m_pName;
	const char* m_pProjectName;
	const char* m_pszCPPName;

	int m_nSizeOf;
	
	uint16_t m_nNumFields;
	uint16_t m_nNumMetadata;
	
	uint8_t m_nAlignOf;
	uint8_t m_nNumBaseClasses;
	
	int16_t m_nMultipleInheritanceDepth;
	int16_t m_nSingleInheritanceDepth;
	
	SchemaClassFieldData_t* m_pFields;
	SchemaBaseClassInfoData_t* m_pBaseClasses;
	datamap_t* m_pDataDesc;
	SchemaMetadataEntryData_t* m_pMetadata;
	
	CSchemaSystemTypeScope* m_pOuterTypeScope;
	CSchemaType_DeclaredClass* m_pTypeOf;
	
	uint32_t m_nClassFlags1;
	uint32_t m_nClassFlags2;
	
	SchemaGenClassOpsFn_t m_pClassOpsFn;
};

#endif // SCHEMACLASSINFO_DATA_H